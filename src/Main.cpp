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
#include <iostream>
#include <stdexcept>
#include <string>

#include "base/Configuration.h"
#include "base/VFRB.h"
#include "util/ConfigReader.h"
#include "util/Logger.h"

using namespace std;

#define VERSION "1.3.1-SNAPSHOT"

int32_t strToInt(const string& str)
{
    try
    {
        return stoi(str);
    }
    catch (const invalid_argument& iae)
    {
        Logger::error("Invalid configuration: ", str);
    }
    return 0;
}

double strToDouble(const string& str)
{
    try
    {
        return stod(str);
    }
    catch (const invalid_argument& iae)
    {
        Logger::error("Invalid configuration: ", str);
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
    in_port_t out_port = 0;
    in_port_t aprsc_port = 0;
    in_port_t sbs_port = 0;
    std::string aprsc_host("nA");
    std::string sbs_host("nA");
    std::string login;
    std::string weather_feed_host("nA");
    in_port_t weather_feed_port = 0;
    int32_t maxHeight = 0;
    int32_t maxDist = 0;

    if (argc == 3)
    {
        Logger::info(VERSION);
        Logger::info("== Configuration ==");
        ConfigReader cr(argv[2]);
        cr.read();

        latitude = strToDouble(cr.getProperty("latitude", "0.0"));
        Logger::info("latitude: ", to_string(latitude));

        longitude = strToDouble(cr.getProperty("longitude", "0.0"));
        Logger::info("longitude: ", to_string(longitude));

        altitude = strToInt(cr.getProperty("altitude", "0"));
        Logger::info("altitude: ", to_string(altitude));

        geoid = strToDouble(cr.getProperty("geoid", "0.0"));
        Logger::info("geoid: ", to_string(geoid));

        pressure = strToDouble(cr.getProperty("pressure", "1013.25"));
        Logger::info("Pressure: ", to_string(pressure));

        temp = strToDouble(cr.getProperty("temp", "15.0"));
        Logger::info("Temp: ", to_string(temp));

        out_port = (in_port_t) strToInt(cr.getProperty("outPort", "0"));
        Logger::info("outPort: ", to_string(out_port));

        aprsc_port = (in_port_t) strToInt(cr.getProperty("aprscPort", "0"));
        Logger::info("aprscPort: ", to_string(aprsc_port));

        sbs_port = (in_port_t) strToInt(cr.getProperty("sbsPort", "0"));
        Logger::info("sbsPort: ", to_string(sbs_port));

        aprsc_host = cr.getProperty("aprscHost", "nA");
        Logger::info("aprscHost: ", aprsc_host);

        sbs_host = cr.getProperty("sbsHost", "nA");
        Logger::info("sbsHost: ", sbs_host);

        login = cr.getProperty("aprscLogin", "");
        Logger::info("aprscLogin: ", login);

        weather_feed_host = cr.getProperty("weatherFeedHost", "nA");
        Logger::info("weatherFeedHost: ", weather_feed_host);

        weather_feed_port = (in_port_t) strToInt(cr.getProperty("weatherFeedPort", "0"));
        Logger::info("weatherFeedPort: ", to_string(weather_feed_port));

        maxHeight = strToInt(cr.getProperty("maxHeight", "0"));
        Logger::info("maxHeight: ", to_string(maxHeight));

        maxDist = strToInt(cr.getProperty("maxDist", "0"));
        Logger::info("maxDist: ", to_string(maxDist));

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
    Configuration::global_out_port = out_port;
    Configuration::global_aprsc_port = aprsc_port;
    Configuration::global_sbs_port = sbs_port;
    Configuration::global_aprsc_host = aprsc_host;
    Configuration::global_sbs_host = sbs_host;
    Configuration::global_aprsc_login = login;
    Configuration::global_weather_feed_host = weather_feed_host;
    Configuration::global_weather_feed_port = weather_feed_port;
    Configuration::filter_maxHeight = maxHeight;
    Configuration::filter_maxDist = maxDist;

    Logger::info("Running program ...");
    VFRB::run();

    return 0;
}
