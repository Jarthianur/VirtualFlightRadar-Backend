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

#ifndef UTIL_SENSOR_H_
#define UTIL_SENSOR_H_

#include <string>

/// ICAO standard atmospheric pressure at MSL
#define ICAO_STD_A 1013.25

namespace util
{

struct Atmosphere
{
    std::string mdaStr;
    double pressure;
    /* Humidity and temperature are not important for us.*/
};

struct Wind
{
    std::string mwvStr;
};

struct Climate
{
	struct Wind mWind;
	struct Atmosphere mAtmosphere;

	inline bool hasAtmosphere()
	{
        return !mAtmosphere.mdaStr.empty();
	}

	inline bool hasWind()
	{
        return !mWind.mwvStr.empty();
	}
};

}  // namespace util

#endif /* UTIL_SENSOR_H_ */
