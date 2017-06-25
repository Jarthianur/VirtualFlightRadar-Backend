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
 * @class GpsData implements Data
 * @brief Manage GPS information.
 * @see Data.hpp
 */
class GpsData: public Data<struct util::ExtGpsPosition>
{
public:
    /**
     * @fn GpsData
     * @brief Constructor
     */
    GpsData();
    /**
     * @fn ~GpsData
     * @brief Destructor
     */
    virtual ~GpsData() noexcept;
    /**
     * @fn init
     * @brief Initialize the GPS information.
     * @param r_pos The initial position
     */
    void init(const struct util::ExtGpsPosition& r_pos);
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
    void update(const struct util::ExtGpsPosition& cr_pos, std::int32_t prio) override;
    /**
     * Get the last registered GPS position.
     *
     * @return the position
     */
    struct util::ExtGpsPosition getBasePos();
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
    struct Wrapper<struct util::ExtGpsPosition> mBasePos;
    /// GpsModule providing functionality to build GPS sentences.
    util::GpsModule mGpsMod;
};

}  // namespace data

#endif /* SRC_DATA_GPSDATA_H_ */
