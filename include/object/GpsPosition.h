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

#pragma once

#include <cstdint>

#include "impl/DateTimeImplBoost.h"
#include "util/utility.hpp"

#include "Object.h"
#include "Timestamp.hpp"

namespace object
{
/**
 * @brief A position on earth.
 */
struct Location
{
    static constexpr auto MAX_LATITUDE  = 90.0;
    static constexpr auto MIN_LATITUDE  = -90.0;
    static constexpr auto MAX_LONGITUDE = 180.0;
    static constexpr auto MIN_LONGITUDE = 0.0;
    static constexpr auto MAX_ALTITUDE  = 100000;
    static constexpr auto MIN_ALTITUDE  = -11000;

    double       latitude;   ///< Latitude; deg
    double       longitude;  ///< Longitude; deg
    std::int32_t altitude;   ///< Altitude; m
};

/**
 * @brief Extend Object to a GPS position.
 */
class GpsPosition : public Object
{
public:
    static constexpr auto NMEA_SIZE = 192;
    static constexpr auto MAX_GEOID = 86.0;
    static constexpr auto MIN_GEOID = -108.0;

private:
    Location                           m_location;        ///< The location
    double                             m_geoid;           ///< The geoid separation
    double                             m_dilution;        ///< The position dilution
    std::uint8_t                       m_nrOfSatellites;  ///< The number of satellites
    std::int8_t                        m_fixQuality;      ///< The GPS fix quality
    Timestamp<time::DateTimeImplBoost> m_timestamp;       ///< The timestamp of this position
    util::CString<NMEA_SIZE>           m_nmea;

    /**
     * @brief Override Object::assign.
     */
    void assign(Object&& other) override;

    /**
     * @brief Override Object::canUpdate.
     */
    bool canUpdate(const Object& other) const override;

public:
    GpsPosition(std::uint32_t priority, const Location& location, double geoid);

    /**
     * @param position The position
     * @param geoid    The geoid
     */
    GpsPosition(std::uint32_t priority, const Location& location, double geoid, double dilution,
                std::uint8_t satellites, std::int8_t quality,
                const Timestamp<time::DateTimeImplBoost>& timestamp);
    ~GpsPosition() noexcept override = default;

    util::CString<NMEA_SIZE>& operator*();

    util::CStringPack nmea() const override;
    auto              location() const -> const decltype(m_location)&;
    auto              geoid() const -> decltype(m_geoid);
    auto              timestamp() const -> const decltype(m_timestamp)&;
    auto              dilution() const -> decltype(m_dilution);
    auto              nrOfSatellites() const -> decltype(m_nrOfSatellites);
    auto              fixQuality() const -> decltype(m_fixQuality);
};
}  // namespace object
