Temp SNMP Sensor
================

[PlatformIO](https://platformio.org/) based Ethernet temperature sensor.

Request temperature for I2C sensor by the address from oid 1.3.6.1.2.1.6540.{sensor_address}.
Sensor address printed in the temp-i2c-addr-pio.

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

```bash
# get uptime
snmpget -v 1 -c public 192.168.1.11 1.3.6.1.2.1.1.5.0
# get sysName
snmpget -v 1 -c public 192.168.1.11 1.3.6.1.2.1.1.5.0
# get temperature
snmpget -v 1 -c public 192.168.1.11 1.3.6.1.2.1.6540.1.2.3.4.5.6.7.8
```

ToDo
----

Store values in the EEPROM.
