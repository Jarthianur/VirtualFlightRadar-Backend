#ifndef ADSBPARSER_H_
#define ADSBPARSER_H_

#define PI 3.14159265359

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

	int unpack(Aircraft&, std::string&);
	void process(Aircraft&, std::string&, double, double, int);
	int checksum(char*);

private:
	double radian(double);
	int degree(double);
	int dtoi(double);

	double relNorth,
	relEast,
	relVert,
	distance,
	latdeg,
	latmin,
	longdeg,
	longmin;
	int relBearing;
	char latstr, longstr;
};

#endif /* ADSBPARSER_H_ */
