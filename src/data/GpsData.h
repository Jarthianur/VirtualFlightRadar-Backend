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
     * @param cr_pos The initial position
     */
    void init(const struct util::ExtGpsPosition& cr_pos) override;
    /**
     * @fn update
     * @brief Try to update the base position.
     * @param cr_pos The new position
     * @param prio   The attempts priority
     * @locks mBasePos.mutex
     */
    void update(const struct util::ExtGpsPosition& cr_pos, std::int32_t prio) override;
    /**
     * @fn getGpsStr
     * @brief Get a full NMEA GPS report.
     * @note A full report contains GPGGA and GPRMC and includes trailing <cr><lf>.
     * @return the NMEA string
     * @locks mBasePos.mutex
     */
    std::string getGpsStr();
    /**
     * @fn getBasePos
     * @brief Get the base GPS position.
     * @return the position
     * @locks mBasePos.mutex
     */
    struct util::ExtGpsPosition getBasePos();
    /**
     * @fn getBaseLat
     * @brief Get the base latitude.
     * @return the latitude
     * @locks mBasePos.mutex
     */
    double getBaseLat();
    /**
     * @fn getBaseLong
     * @brief Get the base longitude.
     * @return the longitude
     * @locks mBasePos.mutex
     */
    double getBaseLong();
    /**
     * @fn getBaseAlt
     * @brief Get the base altitude.
     * @return the altitude
     * @locks mBasePos.mutex
     */
    std::int32_t getBaseAlt();

private:
    /// Wrapper holding the base position.
    struct Wrapper<struct util::ExtGpsPosition> mBasePos;
    /// GpsModule providing functionality to build GPS sentences.
    util::GpsModule mGpsMod;
};

}  // namespace data

#endif /* SRC_DATA_GPSDATA_H_ */
