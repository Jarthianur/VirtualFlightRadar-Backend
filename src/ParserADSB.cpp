#include <cmath>
#include <cstdio>
#include <ctime>
#include "ParserADSB.h"

ParserADSB::ParserADSB()
{
}

ParserADSB::~ParserADSB()
{
}

int ParserADSB::unpack(Aircraft& ac, const std::string& sentence) const
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
               ac.latitude = std::stold(msg.substr(0,delim), nullptr);
            else return -1;
            break;
         case 15:
            if (msg.substr(0,delim).length() > 0)
               ac.longitude = std::stold(msg.substr(0,delim), nullptr);
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

void ParserADSB::process(Aircraft& ac, long double baselat,
      long double baselong, int basealt)
{
   long_b = radian(baselong);
   long_ac = radian(ac.longitude);
   lat_b = radian(baselat);
   lat_ac = radian(ac.latitude);
   //printf("\nlon1= %.17Lf, lat1= %.17Lf, lon2= %.17Lf, lat2= %.17Lf\n", lon1,lat1,lon2,lat2);
   long_dist = long_ac - long_b;
   lat_dist = lat_ac -lat_b;
   a = std::pow(std::sin(lat_dist/2.0L),2.0L) + std::cos(lat_b) * std::cos(lat_ac) * std::pow(std::sin(long_dist/2.0L),2.0L);
   //printf("a= %.17Lf\n", a);
   c = 2.0L * std::atan2(std::sqrt(a), std::sqrt(1.0L - a));
   //printf("c= %.17Lf\n",c);
   dist = 6371000.0L * c;
   //printf("distance= %.17Lf\n", distance);

   bearing = std::atan2(std::sin(long_ac-long_b)*std::cos(lat_ac), std::cos(lat_b)*std::sin(lat_ac)-std::sin(lat_b)*std::cos(lat_ac)*std::cos(long_ac-long_b));
   //printf("bearing= %.17Lf\n",bearing);
   bearing_rel = degree(bearing);
   //printf("relBearing= %.17Lf\n",relBearing);
   bearing_abs = std::fmod((bearing_rel + 360.0L), 360.0L);
   //printf("absBearing= %.17Lf\n",absBearing);

   rel_N = std::cos(radian(bearing_abs)) * dist;
   //printf("relNorth= %.17Lf, rad(absBearing)= %.17Lf\n",relNorth, radian(absBearing));
   rel_E = std::sin(radian(bearing_abs)) * dist;
   //printf("relEast= %.17Lf\n",relEast);
   rel_V = ac.altitude / 3.2808L - basealt;

   ac.nmea_str.clear();
   //PFLAU
   snprintf(buffer, BUFF_OUT_S, "$PFLAU,,,,1,0,%d,0,%d,%u,%s*", ldToI(bearing_rel),
         ldToI(rel_V), ldToI(dist), ac.id.c_str());
   int csum = checksum(buffer);
   ac.nmea_str.append(buffer);
   snprintf(buffer, 64, "%02x\r\n", csum);
   ac.nmea_str.append(buffer);
   //PFLAA
   snprintf(buffer, BUFF_OUT_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,8*", ldToI(rel_N),
         ldToI(rel_E), ldToI(rel_V), ac.id.c_str());
   csum = checksum(buffer);
   ac.nmea_str.append(buffer);
   snprintf(buffer, 64, "%02x\r\n", csum);
   ac.nmea_str.append(buffer);
   //GPRMC
   latstr = (baselat < 0)? 'S' : 'N';
   longstr = (baselong < 0)? 'W' : 'E';
   lat_deg = std::abs(std::floor(baselat));
   lat_min = std::abs(60.0L * (baselat - lat_deg));
   long_deg = std::abs(std::floor(baselong));
   long_min = std::abs(60.0L * (baselong - long_deg));
   time_t now = time(0);
   tm* utc = gmtime(&now);
   //$GPRMC,061748,A,5000.05,N,00815.75,E,0,0,050416,001.0,W*61
   snprintf(buffer, BUFF_OUT_S, "$GPRMC,%02d%02d%02d,A,%02.0Lf%05.2Lf,%c,%03.0Lf%05.2Lf,%c,0,0,%02d%02d%02d,001.0,W*",
         utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg, long_min, longstr, utc->tm_mday, utc->tm_mon+1, utc->tm_year-100);
   csum = checksum(buffer);
   ac.nmea_str.append(buffer);
   snprintf(buffer, 64, "%02x\r\n", csum);
   ac.nmea_str.append(buffer);
   return;
}
