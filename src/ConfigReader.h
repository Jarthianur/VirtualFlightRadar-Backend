/*
 * FileReader.h
 *
 *  Created on: 24.05.2016
 *      Author: lula
 */

#ifndef CONFIGREADER_H_
#define CONFIGREADER_H_

#include <fstream>
#include <unordered_map>
#include <string>

class ConfigReader
{
public:
   ConfigReader(const char*);
   inline virtual ~ConfigReader() throw() {};

   void read();
   const char* getProperty(const char*, const char*) const;

private:
   const char* file;
   std::unordered_map<std::string, std::string> config;
};

#endif /* CONFIGREADER_H_ */
