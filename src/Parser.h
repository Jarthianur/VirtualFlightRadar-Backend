/*
 * Parser.h
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#ifndef PARSER_H_
#define PARSER_H_

#define BUFF_OUT_S 4096
#define LESS_BUFF_S 128

#include <string>
#include "Aircraft.h"
#include "AircraftContainer.h"

class Parser
{
public:
   Parser(long double, long double, int);
   virtual ~Parser();

   /**
    * unpack a msg into Aircraft
    */
   virtual int unpack(const std::string&, AircraftContainer&) = 0;

   /**
    * build nmea-msg from Aircraft to given string-reference
    */
   virtual void process(Aircraft*, std::string&) = 0;

   /**
    * parse GPRMC
    */
   void gprmc(std::string&);

protected:
   /**
    * format string buffer
    */
   char buffer[BUFF_OUT_S];

   /**
    * convert degree to radian
    */
   long double radian(long double) const;

   /**
    * convert radian to degree
    */
   long double degree(long double) const;

   /**
    * convert long double to int, round to nearest number
    */
   int ldToI(long double) const;

   /**
    * compute checksum of nmea string
    */
   int checksum(const char*) const;

   /**
    * calculate nmea-data
    */
   void calcPosInfo(Aircraft*);

   /**
    * Number PI as precise as CPU can do
    */
   const long double PI;

   /**
    * base position info
    */
   long double baselat,
   baselong,
   /**
    * relative North, East, Vertical
    */
   rel_N,
   rel_E,
   rel_V,
   /**
    * distance from base position to Aircraft
    */
   dist,
   /**
    * Latitude degree, minutes
    * Longitude degree, minutes
    */
   lat_deg,
   lat_min,
   long_deg,
   long_min,
   /**
    * Longitude base, Aircraft
    * Latitude base, Aircraft
    */
   long_b,
   long_ac,
   lat_b,
   lat_ac,
   /**
    * Longitude, Latitude distance
    */
   long_dist,
   lat_dist,
   /**
    * bearing, relative bearing, absolute bearing
    */
   bearing,
   bearing_rel,
   bearing_abs,
   /**
    * values to calculate distance
    */
   a, c;
   /**
    * (alt = height + antennaheight)
    */
   int basealt;
   /**
    * Latitude: S - N
    * Longitude: W - E
    */
   char latstr, longstr;

};

#endif /* PARSER_H_ */
