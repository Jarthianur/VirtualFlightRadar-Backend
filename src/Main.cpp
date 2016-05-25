/*
 * NMEA Protocol:
 * PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
 * PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>(,<ID>)
 * GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
 * //--GLL Latitude,N/S,Longitude,E/W,UTC,Status (A=Valid, V=Invalid),Checksum
 * XCSoar connects on port 4353
 */

#include "VFRB.h"
#include "ConfigReader.h"
#include <iostream>

using namespace std;

long double latitude;
long double longitude;
int altitude;
double geoid;
int out_port;
int ogn_port;
int adsb_port;
const char* ogn_host;
const char* adsb_host;
const char* user;
const char* pass;

int main(int argc, char* argv[]) {
   //call: ./Flugleitertool 49.665263 9.003075 110 4353 14580 30003 localhost localhost D5234 12772

   if (argc == 3) {
      ConfigReader cr(argv[2]);
      cr.read();
      latitude = stold(cr.getProperty("latitude", "0"));
      longitude = stold(cr.getProperty("longitude","0"));
      altitude = stoi(cr.getProperty("altitude","0"));
      geoid = stod(cr.getProperty("geoid","0"));
      out_port = stoi(cr.getProperty("outport","0"));
      ogn_port = stoi(cr.getProperty("ognport","0"));
      adsb_port = stoi(cr.getProperty("adsbport","0"));
      ogn_host = cr.getProperty("ognhost", "localhost");
      adsb_host = cr.getProperty("adsbhost", "localhost");
      user = cr.getProperty("user", "xxxxx");
      pass = cr.getProperty("pass", "yyyyy");
   } else if (argc == 12) {
      latitude = stold(argv[1]);
      longitude = stold(argv[2]);
      altitude = stoi(argv[3]);
      geoid = stod(argv[4]);
      out_port = stoi(argv[5]);
      ogn_port = stoi(argv[6]);
      adsb_port = stoi(argv[7]);
      ogn_host = argv[8];
      adsb_host = argv[9];
      user = argv[10];
      pass = argv[11];
   } else {
      cout << "usage: ./VirtualFlightRadar-Backend lat long alt geoid out_port ogn_port adsb_port ogn_host adsb_host user pass"<< endl;
      cout << "or: ./VirtualFlightRadar-Backend -c pathToConfigFile" << endl;
      return 0;
   }

   //VFRB::run(latitude, longitude, altitude, out_port, ogn_port, adsb_port, ogn_host.c_str(), adsb_host.c_str(), user.c_str(), pass.c_str());
   VFRB::run();

   return 0;
}
