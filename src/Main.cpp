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

#include "VFRB.h"
#include "ConfigReader.h"
#include "Configuration.h"
#include <iostream>

using namespace std;

#define VERSION "1.3.0-SNAPSHOT"

int main(int argc, char* argv[]) {

    double latitude = 0.0;
    double longitude = 0.0;
    int altitude = 0;
    float geoid = 0.0;
    int out_port = 0;
    int ogn_port = 0;
    int adsb_port = 0;
    std::string ogn_host("nA");
    std::string adsb_host("nA");
    std::string login;
    std::string wind_feed_host("nA");
    int wind_feed_port = 0;
    int maxHeight = 0;
    int maxDist = 0;

    if (argc == 3) {
        cout << VERSION << endl;
        ConfigReader cr(argv[2]);
        cr.read();
        latitude = stold(cr.getProperty("latitude", "0.0"));
        cout << "latitude: " << latitude << endl;
        longitude = stold(cr.getProperty("longitude","0.0"));
        cout << "longitude: " << longitude << endl;
        altitude = stoi(cr.getProperty("altitude","0"));
        cout << "altitude: " << altitude << endl;
        geoid = stof(cr.getProperty("geoid","0.0"));
        cout << "geoid: " << geoid << endl;
        out_port = stoi(cr.getProperty("outport","0"));
        cout << "outport: " << out_port << endl;
        ogn_port = stoi(cr.getProperty("ognport","0"));
        cout << "ognport: " << ogn_port << endl;
        adsb_port = stoi(cr.getProperty("adsbport","0"));
        cout << "adsbport: " << adsb_port << endl;
        ogn_host = cr.getProperty("ognhost", "nA");
        cout << "ognhost: " << ogn_host << endl;
        adsb_host = cr.getProperty("adsbhost", "nA");
        cout << "adsbhost: " << adsb_host << endl;
        login = cr.getProperty("login", "");
        cout << "login: " << login << endl;
        wind_feed_host = cr.getProperty("windFeedHost", "nA");
        cout << "windFeedHost: " << wind_feed_host << endl;
        wind_feed_port = stoi(cr.getProperty("windFeedPort","0"));
        cout << "windFeedPort: " << wind_feed_port << endl;
        maxHeight = stoi(cr.getProperty("maxHeight", "0"));
        cout << "maxHeight: " << maxHeight << endl;
        maxDist = stoi(cr.getProperty("maxDist", "0"));
        cout << "maxDist: " << maxDist << endl;
    } else {
        cout << "usage: ./VirtualFlightRadar-Backend -c pathToConfigFile"<< endl;
        return 1;
    }

    Configuration::base_altitude = altitude;
    Configuration::base_latitude = latitude;
    Configuration::base_longitude = longitude;
    Configuration::base_geoid = geoid;
    Configuration::global_out_port = out_port;
    Configuration::global_ogn_port = ogn_port;
    Configuration::global_adsb_port = adsb_port;
    Configuration::global_ogn_host = ogn_host;
    Configuration::global_adsb_host = adsb_host;
    Configuration::global_login_str = login;
    Configuration::global_wind_feed_host = wind_feed_host;
    Configuration::global_wind_feed_port = wind_feed_port;
    Configuration::filter_maxHeight = maxHeight;
    Configuration::filter_maxDist = maxDist;

    VFRB::run();

    return 0;
}
