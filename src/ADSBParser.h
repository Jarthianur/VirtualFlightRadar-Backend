#ifndef ADSBPARSER_H_
#define ADSBPARSER_H_

#define BUFF_IN_S 1024

#include <string>
#include <stdexcept>
#include "Aircraft.h"
#include <cmath>
#include <cstdio>
#include <ctime>
#include <cstdlib>

class ParserADSB
{
public:
	ParserADSB();
	virtual ~ParserADSB();

	int unpack(Aircraft&, const std::string&) const;
	void process(Aircraft&, std::string&, long double, long double, int);
	int checksum(const char*) const;

private:
	long double radian(long double) const;
	long double degree(long double) const;
	int dtoi(long double) const;

	/**
	 * relative North, East, Vertical
	 */
	long double rel_N,
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
    * Latitude: S - N
    * Longitude: W - E
    */
	char latstr, longstr;
	/**
	 * Number PI as precise as CPU can do
	 */
	long double PI = std::acos(-1.0L);
};

#endif /* ADSBPARSER_H_ */
