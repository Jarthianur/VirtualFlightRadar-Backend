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
   char buff[256];
   //PFLAU
   snprintf(buff, 256, "$PFLAU,,,,1,0,%d,0,%d,%u,%s*", relBearing,
         dtoi(relVert), dtoi(distance), ac.id.c_str());
   int csum = checksum(buff);
   dststr.append(buff);
   snprintf(buff, 64, "%02x\r\n", csum);
   dststr.append(buff);
   //PFLAA
   snprintf(buff, 256, "$PFLAA,0,%d,%d,%d,1,%s,,,,,8*", dtoi(relNorth),
         dtoi(relEast), dtoi(relVert), ac.id.c_str());
   csum = checksum(buff);
   dststr.append(buff);
   snprintf(buff, 64, "%02x\r\n", csum);
   dststr.append(buff);
   //GPRMC
   latstr = (baselat < 0)? 'S' : 'N';
   longstr = (baselong < 0)? 'W' : 'E';
   latdeg = abs(floor(baselat));
   latmin = abs(60 * (baselat - latdeg));
   longdeg = abs(floor(baselong));
   longmin = abs(60 * (baselong - longdeg));
   time_t now = time(0);
   tm* utc = gmtime(&now);
   //$GPRMC,061748,A,5000.05,N,00815.75,E,0,0,050416,001.0,W*61
   snprintf(buff, 256, "$GPRMC,%02d%02d%02d,A,%02.0f%05.2f,%c,%03.0f%05.2f,%c,0,0,%02d%02d%02d,001.0,W*",
         utc->tm_hour, utc->tm_min, utc->tm_sec, latdeg, latmin, latstr, longdeg, longmin, longstr, utc->tm_mday, utc->tm_mon, utc->tm_year-2000);
      csum = checksum(buff);
      dststr.append(buff);
      snprintf(buff, 64, "%02x\r\n", csum);
      dststr.append(buff);
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

int ADSBparser::checksum(char* sentence)
{
   int csum = 0;
   int i=1;
   while (sentence[i] != '\0') {
      csum ^= (int) sentence[i++];
   }
   return csum;
}

int ADSBparser::dtoi(double d)
{
   return (int)(d + 0.5);
}
