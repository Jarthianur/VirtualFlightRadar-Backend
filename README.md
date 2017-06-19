# VirtualFlightRadar-Backend

[![Build Status](https://travis-ci.org/Jarthianur/VirtualFlightRadar-Backend.svg?branch=master)](https://travis-ci.org/Jarthianur/VirtualFlightRadar-Backend)

VFR-B := VirtualFlightRadar-Backend

This project is intended to be an interface between APRS-/ADS-B-receivers, wind-sensors and XCSoar and the airfield manager respectively.

The aim is to provide a powerful backend-service, which receives position reports broadcasted by aircrafts via FLARM or ADS-B, alongside climate information,
and finally sending NMEA reports to any client. These reports may be displayed by any aviation-purposed navigation application, supporting NMEA protocol.
The well-known, free glider navigation tool [XCSoar](https://www.xcsoar.org/) is recommended as such, as it supports all features provided by the VFR-B
and is also available for most common platforms.

Have a look at the [code of conduct](/CODE_OF_CONDUCT.md).

### The core functionalities of the VFR-B are:

+ receive and parse APRS messages from APRSC (FLARM reports)
+ receive and parse ADS-B messages, transcoded into SBS, from any SDR (e.g. [Dump1090mutability](https://github.com/mutability/dump1090))
+ receive and parse NMEA messages from any wind-sensor, integrated in the network (WIMWV, WIMDA)
+ keep track of and manage detected aircrafts, normalizing measuring units
+ serve these information as NMEA sentences on the network

APRSC := APRS server coupled with a SDR, e.g. utilized by [OGN](http://wiki.glidernet.org/ "Open Glider Network")  
SDR := Software Defined Radio

## What are the benefits

The greatest benefit may be, that it assists an airfield manager to survey surrounding traffic.
Also having real-time wind information, much more accurate than a windsock, is a great benefit too.
But it is also useful for pilots waiting, on the ground, for a good moment to start, or to observe their comrades.
Although, **this system is no replacement for airspace surveilance!**
Nevertheless detecting (E-)planes before they cross a winch-start in low altitude, or any other imaginable scenario,
may increase safety and prevent some or other dangerous situations.

## The whole system

**VFR-B is a standalone backend module. To make it to be of use, working installations of, or access to any APRS server,
ASDB-to-SBS decoder and network serving wind-sensor are required.**
Module means one can replace each component with whatever liked, unless it fits to the interfaces.

VFR-B is, for example, used in [AMVR](https://github.com/rueckwaertsflieger/AMVR).

## What about privacy

One aim of VFR-B is to stay, as much as possible, independent from the outside world/internet.
This means no aircraft-, wind reports, or whatever is going to be sent to the internet directly.
**But this depends on how one will use this service.**
Of course, one can configure it to receive APRS from public OGN servers,
but to keep track of local traffic, the traffic information must be sent to OGN first.
Also the internal NMEA server will serve its data from *localhost*, which means, everybody, inside the local network,
is able to fetch it, but no-one outside.
It is also possible to make the machine, which runs the VFR-B, accessible to the public.

## Software requirements

+ GNU compiler g++ (at least 4.9)
+ GNU make
+ boost library (>= 1.55.0)
  + *(explicit built:)*
    + thread
    + system
    + regex
    + chrono
    + signals

**Boost can be installed via package manager**

```bash
$ sudo apt-get update
$ sudo apt-get install libboost-dev \
libboost-system-dev \
libboost-thread-dev \
libboost-regex-dev \
libboost-chrono-dev \
libboost-signals-dev
```

**or later, along with the *install.sh*:**

```bash
./install.sh deps
```

**or manually:**

Download the boost_1_63_0.tar.gz from [here](http://www.boost.org/users/history/version_1_63_0.html).
From Downloads directory unpack it into ~/boost and run its install scripts

```bash
# if not already existing
$ mkdir -p ~/boost/
$ tar xf boost_1_63_0.tar.gz -C ~/boost/
$ cd ~/boost/boost_1_63_0
$ ./bootstrap.sh --prefix=/home/$USER/boost/
$ ./b2 --build-dir=/home/$USER/boost/ --with-system --with-chrono --with-regex --with-signals --with-thread
```

Now *~/boost/boost_1_63_0/* is the boost root directory.

Note that the boost version from package manager may be too old. *( >= 1.55.0 verified to work also)*

## Installation

Clone or download this repository.  
Before installing the program, one should adjust the values in
[Parameters.h](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/src/util/Parameters.h).
These parameters have to stay fixed, so they can not be changed after compilation.
Please read the comments carefully, as these values change the programs behavior, or may even break it.

From inside the projects root directory follow these steps.

Either adjust the [vfrb.ini](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/vfrb.ini)
file right now to deploy it ready-to-use, or later.  
Edit the [bootstrap.sh](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/bootstrap.sh) according to Your needs.  
There are just a few variables, which need to be set as stated in their comments.  
Next run `./install.sh` and look at its output, or run `./install.sh service` if You want it as a systemd service.  
If all requirements are met, the VFR-B was successfully built, stored where specified and a systemd service was properly configured.

If not already done, now the runtime configuration must be specified in the target INI file.
This INI file was also stored according to the path in bootstrap.sh .

## What to configure

The INI file has entries like this
>latitude=49.000000  
longitude=8.000000  
altitude=400  
geoid=47.0  
pressure=1013.25  
temp=15.0  
serverPort=1234  
aprscHost=localhost  
aprscPort=14580  
aprscLogin=user x pass x  
sbsHost=  
sbsPort=x  
climateSensorHost=nA  
climateSensorPort=0  
maxHeight=-1  
maxDist=40000

There are base position parameters, connection parameters and climate fallback values, along with filters.
To disable any input feed, just leave corresponding hostnames empty, or set them to *'nA'* .
Same with the filters, to disable leave it empty, or set to *-1* .
Additional information, like units, can be found in the comments.
Comments begin with a *;* , feel free to add comments containing whatever liked, e.g. available ports and hosts.

## How to run

### manually

Don't care about the following, if boost was installed via package manager.
Boost is linked dynamically, which means the *LD_LIBRARY_PATH* variable has to be set to boost libs path.
This is done by the install.sh automatically through appending the export instruction to *~/.bashrc* .
To apply this change reboot the system.

```bash
$ {path to binary} -c {path to config file} > {path to log file}  2>&1 &
```

**example:**

```bash
$ ./vfrb -c vfrb.ini > vfrb.log 2>&1 &
```

The log will be in the specified file.

### as service

By invoking `./install.sh service`, the systemd service for VFR-B was automatically configured to start after boot.  
To disable it call, where *servicename* is like *VFRB_NAME_B* from bootstrap.sh.

```bash
$ sudo systemctl disable {servicename}.service
```

**Run the service once by**

```bash
$ sudo service {servicename} start
```

**Watch the log with**

```bash
$ journalctl -u {servicename}.service
```

**Examples:**

```bash
$ sudo service vfrb start
# ------
$ sudo systemctl disable vfrb.service
# ------
$ journalctl -u vfrb.service
```

## Future plans

+ setup cross platform build for RaspPi
+ probability-of-arrival evaluation, displayed via warning levels
+ read *gpsd*
+ read pressure sensor
+ compute missing aircraft data

Contributions are always welcome.

---
Status quo:

Version 2.0.0  
Read more in the [CHANGELOG.md](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/CHANGELOG.md).
