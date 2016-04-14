/*
 * ADSBparser.h
 *
 *  Created on: 13.04.2016
 *      Author: lula
 */

#ifndef ADSBPARSER_H_
#define ADSBPARSER_H_

#define PI 3.14159265359

#include <string>
#include <stdexcept>
#include "Aircraft.h"
#include <math.h>
#include <sstream>
#include <iomanip>

class ADSBparser
{
public:
	ADSBparser();
	virtual ~ADSBparser();

	int unpack(Aircraft&, std::string&);
	void process(Aircraft&, std::string&, double, double, int);
	int checksum(std::string&);

private:
	double radian(double);
	int degree(double);

	double relNorth,
	relEast,
	relVert,
	distance,
	latdeg,
	latmin,
	longdeg,
	longmin;
	int relBearing;
	//time utc
	char latstr, longstr;
};

#endif /* ADSBPARSER_H_ */
