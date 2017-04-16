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

#ifndef SRC_CONFIG_CONFIGURATION_H_
#define SRC_CONFIG_CONFIGURATION_H_

#include <cstdint>
#include <string>

class Configuration
{
public:

    Configuration();
    virtual ~Configuration() noexcept;

    static std::int32_t base_altitude;
    static double base_latitude;
    static double base_longitude;
    static double base_geoid;
    static double base_pressure;
    static double base_temp;

    static std::uint16_t global_server_port;

    static std::string global_aprsc_host;
    static std::string global_aprsc_port;
    static std::string global_aprsc_login;

    static std::string global_sbs_host;
    static std::string global_sbs_port;

    static std::string global_climate_host;
    static std::string global_climate_port;

    static std::int32_t filter_maxHeight;
    static std::int32_t filter_maxDist;
};

#endif /* SRC_CONFIG_CONFIGURATION_H_ */
