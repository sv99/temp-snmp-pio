Temperature SNMP Sensor
=======================

[PlatformIO](https://platformio.org/) based SNMP temperature sensor.

The sensor measure temperature from I2C connected sensors (DS18B20) by the address.
The sensor address sended in the oid suffix 1.3.6.1.2.1.6540.{sensor_address}.
For example 1.3.6.1.2.1.6540.40.255.33.49.101.4.0.17 for address 40.255.33.49.101.4.0.17.

The sensor address printed in the [sv99/temp-i2c-addr-pio](https://github.com/sv99/temp-i2c-addr-pio).

Device name and IP address fixed in the binary.

Hardware
--------

* Arduino UNO/Leonardo
* Ethernet Shield W5100
* DS18B20 I2C temperature sensor

Ethernet shield need setup mac address.

Packages
--------

* gyverlibs/microDS18B20@^3.10
* patricklaf/SNMP@^1.1.0
* [agentuino](https://code.google.com/archive/p/agentuino) - in the local lib folder

SNMP requests
-------------

Agentuino supported SNMP protocol v1. Other library with support protocol v2c not worked on my boards.
Agentuino library patched.

```bash
# get sysUpTimeInstance
snmpget -v 1 -c public 192.168.1.11 1.3.6.1.2.1.1.3.0
# get sysName.0
snmpget -v 1 -c public 192.168.1.11 1.3.6.1.2.1.1.5.0
# get temperature
snmpget -v 1 -c public 192.168.1.11 1.3.6.1.2.1.6540.1.2.3.4.5.6.7.8
snmpget -v 1 -c public 192.168.1.11 1.3.6.1.2.1.6540.40.255.33.49.101.4.0.17
```

ToDo
----

Store values in the EEPROM.
