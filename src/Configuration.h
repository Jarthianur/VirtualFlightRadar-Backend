/*
Copyright_License {

  Copyright (C) 2017 Virtual Flight Radar - Backend
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

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>

class Configuration
{
public:

    Configuration();
    virtual ~Configuration() throw();

    static int base_altitude;
    static double base_latitude;
    static double base_longitude;
    static double base_geoid;
    static int global_out_port;
    static int global_ogn_port;
    static int global_adsb_port;
    static std::string global_ogn_host;
    static std::string global_adsb_host;
    static std::string global_login_str;
    static std::string global_wind_feed_host;
    static int global_wind_feed_port;
    static int filter_maxHeight;
    static int filter_maxDist;

};

#endif /* CONFIGURATION_H_ */
