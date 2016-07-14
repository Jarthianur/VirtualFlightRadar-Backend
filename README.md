# VFR-Backend
VFR-B := VirtualFlightRadar-Backend

This project is intended to be an interface between OGN-/ADS-B-receivers and XCSoar and the airfield manager respectively.

The aim is to provide a nice and handy tool to display all aircrafts, broadcasting FLARM and ADS messages as objects on the manager's info screen. The well known glider-nav-tool XCSoar is commonly used as a presentation application.
That will make it easier for airfield managers to watch over the surrounding  traffic and additional information as are thermal upwinds, usefull for pilots waiting for a good moment to start.
etc...


VFR-B parses APRS / OGN and ADS-B / SBS messages from SDRs as are OGN receiver and Dump1090, to finally send transcoded NMEA position reports to XCSoar.

It also will provide some backend logic to support data processing and filtering for validating and further processing of received information.  Possible features include discarding bad or duplicate data, discarding far targets and estimating times and probability of arrival at airfield for security purposes.

Visit https://github.com/rueckwaertsflieger/AMVR for the super project and even more information.
There You also get all recommended guides and required software.


Contributions are always welcome.

Status quo:

Version 1.3.0

Read more in the FEATURES.
You can find an installation guide in the wiki.
Note that this source is, at the moment, only made for Linux, as it is intended to run on a Raspberry Pi, or similar.

To run the VFR-B (in terminal): 
>$ ./{path to binary} -c {path to config file} > {path to log file} &<br><br>
>example:<br>
>$ ./VFR-Backend -c properties.conf > vfrb.log &<br>
>// This runs the tool, logs all output to the log file and puts the process into background






  
