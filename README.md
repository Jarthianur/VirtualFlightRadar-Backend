# VirtualFlightRadar-Backend (VFR-B)

[![Build Status](https://travis-ci.org/Jarthianur/VirtualFlightRadar-Backend.svg?branch=master)](https://travis-ci.org/Jarthianur/VirtualFlightRadar-Backend)

This project is intended to be an interface between APRS-/ADS-B-receivers, weather-sensors and XCSoar and the airfield manager respectively.

The aim is to provide a powerful backend-service, which receives position reports broadcasted by aircrafts via FLARM or ADS-B, alongside climate information,
and finally sending NMEA reports to any client. These reports may be displayed by any aviation-purposed navigation application, supporting NMEA protocol.
The well-known, free glider navigation tool [XCSoar](https://www.xcsoar.org/) is recommended as such, as it supports all features provided by the VFR-B
and is also available for most common platforms.

Have a look at the [code of conduct](CODE_OF_CONDUCT.md).

## Content

+ [Features](#features)
+ [Benefits](#what-are-the-benefits)
+ [VirtualFlightRadar](#virtualflightradar---the-whole-system)
+ [Privacy](#what-about-privacy)
+ [Installation](docs/installation.md)
+ [Configuration](docs/configuration.md)
+ [Operation](docs/operation.md)

## Features

+ receive and parse APRS messages from APRSC (FLARM reports)
+ receive and parse ADS-B messages, transcoded into SBS, from any SDR (e.g. [Dump1090mutability](https://github.com/mutability/dump1090))
+ receive and parse NMEA messages from any sensor, integrated in the network (WIMWV, WIMDA)
+ receive and parse NMEA messages from GPSD
+ *Ground-Mode*; the GPS feed stops as soon as a good position is received
+ generic configuration of input feeds, even of same type, handled with priorities
+ keep track of and manage detected aircrafts, normalizing measuring units
+ filter aircrafts by their height and distance
+ serve these information as NMEA sentences on the network

APRSC := APRS server coupled with a SDR, e.g. utilized by [OGN](http://wiki.glidernet.org/ "Open Glider Network")  
SDR := Software Defined Radio

## What Are The Benefits

The greatest benefit may be, that it assists an airfield manager to survey surrounding traffic.
Also having real-time wind information, much more accurate than a windsock, is a great benefit too.
But it is also useful for pilots waiting on the ground for a good moment to start, or to observe their comrades.
Although, **this system is no replacement for airspace surveilance!**
Nevertheless detecting (E-)planes before they cross a winch-start in low altitude, or any other imaginable scenario,
may increase safety and prevent some or other dangerous situations.

## VirtualFlightRadar - The Whole System

**VFR-B is a standalone backend module. To make it to be of use, working installations of, or access to any APRS server,
ASDB-to-SBS decoder and network serving wind/pressure sensor are required.**
Module means one can replace each component with whatever liked, unless it fits to the interfaces.

VFR-B is, for example, used in [AMVR](https://github.com/rueckwaertsflieger/AMVR).

## What About Privacy

One aim of VFR-B is to stay, as much as possible, independent from the outside world (Internet).
That means no aircraft-, wind reports, or whatever is going to be sent to the Internet directly.
**But this depends on how one will use this service.**
Of course, one can configure it to receive APRS from public OGN servers,
but to keep track of local traffic, the traffic information must be sent to OGN first.
Also the internal NMEA server will serve its data from *localhost*, which means, everybody, inside the local network,
is able to fetch it.
It is also possible to make the machine, which runs the VFR-B, accessible to the public.

Contributions are always welcome.

---
Status quo:

Version **2.1.0**  
Read more in the [CHANGELOG.md](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/CHANGELOG.md).
