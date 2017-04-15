/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include <cstdint>
#include <stdexcept>
#include <string>

#include "config/ConfigReader.h"
#include "config/Configuration.h"
#include "util/Logger.h"
#include "vfrb/VFRB.h"
#include "data/ClimateData.h"
#include "data/AircraftContainer.h"

#ifndef VERSION
#define VERSION "DEMO"
#endif

std::int32_t strToInt(const std::string& str) noexcept;
double strToDouble(const std::string& str) noexcept;
void readConfig(const char* file);

int main(int argc, char* argv[])
{
    if (argc != 3 || argv[1] != "-c")
    {
        Logger::info("usage: ./VirtualFlightRadar-Backend -c pathToConfigFile");
        return -1;
    }
    else
    {
        Logger::info("VirtualFlightRadar-Backend -- ", VERSION);
        readConfig(argv[2]);
    }

    // set climate fallbacks
    VFRB::msClimateData.setPress();
    VFRB::msClimateData.setTemp();

    // init containers processor
    VFRB::msAcCont.initProcessor(Configuration::base_latitude,
                                 Configuration::base_longitude,
                                 Configuration::base_altitude);
    VFRB::run();

    return 0;
}

std::int32_t strToInt(const std::string& str) noexcept
{
    try
    {
        return stoi(str);
    }
    catch (const std::logic_error& iae)
    {
        Logger::error("(VFRB) invalid configuration: ", str);
    }
    return 0;
}

double strToDouble(const std::string& str) noexcept
{
    try
    {
        return stod(str);
    }
    catch (const std::logic_error& iae)
    {
        Logger::error("(VFRB) invalid configuration: ", str);
    }
    return 0.0;
}

void readConfig(const char* file)
{
    ConfigReader cr(file);
    cr.read();

    //TODO rm
    return;

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
}
