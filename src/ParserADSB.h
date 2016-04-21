#ifndef PARSERADSB_H_
#define PARSERADSB_H_

#include "Parser.h"

class ParserADSB : public Parser
{
public:
	ParserADSB(long double, long double, int);
	virtual ~ParserADSB();

	int unpack(Aircraft&, const std::string&) const;
	void process(Aircraft&, std::string&);
};

#endif /* PARSERADSB_H_ */
