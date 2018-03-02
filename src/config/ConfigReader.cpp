/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
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

#include <stdexcept>

#include "../Logger.h"

namespace config
{
ConfigReader::ConfigReader()
    : mConfRe("^(\\S+?)\\s*?=\\s*?(\\S+?[^;]*?)\\s*?(?:;[\\S\\s]*?)?$",
              boost::regex_constants::optimize)
{}

ConfigReader::~ConfigReader() noexcept
{}

void ConfigReader::read(std::istream& rStream, PropertyMap& rMap)
{
    std::string currentLine;
    std::string currentSection;
    std::size_t lineNr = 0;

    while(std::getline(rStream, currentLine))
    {
        ++lineNr;
        if(currentLine.empty() || (currentLine.length() > 0 && currentLine[0] == ';'))
        {
            continue;
        }

        auto section = parseSection(currentLine);
        if(section.get<0>())
        {
            currentSection = section.get<1>();
            addSection(currentSection, lineNr, rMap);
            continue;
        }

        auto keyValue = parseKeyValue(currentLine);
        if(keyValue.get<0>())
        {
            addKeyValue(currentSection, keyValue.get<1>(), lineNr, rMap);
        }
        else
        {
            Logger::error({"(ConfigReader) malformed param [", std::to_string(lineNr),
                           "]: ", currentLine});
        }
    }
}

boost::tuple<bool, std::string> ConfigReader::parseSection(const std::string& crLine)
{
    std::string section;
    bool valid = false;
    if(crLine.at(0) == '[')
    {
        try
        {
            section = crLine.substr(1, crLine.rfind(']') - 1);
            valid   = true;
        }
        catch(const std::out_of_range&)
        {
        }
    }
    return boost::make_tuple(valid, section);
}

boost::tuple<bool, KeyValue> ConfigReader::parseKeyValue(const std::string& crLine)
{
    std::string key;
    std::string value;
    boost::smatch match;
    bool valid = boost::regex_match(crLine, match, mConfRe);

    if(valid)
    {
        key           = match.str(1);
        value         = match.str(2);
        std::size_t l = value.find_last_not_of(' ');

        if(l != std::string::npos)
        {
            value = value.substr(0, l + 1);
        }
    }
    return boost::make_tuple(valid, std::make_pair(key, value));
}

void ConfigReader::addSection(const std::string& crSection, std::size_t vLineNr,
                              PropertyMap& rMap)
{
    if(!rMap.addProperty(crSection))
    {
        Logger::warn({"(ConfigReader) could not add section [", std::to_string(vLineNr),
                      "]: ", crSection});
    }
}

void ConfigReader::addKeyValue(const std::string& crSection, const KeyValue& crKeyValue,
                               std::size_t vLineNr, PropertyMap& rMap)
{
    if(!rMap.addProperty(crSection, crKeyValue))
    {
        Logger::warn({"(ConfigReader) could not add property [", std::to_string(vLineNr),
                      "]: ", crKeyValue.first});
    }
}

}  // namespace config
