/*
Copyright_License {

  Copyright (C) 2016 Virtual Flight Radar - Backend
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License version 3
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "ConfigReader.h"
#include <iostream>

ConfigReader::ConfigReader(const char* filename)
: file(filename),
  conf_re("(\\S+)\\s*?=\\s*?([\\+|-]?[^]+)")
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
                value = match.str(2);
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
