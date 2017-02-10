# VirtualFlightRadar-Backend

VFR-B := VirtualFlightRadar-Backend

This project is intended to be an interface between APRS-/ADS-B-receivers, wind-sensors and XCSoar and the airfield manager respectively.

The aim is to provide a powerful backend-service, which receives position reports broadcasted by aircrafts via FLARM or ADS-B, alongside climate information,
and finally sending NMEA reports to any client. These reports may be displayed by any aviation-purposed navigation application, supporting NMEA protocol.
The well-known, free glider navigation tool [XCSoar](https://www.xcsoar.org/) is recommended as such, as it supports all features provided by the VFR-B
and is also available for most common platforms.

### The core functionalities of the VFR-B are:

+ receive and parse APRS messages from APRSC (FLARM reports)
+ receive and parse ADS-B messages, transcoded into SBS, from any SDR (e.g. [Dump1090](https://github.com/antirez/dump1090))
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

VFR-B is one of the subsystems of [AMVR](https://github.com/rueckwaertsflieger/AMVR).
All necessary information, **required software/hardware and a setup-guide can be found in its [wiki](https://github.com/rueckwaertsflieger/AMVR/wiki)**.
The system is kept modular, that means one can replace each component with whatever liked, unless it fits to the interfaces.
Please consider, even that the VFR-B is released, the AMVR isn't.
It is still under heavy development. Thus we are not able to provide an easy step-by-step guide, yet.

## What about privacy

One aim of VFR-B (AMVR) is to stay, as much as possible, independent from the outside world/internet.
This means no aircraft-, wind reports, or whatever is going to be sent to the internet directly.
**But this depends on how one will use this service.**
Of course, one can configure it to receive APRS from public OGN servers,
but to keep track of local traffic, the traffic information must be sent to OGN first.
Also the internal NMEA server will send its data to *localhost*, which means, everybody, inside the local network, is able to fetch it.
It is also possible to make the machine, which runs the VFR-B, accessible to the public.
Note that the VFR-B itself, may run completely without internet access, but some things, like a NTP-server,
may be required to run other sub components of the AMVR.

## What to configure

The configuration file looks like this
>latitude=49.000000  
>longitude=8.000000  
>altitude=400  
>geoid=47.0  
>pressure=1013.25  
>temp=15.0  
>serverPort=1234  
>aprscHost=localhost  
>aprscPort=14580  
>aprscLogin=user x pass x  
>sbsHost=  
>sbsPort=x  
>climateSensorHost=nA  
>climateSensorPort=0  
>maxHeight=-1  
>maxDist=40000

There are base position parameters, connection parameters and climate fallback values, along with filters.
To disable any input feed, just leave corresponding hostnames empty, or set them to *'nA'* .
Same with the filters, to disable leave it empty, or set to *-1* .
Additional information, like units, can be found in the *properties.conf* file's comments.
Comments begin with a *#* , feel free to add comments containing whatever liked, e.g. available ports and hosts.

Before installing the program, one should adjust the values in
[Parameters.h](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/dev-2/src/util/Parameters.h).
These parameters have to stay fixed, so they can not be changed after compilation.
Please read the comments carefully, as these values change the programs behavior, or may even break it.

## Software requirements

+ GNU compiler g++ (at least 4.9)
+ GNU make
+ boost library (1.63.0)
  + *(explicit built:)*
  + thread
  + system
  + regex
  + chrono
  + signals

-> [How to install boost](http://www.boost.org/doc/libs/1_63_0/more/getting_started/index.html)

## Installation

A step-by-step guide can be found [here](https://github.com/Jarthianur/VirtualFlightRadar-Backend/wiki/Installation-Guide "wiki").
If all requirements are met, simply `cd` into projects *bin/* directory and run `make`.
Note that the build is currently configured just for Unix like systems, yet...

## How to run

### manually

```bash
$ ./{path to binary} -c {path to config file} > {path to log file}  2>&1 &
```

example:

```bash
$ ./vfrb_2-0-0-SNAPSHOT -c properties.conf > vfrb.log 2>&1 &
```

### as service

hja

## Future plans

+ setup build for all common platforms, like Windows, Mac ...
+ change pressure fallback value to QNH
+ service to change configuration on-the-fly
+ probability-of-arrival evaluation, displayed via warning levels
+ read *gpsd*
+ compute missing aircraft data

Contributions are always welcome.

---
Status quo:

Version 2.0.0-SNAPSHOT  
Read more in the [CHANGELOG.md](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/dev-2/CHANGELOG.md).