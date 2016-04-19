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
};

#endif /* PARSERADSB_H_ */
