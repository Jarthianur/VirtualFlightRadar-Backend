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
#include <string>

#include "../util/Position.h"

/// @def A_VALUE_NA
/// Indicate a double value is not available.
#define A_VALUE_NA -1024.0

/// @namespace aircraft
namespace aircraft
{
/**
 * @class Aircraft
 * @brief Respresents an aircraft.
 */
class Aircraft
{
public:
    /**
     * @fn Aircraft
     * @brief Constructor
     */
    Aircraft();

    /**
     * @fn Aircraft
     * @brief Constructor
     * @param rId  The id
     * @param rPos The position
     */
    Aircraft(std::string& rId, struct util::GpsPosition& rPos);

    /**
     * @fn Aircraft
     * @brief Constructor
     * @param rId           The Id
     * @param rPos          The prosition
     * @param vGndSpd       The ground speed
     * @param vIdType       The Id type
     * @param vAircraftType The aircraft type
     * @param vClimbRate    The climb rate
     * @param vHeading      The heading
     */
    Aircraft(std::string& rId, struct util::GpsPosition& rPos, double vGndSpd,
             std::uint32_t vIdType, std::int32_t vAircraftType, double vClimbRate,
             double vHeading);

    /**
     * @fn ~Aircraft
     * @brief Destructor
     */
    virtual ~Aircraft() noexcept;

    /**
     * @fn operator==
     * @brief Compare two aircrafts by their Id.
     * @param crOther The Aircraft to compare
     * @return true, if Id's are equal, else false
     */
    bool operator==(const Aircraft& crOther) const;

    /**
     * @enum TargetType
     * @brief Device type from which the information is received.
     * @note FLARM is preferred over TRANSPONDER,
     *       in case an aircraft has both available.
     */
    enum class TargetType : std::uint32_t
    {
        FLARM,
        TRANSPONDER
    };

    /**
     * @fn update
     * @brief Update this Aircraft.
     * @param crUpdate  Other Aircraft with new data
     * @param vPriority Priority of the update
     */
    void update(const Aircraft& crUpdate, std::uint32_t vPriority);

    /**
     * @fn getId
     * @brief Get the Id.
     * @return the Id
     */
    const std::string& getId() const;

    /**
     * @fn getIdType
     * @brief Get the Id type.
     * @return the Id type
     */
    const std::uint32_t getIdType() const;

    /**
     * @fn getTargetT
     * @brief Get the target type.
     * @see TargetType
     * @return the target type
     */
    const TargetType getTargetType() const;

    /**
     * @fn getAircraftT
     * @brief Get the aircraft type.
     * @return the aircraft type
     */
    const std::int32_t getAircraftType() const;

    /**
     * @fn hasFullInfo
     * @brief Are position and movement informations given?
     * @return true, if full info is available, else false
     */
    const bool hasFullInfo() const;

    /**
     * @fn getUpdateAge
     * @brief Get the current update age.
     * @return the update age by reference
     */
    std::uint64_t& getUpdateAge();

    /**
     * @fn getLastPriority
     * @brief Get last written priority.
     * @return the last priority
     */
    const std::uint32_t getLastPriority() const;

    /**
     * @fn getLatitude
     * @brief Get the last known latitude.
     * @return the latitude
     */
    const double getLatitude() const;

    /**
     * @fn getLongitude
     * @brief Get the last known longitude.
     * @return the longitude
     */
    const double getLongitude() const;

    /**
     * @fn getAltitude
     * @brief Get the last known altitude.
     * @return the altitude.
     */
    const std::int32_t getAltitude() const;

    /**
     * @fn getGndSpeed
     * @brief Get the last known speed over ground.
     * @return the ground speed
     */
    const double getGndSpeed() const;

    /**
     * @fn getHeading
     * @brief Get the last known heading.
     * @return the heading
     */
    const double getHeading() const;

    /**
     * @fn getClimbRate
     * @brief Get the last known climb rate.
     * @return the climb rate
     */
    const double getClimbRate() const;

    /**
     * @fn getUpdateAttempts
     * @brief Get the update attempt count.
     * @return the update attempt count by reference
     */
    std::uint64_t& getUpdateAttempts();

    /**
     * @fn setId
     * @brief Set the Id.
     * @param cr_id The new Id
     */
    void setId(const std::string& cr_id);

    /**
     * @fn setPosition
     * @brief Set the GPS position.
     * @param cr_pos The new position
     */
    void setPosition(const struct util::GpsPosition& cr_pos);
    /**
     * @fn setAircraftT
     * @brief Set the Aircraft type.
     * @param c_act The new Aircraft type
     */
    void setAircraftT(const std::int32_t c_act);
    /**
     * @fn setIdType
     * @brief Set the Id type.
     * @param c_idt The new Id type
     */
    void setIdType(const std::uint32_t c_idt);
    /**
     * @fn setClimbRate
     * @brief Set the climb rate.
     * @param c_climb The new climb rate; default A_VALUE_NA
     */
    void setClimbRate(const double c_climb = A_VALUE_NA);
    /**
     * @fn setHeading
     * @brief Set the heading.
     * @param c_head The new heading; default A_VALUE_NA
     */
    void setHeading(const double c_head = A_VALUE_NA);
    /**
     * @fn setGndSpeed
     * @brief Set the ground speed.
     * @param c_gndspd The new ground speed; default  A_VALUE_NA
     */
    void setGndSpeed(const double c_gndspd = A_VALUE_NA);
    /**
     * @fn setTargetT
     * @brief Set the target type.
     * @see TargetType
     * @param tt The new target type
     */
    void setTargetT(TargetType tt);
    /**
     * @fn setFullInfo
     * @brief Set full information to be available.
     * @param info Is available, or not (default: true)
     */
    void setFullInfo(bool info = true);

    /**
     * @fn setLastPriority
     * @brief
     * @param prio
     */
    void setLastPriority(std::uint32_t prio);

private:
    /// Aircraft ID (address), identifier; Uniqueness is assumed and must be guaranteed by
    /// input feed.
    std::string mId;
    /// ID (address) type.
    std::uint32_t mIdType = 1;
    /// Aircraft type encoded as number (glider, powered airplane ...)
    std::int32_t mAircraftType = 8;
    /// Target type; Got the last update from which device.
    TargetType mTargetType = TargetType::FLARM;
    /// Is full set of information available?
    bool mFullInfo = false;
    /// Times processed without update.
    std::uint64_t mUpdateAge = 0;
    /// Got last update with which priority.
    std::uint32_t mLastPriority = 0;
    ///
    std::uint64_t mUpdateAttempts = 0;
    /// Last registered position.
    struct util::GpsPosition mPosition;
    /// Speed over ground; m/s
    double mGndSpeed = A_VALUE_NA;
    /// Heading; deg [0-359]
    double mHeading = A_VALUE_NA;
    /// Climb rate; m/s
    double mClimbRate = A_VALUE_NA;
    /* deg/s
     double mTurnRate = A_VALUE_NA;*/
};

}  // namespace aircraft
