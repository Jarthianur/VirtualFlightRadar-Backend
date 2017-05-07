# changelog

#### 2.1.0

+ support GPSD
+ common sensor interface (support [BME280 driver](https://github.com/Jarthianur/sensorics))
+ generic input feeds
+ ! configuration refactoring

#### 2.0.1

+ latitude/longitude parsing fixed

#### 2.0.0

+ integrate boost
+ major refactoring, packaging
+ changed logging format
+ changed config format
+ signal handling
+ disable filters via properties
+ systemd service
+ installation script
+ unit tests

#### 1.3.0

+ ADS-B (SBS) input
+ APRS (OGN flavor) input
+ Wind input and output (WIMWV, WIMDA NMEA sentences)
+ height, distance filters
+ static GPS output
+ PFLAU / PFLAA NMEA output
+ all I/O's and parameters are configurable via config file
+ multi client support