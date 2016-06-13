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
#include <iostream>

using namespace std;

#define VERSION "1.2.2"

int main(int argc, char* argv[]) {

    long double latitude;
    long double longitude;
    int altitude;
    float geoid;
    int out_port;
    int ogn_port;
    int adsb_port;
    std::string ogn_host;
    std::string adsb_host;
    std::string login;
    std::string nmea_feed_host;
    int nmea_feed_port;
    int maxHeight;
    int maxDist;

    if (argc == 3) {
        cout << VERSION << endl;
        ConfigReader cr(argv[2]);
        cr.read();
        latitude = stold(cr.getProperty("latitude", "0.0L"));
        longitude = stold(cr.getProperty("longitude","0.0L"));
        altitude = stoi(cr.getProperty("altitude","0"));
        geoid = stof(cr.getProperty("geoid","0.0"));
        out_port = stoi(cr.getProperty("outport","0"));
        ogn_port = stoi(cr.getProperty("ognport","0"));
        adsb_port = stoi(cr.getProperty("adsbport","0"));
        ogn_host = cr.getProperty("ognhost", "nA");
        adsb_host = cr.getProperty("adsbhost", "nA");
        login = cr.getProperty("login", "xxxxx");
        nmea_feed_host = cr.getProperty("nmeaFeedHost", "nA");
        nmea_feed_port = stoi(cr.getProperty("nmeaFeedPort","0"));
        maxHeight = stoi(cr.getProperty("maxHeight", "9999999"));
        maxDist = stoi(cr.getProperty("maxDist", "40000"));
    } else {
        cout << "usage: ./VirtualFlightRadar-Backend -c pathToConfigFile"<< endl;
        return 1;
    }

    VFRB::base_altitude = altitude;
    VFRB::base_latitude = latitude;
    VFRB::base_longitude = longitude;
    VFRB::base_geoid = geoid;
    VFRB::global_out_port = out_port;
    VFRB::global_ogn_port = ogn_port;
    VFRB::global_adsb_port = adsb_port;
    VFRB::global_ogn_host = ogn_host;
    VFRB::global_adsb_host = adsb_host;
    VFRB::global_login_str = login;
    VFRB::global_nmea_feed_host = nmea_feed_host;
    VFRB::global_nmea_feed_port = nmea_feed_port;
    VFRB::filter_maxHeight = maxHeight;
    VFRB::filter_maxDist = maxDist;
    VFRB::run();

    return 0;
}
