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
#include <utility>

#include "../Logger.hpp"

namespace config
{
const boost::regex ConfigReader::s_keyValueRE("^(\\S+?)\\s*?=\\s*?(\\S+?[^;]*?)\\s*?(?:;[\\S\\s]*?)?$",
              boost::regex_constants::optimize);

ConfigReader::ConfigReader()
{}

ConfigReader::~ConfigReader() noexcept
{}

void ConfigReader::read(std::istream& stream, Properties& properties)
{
    std::string currentLine;
    std::string currentSection;
    std::size_t lineNr = 0;

    while(std::getline(stream, currentLine))
    {
        ++lineNr;
        if(currentLine.empty() || (currentLine.length() > 0 && currentLine[0] == ';'))
        {
            continue;
        }

        boost::optional<std::string> section = parseSection(currentLine);
        if(section)
        {
            currentSection = *section;
            if(!properties.addProperty(currentSection))
                {
                    logger.warn("(ConfigReader) could not add section [", std::to_string(lineNr),
                                "]: ", section);
                }
            continue;
        }

        boost::optional<KeyValue> keyValue = parseProperty(currentLine);
        if(keyValue)
        {
        	if(!properties.addProperty(currentSection, *keyValue))
        	    {
        	        logger.warn("(ConfigReader) could not add property [", std::to_string(lineNr),
        	                    "]: ", (*keyValue).first);
        	    }
        }
        else
        {
            logger.error("(ConfigReader) malformed param [", std::to_string(lineNr),
                         "]: ", currentLine);
        }
    }
}

boost::optional<std::string> ConfigReader::parseSection(const std::string& line)
{
    try
    {
        if(line.at(0) == '[')
        {
            return line.substr(1, line.rfind(']') - 1);
        }
    }
    catch(const std::out_of_range&)
    {
    }
    return boost::none;
}

boost::optional<KeyValue> ConfigReader::parseProperty(const std::string& line)
{
    boost::smatch match;

    if(boost::regex_match(line, match, s_keyValueRE))
    {
        std::string value = match.str(2);
        std::size_t l     = value.find_last_not_of(' ');

        if(l != std::string::npos)
        {
            value = value.substr(0, l + 1);
        }
        return std::make_pair(match.str(1), value);
    }
    return boost::none;
}

}  // namespace config
