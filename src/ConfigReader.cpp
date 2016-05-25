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
      std::cout << key << '=' << value << std::endl;
      config.insert({key, value});
   }
   src.close();
}

const char* ConfigReader::getProperty(const char* key, const char* defaultValue) const
{
    auto it = config.find(key);
    if (it == config.end()) {
        return defaultValue;
    } else {
        return it->second.c_str();
    }
}
