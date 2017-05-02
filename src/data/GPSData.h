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

#ifndef SRC_DATA_GPSDATA_H_
#define SRC_DATA_GPSDATA_H_

#include <cstdint>
#include <string>

#include "../util/GPSmodule.h"
#include "../util/Priority.h"
#include "Data.hpp"
#include "Position.hpp"

/**
 * The GPSData class.
 *
 * This class holds GPS information.
 */
class GPSData {
public:
	/**
	 * Constructor
	 */
	GPSData();
	/**
	 * Destructor
	 *
	 * @exceptsafe no-throw
	 */
	virtual ~GPSData() noexcept;
	/**
	 * Set the default GPS position and meta-data.
	 * After this operation, an instance of this class
	 * returns always a valid, usable GPS position.
	 *
	 * @param b_lat the latitude
	 * @param b_lon the longitude
	 * @param b_alt the altitude
	 * @param geoid the geoid separation
	 */
	void setDefaults(double /*b_lat*/, double /*b_lon*/, std::int32_t /*b_alt*/,
			double /*geoid*/);
	/**
	 * Get the NMEA sentences reporting a GPS position.
	 * As they are GPGGA and GPRMC.
	 * The returned sentences include trailing <cr><lf>.
	 *
	 * @return the NMEA sentences in string
	 */
	std::string getGPSstr();
	/*void setGGAstr(Priority /*prio* /, const std::string& /*gga* /);
	 void setRMCstr(Priority /*prio* /, const std::string& /*rmc* /);*/
	/**
	 * Set the new base position.
	 * May fail due to Priority.
	 *
	 * @param prio  the Priority attempting to write
	 * @param r_pos the new position
	 */
	void setBasePos(Priority /*prio*/, const struct ExtGPSPosition& /*r_pos*/);
	/**
	 * Get the last registered GPS position.
	 *
	 * @return the position
	 */
	struct ExtGPSPosition getBasePos();
	/**
	 * Get the base positions latitude.
	 *
	 * @return the latitude
	 */
	double getBaseLat();
	/**
	 * Get the base positions longitude.
	 *
	 * @return the longitude
	 */
	double getBaseLong();
	/**
	 * Get the base positions altitude.
	 *
	 * @return the altitude
	 */
	std::int32_t getBaseAlt();

private:
	/*std::string getGGAstr();
	 std::string getRMCstr();*/
	/// Data holding the base position.
	struct Data<struct ExtGPSPosition> mBasePos;
	/*struct TmpData<std::string> mGGAstr;
	 struct TmpData<std::string> mRMCstr;*/
	/// GPSmodule providing functionality to build GPS sentences.
	GPSmodule mGPSfix;
};

#endif /* SRC_DATA_GPSDATA_H_ */
