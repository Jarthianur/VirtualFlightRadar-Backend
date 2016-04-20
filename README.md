# FLT
FLT := FlugLeiterTool

This project is intended to be an interface between OGN-/ADS-B-recievers and XCSoar.
The aim is to provide a nice and handy tool for my aviation-club to display all aircrafts, supporting FLARM or at least transponders,
in the well known Glider-Nav-tool XCSoar.
That will make it easier for air-traffic-controllers to overwatch the ongoing traffic and also provides information about thermal upwinds,
which is usefull for pilots waiting for a good moment to start.
etc...


It recieves and parses OGN/ADS-B messages and sends them to an XCSoar client using multiple threads.

It also will provide some backend logic to be extended with collision-warning.

This is currently in an early dev-version, so I'm just playing around with it and GitHub.

Open issues:
- parsing incoming aprs-sentences (regex)
- fork client connects
- implement threadsafe container for aircrafts
- implement backend to manage listet aircrafts
  - merge incoming objects
  - remove duplicates
  - invalidate deprecated aircrafts
  - filter out keepalive beacon
  
If someone else has a look at this project, I don't handle this too professional for now, but advices are always welcome.

I'm not that experienced C++ developer, especially not in therms of multithreading, but always keep learning.
  
