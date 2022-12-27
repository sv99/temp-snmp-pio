/**
  Temperature Control - S.Volkov <https://github.com/svolkov>
  SNMP Agent running in Arduino Uno
  Version 1.0
  Sensor DS18B20
  SNMP Agent Library based on Agentuino and modifies by S.Volkov
  Copyright 2010 Eric C. Gionet <lavco_eg@hotmail.com>

*/
#include <Ethernet.h>
#include <Agentuino.h>
#include <microDS18B20.h>

// name and address
#ifdef DEVICE1
// device 1 - Rentgena
static char locName[20]  = "ren_temp_sensor";
static uint8_t mac[]     = { 0xDE, 0xAD, 0xBE, 0xEF, 0xEE, 0xD0 };
static uint8_t ip[]      = { 192, 168, 1, 11 };
static uint8_t dns[]     = { 192, 168, 1, 233 };
static uint8_t gateway[] = { 192, 168, 1, 202 };
static uint8_t subnet[]  = { 255, 255, 255, 0 };
#else
// device 2 - Bassein
static char locName[20]  = "bas_temp_sensor";
static uint8_t mac[]     = { 0xDE, 0xAD, 0xBE, 0xEF, 0xEE, 0xD1 };
static uint8_t ip[]      = { 192, 168, 1, 11 };
static uint8_t dns[]     = { 192, 168, 1, 233 };
static uint8_t gateway[] = { 192, 168, 1, 202 };
static uint8_t subnet[]  = { 255, 255, 255, 0 };
#endif

// oids
const static char sysUpTime[] PROGMEM      = "1.3.6.1.2.1.1.3.0";  // uptime
const static char sysName[] PROGMEM        = "1.3.6.1.2.1.1.5.0";  // station nome
const static char snmpTempPrefix[] PROGMEM = "1.3.6.1.2.1.6540.";

// device pin configuration
#define DS18B20_PIN 2
// указываем DS_ADDR_MODE для подключения блока адресации
MicroDS18B20<DS18B20_PIN, DS_ADDR_MODE> sensor;
//MicroDS18B20<DS18B20_PIN> sensor;

// SNMP vars
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;
static char notConnected[]  = "Not connected";

// vars
uint32_t prevMillis = millis();
static uint32_t locUpTime    = 0;
static char upTimeString[20] = ""; // uptime buffer
static char strTemp[20]      = ""; // temperature buffer
static uint8_t sensor_addr[8];
//static uint8_t sensor_fixed[] = {0x28, 0x58, 0xAB, 0x1D, 0x0, 0x0, 0x0, 0x77};

void pduReceived()
{
    SNMP_PDU pdu{};
    api_status = Agentuino.requestPdu(&pdu);
    if ( pdu.type == SNMP_PDU_GET && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS ) {

        pdu.OID.toString(oid);

        if ( strcmp_P(oid, sysUpTime ) == 0 ) {
            // uptime
            status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, upTimeString);
            pdu.type = SNMP_PDU_RESPONSE;
            pdu.error = status;
        }
        else if ( strcmp_P(oid, sysName ) == 0 ) {
            // name
            status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
            pdu.type = SNMP_PDU_RESPONSE;
            pdu.error = status;
        }
        else if (strncmp_P(oid, snmpTempPrefix, strlen_P(snmpTempPrefix) ) == 0 ) {
            // temp request - decode sensor address, which sent after prefix
            // 1.3.6.1.2.1.6540. +
            char *addr = oid + strlen_P(snmpTempPrefix);
            int i = 0;
            char *ch;
            while ( (ch = strsep(&addr, ".")) != nullptr) {
                // Serial.println(ch);
                sensor_addr[i] = strtoul(ch, nullptr, 10);
                i += 1;
            }
            if (i != 8) {
                pdu.error = SNMP_ERR_INCONSISTEN_NAME;
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, notConnected);
            } else {
                // get temperature
                sensor.setAddress(sensor_addr);
                //sensor.setAddress(sensor_fixed);
                if (sensor.readTemp()) {
                    sensor.requestTemp();
                    delay(800);
                    float temp = sensor.getTemp();
                    Serial.println(temp);
                    dtostrf(temp, 10, 2, strTemp);
                    // trim left blank symbols
                    char *val = strTemp;
                    while (*val == ' ') {
                        val++;
                    }
                    status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, val);
                } else {
                    pdu.error = SNMP_ERR_BAD_VALUE;
                    status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, notConnected);
                }
            }
            pdu.type = SNMP_PDU_RESPONSE;
            pdu.error = status;
        }
        else {
            // oid does not exist
            // response packet - object not found
            pdu.type = SNMP_PDU_RESPONSE;
            pdu.error = SNMP_ERR_NO_SUCH_NAME;
        }
        Agentuino.responsePdu(&pdu);
    }

    Agentuino.freePdu(&pdu);
}

void print_hex(uint8_t buf[], uint8_t len, const char *prefix) {
    Serial.print(prefix);
    Serial.print('{');
    for (uint8_t i = 0; i < len; i++) {
        Serial.print("0x");
        Serial.print(buf[i], HEX);  // Выводим адрес
        if (i < len - 1) Serial.print(", ");
    }
    Serial.println('}');
}

void print_dec(uint8_t buf[], uint8_t len, const char *prefix) {
    Serial.print(prefix);
    for (uint8_t i = 0; i < len; i++) {
        Serial.print(buf[i]);  // Выводим адрес
        if (i < len - 1) Serial.print(".");
    }
    Serial.println();
}

void setup()
{
    Serial.begin(9600);
    Ethernet.begin(mac, ip, dns, gateway, subnet);
    api_status = Agentuino.begin();
    delay(10);
    if ( api_status == SNMP_API_STAT_SUCCESS ) {
        Agentuino.onPduReceive(pduReceived);
        delay(10);
    }
    // without delay leonardo don't print this
    delay(3000);
    print_hex(mac, 6, "mac: ");
    print_dec(ip, 4,     "ip:     ");
    print_dec(subnet, 4, "subnet: ");
    print_dec(dns, 4,    "dns:    ");
    print_dec(gateway, 4, "gate:   ");
}

void loop()
{
    Agentuino.listen();
    if ( millis() - prevMillis > 1000 ) {
        locUpTime = millis() / 1000;
        dtostrf(locUpTime, 10, 0, upTimeString);
    }
}
