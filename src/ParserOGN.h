/*
 * ParserOGN.h
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#ifndef PARSEROGN_H_
#define PARSEROGN_H_

#include "Parser.h"
#include <vector>
#include <regex>

class ParserOGN: public Parser
{
public:
    ParserOGN(long double, long double, int);
    virtual ~ParserOGN();

    int unpack(const std::string&, AircraftContainer&);
    void process(Aircraft*, std::string&);

private:
    void splitToTokens(const std::string&);
    long double dmsToDeg(long double dms) const;
    //constants
    long double kts2kmh = 1.852L;
    long double kmh2kts = 0.539957L;
    long double feet2m = 0.3048L;
    long double ms2fpm = 196.85L;
    long double fpm2ms = 0.00508L;
    //regex
    const std::regex aprs_re;
    const std::regex addr_re;
    const std::regex climb_re;
    //comment tokens
    std::vector<std::string> tokens;
    std::string comment;
    std::string id;
    int addr_t, ac_t, climb_r;
};

#endif /* PARSEROGN_H_ */
