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

using namespace std;

#define VERSION "1.3.1-SNAPSHOT"

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
        cout << VERSION << endl;
        cout << "== Configuration ==" << endl;
        ConfigReader cr(argv[2]);
        cr.read();
        try
        {
            latitude = stod(cr.getProperty("latitude", "0.0"));
            cout << "latitude: " << latitude << endl;

            longitude = stod(cr.getProperty("longitude", "0.0"));
            cout << "longitude: " << longitude << endl;

            altitude = stoi(cr.getProperty("altitude", "0"));
            cout << "altitude: " << altitude << endl;

            geoid = stod(cr.getProperty("geoid", "0.0"));
            cout << "geoid: " << geoid << endl;

            pressure = stod(cr.getProperty("pressure", "1013.25"));
            cout << "Pressure: " << pressure << endl;

            temp = stod(cr.getProperty("temp", "15.0"));
            cout << "Temp: " << temp << endl;

            out_port = stoi(cr.getProperty("outPort", "0"));
            cout << "outPort: " << out_port << endl;

            aprsc_port = stoi(cr.getProperty("aprscPort", "0"));
            cout << "aprscPort: " << aprsc_port << endl;

            sbs_port = stoi(cr.getProperty("sbsPort", "0"));
            cout << "sbsPort: " << sbs_port << endl;

            aprsc_host = cr.getProperty("aprscHost", "nA");
            cout << "aprscHost: " << aprsc_host << endl;

            sbs_host = cr.getProperty("sbsHost", "nA");
            cout << "sbsHost: " << sbs_host << endl;

            login = cr.getProperty("aprscLogin", "");
            cout << "aprscLogin: " << login << endl;

            weather_feed_host = cr.getProperty("weatherFeedHost", "nA");
            cout << "weatherFeedHost: " << weather_feed_host << endl;

            weather_feed_port = stoi(cr.getProperty("weatherFeedPort", "0"));
            cout << "weatherFeedPort: " << weather_feed_port << endl;

            maxHeight = stoi(cr.getProperty("maxHeight", "0"));
            cout << "maxHeight: " << maxHeight << endl;

            maxDist = stoi(cr.getProperty("maxDist", "0"));
            cout << "maxDist: " << maxDist << endl;
        }
        catch (const std::invalid_argument& e)
        {
            cout << "malformed configuration\t->\texiting!" << endl;
            return 1;
        }
    }
    else
    {
        cout << "usage: ./VirtualFlightRadar-Backend -c pathToConfigFile" << endl;
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

    cout << "Running program ..." << endl;
    VFRB::run();

    return 0;
}
