/*
 * FileReader.cpp
 *
 *  Created on: 24.05.2016
 *      Author: lula
 */

#include "ConfigReader.h"
#include <iostream>

ConfigReader::ConfigReader(const char* filename)
: file(filename),
  conf_re("(\\S+)(\\s+)?=(\\s+)?([\\+|-]?.+)")
{
}

void ConfigReader::read()
{
    std::ifstream src(file);
    std::string key;
    std::string value;
    while (std::getline(src, key)) {
        try {
            if (key.at(0) == '#') continue;
            std::smatch match;
            if (std::regex_match(key, match, conf_re)) {
                key = match.str(1);
                value = match.str(4);
                key.shrink_to_fit();
                value.shrink_to_fit();
                std::cout << key.c_str() << "__"<< value << "__" << std::endl;
                config.insert({key, value});
            } else {
                std::cout << "malformed parameter! " << key << std::endl;
            }
        } catch (std::regex_error& e) {
            std::cout << e.what() << std::endl;
            break;
        } catch (std::out_of_range& e) {
            continue;
        }
    }
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
