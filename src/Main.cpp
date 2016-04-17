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
#include "Aircraft.h"
#include "ADSBParser.h"
#include "ConnectorADSB.h"

using namespace std;

int main(int argc, char* argv[]) {
   int out = 0;
   if (argc < 2) {
      cout << "No otput port given!"<<endl;
      return 0;
   }
   ConnectorADSB ads("localhost", 30003, std::atoi(argv[1]));
   ParserADSB parser;

   if (ads.connectIn() == -1) return 0;

   if (argc > 2 && (strcmp(argv[2], "-out")==0)) {
      if (ads.connectOut() == -1) return 0;
      out = 1;
   }

   while (1) {
      int error;
      if ((error = ads.readLineIn()) < 0) {
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
            std::string str;
            parser.process(ac, str, 49.665263L, 9.003075L, 110);
            //cout << str << endl;
            if (out == 1) ads.sendMsgOut(str);
         }
      }
   }
   return 0;
}
