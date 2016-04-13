/*
 * NMEA Protocol:
 * PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
 * PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>(,<ID>)
 * GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
 *
 * //--GLL Latitude,N/S,Longitude,E/W,UTC,Status (A=Valid, V=Invalid),Checksum
 *
 */

#include <iostream>
#include "ADSBin.h"
#include "Aircraft.h"

using namespace std;

int main(int argc, char* argv[]) {
   cout << "INIT connect to ads-b" << endl;
   ADSBin ads("localhost", 30003);
   ads.connect();
   while (1) {
      int error;
      if ((error = ads.readLine()) < 0) {
         cout << error << endl;
         return -1;
      }
      //if (ads.response[4] == '3') {
         cout << ads.response;
      //}

   }
   return 0;
}
