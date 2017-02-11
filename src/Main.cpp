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

#include <netinet/in.h>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "io/logger/Logger.h"
#include "io/reader/ConfigReader.h"
#include "util/Configuration.h"
#include "vfrb/VFRB.h"

#ifndef VERSION
#define VERSION "DEMO"
#endif

using namespace std;

int32_t strToInt(const string& str)
{
    try
    {
        return stoi(str);
    }
    catch (const logic_error& iae)
    {
        Logger::error("(VFRB) invalid configuration: ", str);
    }
    return 0;
}

double strToDouble(const string& str)
{
    try
    {
        return stod(str);
    }
    catch (const logic_error& iae)
    {
        Logger::error("(VFRB) invalid configuration: ", str);
    }
    return 0.0;
}

int main(int argc, char* argv[])
{

    double latitude = 0.0;
    double longitude = 0.0;
    int32_t altitude = 0;
    double geoid = 0.0;
    double pressure = 0.0;
    double temp = 0.0;

    in_port_t server_port = 0;

    std::string aprsc_host;
    std::string aprsc_port;
    std::string aprsc_login;

    std::string sbs_host;
    std::string sbs_port;

    std::string climate_host;
    std::string climate_port;

    int32_t maxHeight = 0;
    int32_t maxDist = 0;

    if (argc == 3)
    {
        Logger::info("VirtualFlightRadar-Backend -- ", VERSION);
        ConfigReader cr(argv[2]);
        cr.read();

        latitude = strToDouble(cr.getProperty("latitude", "0.0"));
        Logger::info("(Config) latitude: ", to_string(latitude));

        longitude = strToDouble(cr.getProperty("longitude", "0.0"));
        Logger::info("(Config) longitude: ", to_string(longitude));

        altitude = strToInt(cr.getProperty("altitude", "0"));
        Logger::info("(Config) altitude: ", to_string(altitude));

        geoid = strToDouble(cr.getProperty("geoid", "0.0"));
        Logger::info("(Config) geoid: ", to_string(geoid));

        pressure = strToDouble(cr.getProperty("pressure", "1013.25"));
        Logger::info("(Config) pressure: ", to_string(pressure));

        temp = strToDouble(cr.getProperty("temp", "15.0"));
        Logger::info("(Config) temp: ", to_string(temp));

        server_port = (in_port_t) strToInt(cr.getProperty("serverPort", "0"));
        Logger::info("(Config) serverPort: ", to_string(server_port));

        aprsc_port = cr.getProperty("aprscPort", "0");
        Logger::info("(Config) aprscPort: ", aprsc_port);

        sbs_port = cr.getProperty("sbsPort", "0");
        Logger::info("(Config) sbsPort: ", sbs_port);

        aprsc_host = cr.getProperty("aprscHost", "nA");
        Logger::info("(Config) aprscHost: ", aprsc_host);

        sbs_host = cr.getProperty("sbsHost", "nA");
        Logger::info("(Config) sbsHost: ", sbs_host);

        aprsc_login = cr.getProperty("aprscLogin", "");
        Logger::info("(Config) aprscLogin: ", aprsc_login);

        climate_host = cr.getProperty("climateSensorHost", "nA");
        Logger::info("(Config) climateSensorHost: ", climate_host);

        climate_port = cr.getProperty("climateSensorPort", "0");
        Logger::info("(Config) climateSensorPort: ", climate_port);

        std::string tmp = cr.getProperty("maxHeight", "-1");
        if (tmp == "-1")
        {
            maxHeight = INT32_MAX;
        }
        else
        {
            maxHeight = strToInt(tmp);
        }
        Logger::info("(Config) maxHeight: ", to_string(maxHeight));

        tmp = cr.getProperty("maxDist", "-1");
        if (tmp == "-1")
        {
            maxDist = INT32_MAX;
        }
        else
        {
            maxDist = strToInt(tmp);
        }
        Logger::info("(Config) maxDist: ", to_string(maxDist));

    }
    else
    {
        Logger::info("usage: ./VirtualFlightRadar-Backend -c pathToConfigFile");
        return -1;
    }

    Configuration::base_altitude = altitude;
    Configuration::base_latitude = latitude;
    Configuration::base_longitude = longitude;
    Configuration::base_geoid = geoid;
    Configuration::base_pressure = pressure;
    Configuration::base_temp = temp;
    Configuration::global_server_port = server_port;
    Configuration::global_aprsc_port = aprsc_port;
    Configuration::global_sbs_port = sbs_port;
    Configuration::global_aprsc_host = aprsc_host;
    Configuration::global_sbs_host = sbs_host;
    Configuration::global_aprsc_login = aprsc_login;
    Configuration::global_climate_host = climate_host;
    Configuration::global_climate_port = climate_port;
    Configuration::filter_maxHeight = maxHeight;
    Configuration::filter_maxDist = maxDist;

    VFRB::run();

    return 0;
}
