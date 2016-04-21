# FLT
FLT := FlugLeiterTool

This project is intended to be an interface between OGN-/ADS-B-receivers and XCSoar and the airfield manager respectively.

The aim is to provide a nice and handy tool to display all aircrafts, broadcasting FLARM and ADS messages as objects on the manager's info screen. The well known glider-nav-tool XCSoar is commonly used as a presentation application.
That will make it easier for airfield managers to watch over the surrounding  traffic and additional information as are thermal upwinds, usefull for pilots waiting for a good moment to start.
etc...


FLT parses APRS / OGN and ADS-B / SBS messages from SDRs as are OGN receiver and Dump1090, to finally send transcoded NMEA position reports to XCSoar.

It also will provide some backend logic to support data processing and filtering for validating and further processing of received information.  Possible features include discarding bad or duplicate data, discarding far targets and estimating times and probability of arrival at airfield for security purposes.

This is currently in an early dev-version.


Contributions are always welcome.

I'm a not that experienced C++ developer, especially not in therms of multithreading, but always keep learning.


Open issues:::

high priority:

- parse incoming aprs-sentences (regex)
- implement threadsafety for aircrafts's container
- implement backend to manage listet aircrafts
  - process and parse depending on type (ExtendedAircraft / Aircraft) and handle duplicates (prefer Extended)
  - invalidate deprecated aircrafts

low priority:

- fork client connects
- implement faster container for aircrafts (kind of HashVector) -> key/value, key as unique integer, hashing and sizing

 
  
