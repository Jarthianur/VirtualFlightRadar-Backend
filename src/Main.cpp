/*
 * NMEA Protocol:
 * PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
 * PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>(,<ID>)
 * GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
 * //--GLL Latitude,N/S,Longitude,E/W,UTC,Status (A=Valid, V=Invalid),Checksum
 * $GPGGA,UTC,Lat,N/S,Lon,E/W,1,05,1,Alt(above MSL),M,Geoid separation,M,,*checksum
 * $GPGGA,191410,4735.5634,N,00739.3538,E,1,04,4.4,351.5,M,48.0,M,,*45
 * XCSoar connects on port 4353
 */

#include "VFRB.h"
#include "ConfigReader.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

   long double latitude;
   long double longitude;
   int altitude;
   float geoid;
   int out_port;
   int ogn_port;
   int adsb_port;
   std::string ogn_host;
   std::string adsb_host;
   std::string user;
   std::string pass;
   int maxHeight;

   if (argc == 3) {
      ConfigReader cr(argv[2]);
      cr.read();
      latitude = stold(cr.getProperty("latitude", "0.0L"));
      longitude = stold(cr.getProperty("longitude","0.0L"));
      altitude = stoi(cr.getProperty("altitude","0"));
      geoid = stof(cr.getProperty("geoid","0.0"));
      out_port = stoi(cr.getProperty("outport","0"));
      ogn_port = stoi(cr.getProperty("ognport","0"));
      adsb_port = stoi(cr.getProperty("adsbport","0"));
      ogn_host = cr.getProperty("ognhost", "localhost");
      adsb_host = cr.getProperty("adsbhost", "localhost");
      user = cr.getProperty("user", "xxxxx");
      pass = cr.getProperty("pass", "yyyyy");
      maxHeight = stoi(cr.getProperty("maxHeight", "9999999"));
   } else {
      cout << "usage: ./VirtualFlightRadar-Backend -c pathToConfigFile"<< endl;
      return 1;
   }

   VFRB::base_altitude = altitude;    cout<<VFRB::base_altitude<<endl;
   VFRB::base_latitude = latitude;
   VFRB::base_longitude = longitude;
   VFRB::base_geoid = geoid;
   VFRB::global_out_port = out_port;
   VFRB::global_ogn_port = ogn_port;
   VFRB::global_adsb_port = adsb_port;
   VFRB::global_ogn_host = ogn_host;
   VFRB::global_adsb_host = adsb_host;
   VFRB::global_user = user;
   VFRB::global_pass = pass;
   VFRB::filter_maxHeight = maxHeight;
   VFRB::run();

   return 0;
}
