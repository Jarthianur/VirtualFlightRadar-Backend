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
#include <typeindex>
#include <utility>

#include "../util/Logger.h"

using namespace util;

namespace config
{

ConfigReader::ConfigReader()
        : mConfRe("^(\\S+?)\\s*?=\\s*?(\\S+?[^;]*?)\\s*?(?:;[\\S\\s]*?)?$",
                boost::regex_constants::optimize)
{
}

ConfigReader::~ConfigReader() noexcept
{
}

void ConfigReader::read(std::istream& r_file, PropertyMap& r_map)
{
    std::string key;
    std::string value;
    std::string line;
    std::string section;
    std::size_t line_nr = 0;
    while (std::getline(r_file, line))
    {
        line_nr++;
        try
        {
            if (line.length() == 0 || line.at(0) == ';')
            {
                continue;
            }
            if (line.at(0) == '[')
            {
                section = line.substr(1, line.rfind(']') - 1);
                if (!r_map.addProperty(section))
                {
                    Logger::warn(
                            "(ConfigReader) could not add section ["
                                    + std::to_string(line_nr) + "]: ", section);
                }
                continue;
            }
            boost::smatch match;
            if (boost::regex_match(line, match, mConfRe))
            {
                key = match.str(1);
                value = match.str(2);
                std::size_t l = value.find_last_not_of(' ');
                if (l != std::string::npos)
                {
                    value = value.substr(0, l + 1);
                }
                keyValue kv_pair = std::make_pair(key, value);
                if (!r_map.addProperty(section, kv_pair))
                {
                    Logger::warn(
                            "(ConfigReader) could not add property ["
                                    + std::to_string(line_nr) + "]: ", key);
                }
            } else
            {
                Logger::error(
                        "(ConfigReader) malformed param [" + std::to_string(line_nr)
                                + "]: ", line);
            }
        } catch (const std::out_of_range& e)
        {
            continue;
        }
    }
}

}  // namespace config
