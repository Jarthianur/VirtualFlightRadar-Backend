/*
 * NMEA Protocol:
 * PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
 * PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>(,<ID>)
 * GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
 *
 * //--GLL Latitude,N/S,Longitude,E/W,UTC,Status (A=Valid, V=Invalid),Checksum
 * XCSoar connects on port 4353
 */

#include <iostream>
#include <cstring>
#include "Aircraft.h"
#include "ConnectorADSB.h"
#include "ParserADSB.h"

using namespace std;

int main(int argc, char* argv[]) {
   int out = 0;
   long double lat, lon;
   int anth;
   if (argc > 4) {
      lon = std::stold(argv[1], nullptr);
      lat = std::stold(argv[2], nullptr);
      anth = std::stoi(argv[3]);
   } /*else {
      cout << "usage: ./Flugleitertool lon lat antennaheight port -out"<<endl;
      return 0;
   }*/
   ConnectorADSB ads("localhost", 30003, std::stoi(argv[/*4*/1]));
   ParserADSB parser;

   if (ads.connectIn() == -1) return 0;

   if (/*(argc == 6) && (strcmp(argv[5], "-out")==0)*/argc == 3 &&(strcmp(argv[2], "-out")==0)) {
      if (ads.connectOut() != 0) return 0;
      if (ads.connectClient() != 0) return 0;
      out = 1;
   }
   cout << "Scan for incoming msgs..." << endl;
   while (1) {
      int error;
      if ((error = ads.readLineIn(ads.getAdsbInSock())) < 0) {
         cout << error << endl;
         return -1;
      }
      if (ads.getResponse()[4] == '3') {
         //cout << ads.getResponse();
         Aircraft ac;
         if (parser.unpack(ac, ads.getResponse()) == 0) {
            //cout << "Aircraft " << ac.id << ", alt=" << std::to_string(ac.altitude);
            //cout << ", lat=" << std::to_string(ac.latitude) << ", long=" << std::to_string(ac.longitude);
            //cout << endl;
            parser.process(ac, 49.665263L, 9.003075L, 110);
            //parser.process(ac, str, lat, lon, anth);
            cout << ac.nmea_str << endl;
            if (out == 1) ads.sendMsgOut(ac.nmea_str);
         }
      }
   }
   return 0;
}
