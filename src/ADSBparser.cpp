/*
 * ADSBparser.cpp
 *
 *  Created on: 13.04.2016
 *      Author: lula
 */

#include "ADSBparser.h"

ADSBparser::ADSBparser()
{
   // TODO Auto-generated constructor stub

}

ADSBparser::~ADSBparser()
{
   // TODO Auto-generated destructor stub
}

int ADSBparser::unpack(Aircraft& ac, std::string& sentence)
{
   std::string msg = sentence;
   /*
    * fields:
    * 4 : id
    * 11: altitude
    * 14: latitude
    * 15: longitude
    */
   msg.erase(0,6);
   int delim, index = 2;
   //last param in msg is unprocessed
   while((delim = msg.find(',')) > -1) {
      switch(index) {
         case 4:
            ac.id = msg.substr(0,delim);
            break;
         case 11:
            if (msg.substr(0,delim).length() > 0)
               ac.altitude = std::stoi(msg.substr(0,delim), nullptr);
            else return -1;
            break;
         case 14:
            if (msg.substr(0,delim).length() > 0)
               ac.latitude = std::stod(msg.substr(0,delim), nullptr);
            else return -1;
            break;
         case 15:
            if (msg.substr(0,delim).length() > 0)
               ac.longitude = std::stod(msg.substr(0,delim), nullptr);
            else return -1;
            break;
         default:
            break;
      }
      index++;
      msg.erase(0,delim+1);
   }
   return 0;
}

void ADSBparser::process(Aircraft& ac, std::string& dststr, double baselat,
      double baselong, int basealt)
{
   double lon1 = radian(baselong);
   double lon2 = radian(ac.longitude);
   double lat1 = radian(baselat);
   double lat2 = radian(ac.latitude);
   double distlon = lon2 - lon1;
   double distlat = lat2 -lat1;
   double a = pow(sin(distlat/2),2) + cos(lat1) * cos(lat2) * pow(sin(distlon/2),2);
   double c = 2 * atan2(sqrt(a), sqrt(1 - a));
   distance = 6371000 * c;

   double bearing = atan2(sin(lon2-lon1)*cos(lat2), cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lon2-lon1));
   relBearing = degree(bearing);
   int absBearing = (relBearing + 360) % 360;

   relNorth = cos(radian(absBearing)) * distance;
   relEast = sin(radian(absBearing)) * distance;
   relVert = ac.altitude / 3.2808 - basealt;

   dststr.clear();
   std::stringstream strs;
   strs << "$PFLAU,,,,1,0,";
   strs << relBearing << ",0,";
   strs << ((int) (relVert + 0.5)) << ',';
   strs << ((int) (distance + 0.5)) << ',';
   strs << ac.id << '*';
   dststr = strs.str();
   int csum = checksum(dststr);
   strs << std::setfill('0') << std::setw(2) << std::hex << csum;
   strs << "\r\n";
   dststr = strs.str();

   return;
}

double ADSBparser::radian(double deg)
{
   return ((deg * PI) / 180);
}

int ADSBparser::degree(double rad)
{
   return (int)(((rad * 180) / PI) + 0.5);
}

int ADSBparser::checksum(std::string& sentence)
{
   int csum = 0;
   int tmp;
   int end = ((tmp = sentence.find_last_of('*')) > -1) ? tmp : 0;
   for (int i = 1; i < end; ++i) {
      csum ^= (int) sentence[i];
   }
   return csum;
}
