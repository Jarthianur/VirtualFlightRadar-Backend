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

#include "Configuration.h"

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

int Configuration::base_altitude = 0;
double Configuration::base_latitude = 0.0;
double Configuration::base_longitude = 0.0;
double Configuration::base_geoid = 0.0;
int Configuration::global_out_port = 0;
int Configuration::global_ogn_port = 0;
int Configuration::global_adsb_port = 0;
std::string Configuration::global_ogn_host = "nA";
std::string Configuration::global_adsb_host = "nA";
std::string Configuration::global_login_str = "";
std::string Configuration::global_wind_feed_host = "nA";
int Configuration::global_wind_feed_port = 0;
int Configuration::filter_maxHeight = 0;
int Configuration::filter_maxDist = 0;
