#ifndef PARSERADSB_H_
#define PARSERADSB_H_

#include "Parser.h"

class ParserADSB : public Parser
{
public:
	ParserADSB(long double, long double, int);
	virtual ~ParserADSB();

	Aircraft* unpack(const std::string&);
	void process(Aircraft*, std::string&);
};

#endif /* PARSERADSB_H_ */
