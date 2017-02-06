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

#include "Configuration.h"

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

int32_t Configuration::base_altitude = 0;
double Configuration::base_latitude = 0.0;
double Configuration::base_longitude = 0.0;
double Configuration::base_geoid = 0.0;
double Configuration::base_pressure = 0.0;
double Configuration::base_temp = 0.0;

in_port_t Configuration::global_server_port = 0;

std::string Configuration::global_aprsc_host = "nA";
std::string Configuration::global_aprsc_port = "0";
std::string Configuration::global_aprsc_login = "";

std::string Configuration::global_sbs_host = "nA";
std::string Configuration::global_sbs_port = "0";

std::string Configuration::global_climate_host = "nA";
std::string Configuration::global_climate_port = "0";

int32_t Configuration::filter_maxHeight = 0;
int32_t Configuration::filter_maxDist = 0;
