/*
 * FileReader.cpp
 *
 *  Created on: 24.05.2016
 *      Author: lula
 */

#include "ConfigReader.h"

#include <iostream>

ConfigReader::ConfigReader(const char* filename)
: file(filename)
{
}

void ConfigReader::read()
{
   std::ifstream src;
   src.open(file);
   std::string key;
   std::string value;
   while (!src.eof()) {
      src >> key;
      src >> value;
      config.insert({key, value});
   }
   src.close();
}

std::string ConfigReader::getProperty(const char* key, const char* defaultValue) const
{
    auto it = config.find(key);
    if (it == config.end()) {
        return std::string(defaultValue);
    } else {
        return it->second;
    }
}
