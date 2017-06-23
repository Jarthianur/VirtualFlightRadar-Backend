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

#ifndef SRC_DATA_GPSDATA_H_
#define SRC_DATA_GPSDATA_H_

#include <cstdint>
#include <string>

#include "../util/GpsModule.h"
#include "../util/Position.hpp"
#include "Data.hpp"
#include "Wrapper.hpp"

namespace data
{

/**
 * The GpsData class.
 *
 * This class holds GPS information.
 */
class GpsData: public Data<struct util::ExtGPSPosition>
{
public:
    /**
     * Constructor
     */
    GpsData();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~GpsData() noexcept;
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
    void setDefaults(double b_lat, double b_lon, std::int32_t b_alt, double geoid);
    /**
     * Get the NMEA sentences reporting a GPS position.
     * As they are GPGGA and GPRMC.
     * The returned sentences include trailing <cr><lf>.
     *
     * @return the NMEA sentences in string
     */
    std::string getGpsStr();
    /**
     * Set the new base position.
     * May fail due to priority.
     *
     * @param prio   the priority attempting to write
     * @param cr_pos the new position
     */
    void update(const struct util::ExtGPSPosition& cr_pos, std::int32_t prio) override;
    /**
     * Get the last registered GPS position.
     *
     * @return the position
     */
    struct util::ExtGPSPosition getBasePos();
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
    /// Data holding the base position.
    struct Wrapper<struct util::ExtGPSPosition> mBasePos;
    /// GpsModule providing functionality to build GPS sentences.
    util::GpsModule mGpsMod;
};

}  // namespace data

#endif /* SRC_DATA_GPSDATA_H_ */
