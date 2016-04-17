#ifndef PARSERADSB_H_
#define PARSERADSB_H_

#include "Parser.h"

class ParserADSB : public Parser
{
public:
	ParserADSB();
	virtual ~ParserADSB();

	int unpack(Aircraft&, const std::string&) const;
	void process(Aircraft&, std::string&, long double, long double, int);

private:
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
};

#endif /* PARSERADSB_H_ */
