#ifndef PARSERADSB_H_
#define PARSERADSB_H_

#include "Parser.h"

class ParserADSB : public Parser
{
public:
    ParserADSB(long double, long double, int);
    virtual ~ParserADSB() throw();

    int unpack(const std::string&, AircraftContainer&);
    void process(Aircraft&, std::string&);

private:
    std::string id;
    int alt = 0;
    long double lat = 0.0L, lon = 0.0L;
};

#endif /* PARSERADSB_H_ */
