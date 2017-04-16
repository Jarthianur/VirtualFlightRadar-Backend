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

#include "Configuration.h"

#include <stdexcept>

#include "../util/Logger.h"
#include "ConfigReader.h"

Configuration::Configuration(const char* file)
{
    if (!readConfig(file))
    {
        throw std::logic_error("failed to read configuration file");
    }
}

Configuration::~Configuration() noexcept
{
}

std::int32_t Configuration::base_altitude = 0;
double Configuration::base_latitude = 0.0;
double Configuration::base_longitude = 0.0;
double Configuration::base_geoid = 0.0;
double Configuration::base_pressure = 0.0;
double Configuration::base_temp = 0.0;
std::int32_t Configuration::filter_maxHeight = 0;
std::int32_t Configuration::filter_maxDist = 0;
std::uint16_t Configuration::global_server_port = 1;

std::string Configuration::global_feeds;


bool Configuration::readConfig(const char* file)
{
    ConfigReader cr(file);
    try
    {
        cr.read();
    }
    catch (const std::exception& e)
    {
        Logger::error("(Config) read file: ", e.what());
        return false;
    }
    catch (...)
    {
        Logger::error("(Config) read file");
        return false;
    }

    Configuration::base_latitude = strToDouble(cr.getProperty("latitude", "0.0"));
    Logger::info("(Config) latitude: ", std::to_string(Configuration::base_latitude));

    Configuration::base_longitude = strToDouble(cr.getProperty("longitude", "0.0"));
    Logger::info("(Config) longitude: ", std::to_string(Configuration::base_longitude));

    Configuration::base_altitude = strToInt(cr.getProperty("altitude", "0"));
    Logger::info("(Config) altitude: ", std::to_string(Configuration::base_altitude));

    Configuration::base_geoid = strToDouble(cr.getProperty("geoid", "0.0"));
    Logger::info("(Config) geoid: ", std::to_string(Configuration::base_geoid));

    Configuration::base_pressure = strToDouble(cr.getProperty("pressure", "1013.25"));
    Logger::info("(Config) pressure: ", std::to_string(Configuration::base_pressure));

    Configuration::base_temp = strToDouble(cr.getProperty("temp", "15.0"));
    Logger::info("(Config) temp: ", std::to_string(Configuration::base_temp));

    Configuration::global_server_port = (uint16_t) strToInt(
            cr.getProperty("serverPort", "9999"));
    Logger::info("(Config) serverPort: ",
                 std::to_string(Configuration::global_server_port));
    Logger::debug(cr.getProperty("serverPort", "9999"));

    Configuration::global_aprsc_port = cr.getProperty("aprscPort", "9998");
    Logger::info("(Config) aprscPort: ", Configuration::global_aprsc_port);

    Configuration::global_sbs_port = cr.getProperty("sbsPort", "9997");
    Logger::info("(Config) sbsPort: ", Configuration::global_sbs_port);

    Configuration::global_aprsc_host = cr.getProperty("aprscHost", "nA");
    Logger::info("(Config) aprscHost: ", Configuration::global_aprsc_host);

    Configuration::global_sbs_host = cr.getProperty("sbsHost", "nA");
    Logger::info("(Config) sbsHost: ", Configuration::global_sbs_host);

    Configuration::global_aprsc_login = cr.getProperty("aprscLogin", "");
    Logger::info("(Config) aprscLogin: ", Configuration::global_aprsc_login);

    Configuration::global_climate_host = cr.getProperty("climateSensorHost", "nA");
    Logger::info("(Config) climateSensorHost: ", Configuration::global_climate_host);

    Configuration::global_climate_port = cr.getProperty("climateSensorPort", "0");
    Logger::info("(Config) climateSensorPort: ", Configuration::global_climate_port);

    std::string tmp = cr.getProperty("maxHeight", "-1");
    if (tmp == "-1")
    {
        Configuration::filter_maxHeight = INT32_MAX;
    }
    else
    {
        Configuration::filter_maxHeight = strToInt(tmp);
    }
    Logger::info("(Config) maxHeight: ", std::to_string(Configuration::filter_maxHeight));

    tmp = cr.getProperty("maxDist", "-1");
    if (tmp == "-1")
    {
        Configuration::filter_maxDist = INT32_MAX;
    }
    else
    {
        Configuration::filter_maxDist = strToInt(tmp);
    }
    Logger::info("(Config) maxDist: ", std::to_string(Configuration::filter_maxDist));

    return false;
}

std::int32_t Configuration::strToInt(const std::string& str) noexcept
{
    try
    {
        return stoi(str);
    }
    catch (const std::logic_error& iae)
    {
        Logger::error("(VFRB) invalid configuration: ",
                      str.length() == 0 ? "empty" : str);
    }
    return 0;
}

double Configuration::strToDouble(const std::string& str) noexcept
{
    try
    {
        return stod(str);
    }
    catch (const std::logic_error& iae)
    {
        Logger::error("(VFRB) invalid configuration: ",
                      str.length() == 0 ? "empty" : str);
    }
    return 0.0;
}
