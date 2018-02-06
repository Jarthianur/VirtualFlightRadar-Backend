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

#ifndef SRC_AIRCRAFT_AIRCRAFT_HPP_
#define SRC_AIRCRAFT_AIRCRAFT_HPP_

#include <cstdint>
#include <string>

#include "../util/Position.h"

/// Indicate a double value is not available.
#define A_VALUE_NA -1024.0

namespace aircraft
{

/**
 * @class Aircraft
 * @brief Store all information about an aircraft, like position, id etc.
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
     * @param r_id  The id
     * @param r_pos The position
     */
    Aircraft(std::string& r_id, struct util::GpsPosition& r_pos);
    /**
     * @fn Aircraft
     * @brief Constructor
     * @param r_id    The Id
     * @param r_pos   The prosition
     * @param gnd_spd The ground speed
     * @param id_t    The Id type
     * @param ac_t    The aircraft type
     * @param climb_r The climb rate
     * @param turn_r  The turn rate
     * @param head    The heading
     */
    Aircraft(std::string& r_id, struct util::GpsPosition& r_pos, double gnd_spd,
             std::uint32_t id_t, std::int32_t ac_t, double climb_r, /*double turn_r,*/
             double head);
    /**
     * @fn ~Aircraft
     * @brief Destructor
     */
    virtual ~Aircraft() noexcept;
    /**
     * @fn operator==
     * @brief Compare aircrafts by their Id.
     * @param cr_other The Aircraft to compare
     * @return true, if Id's equal, else false
     */
    bool operator==(const Aircraft& cr_other) const;
    /**
     * TargetType enumeration
     * @brief Device type from which the information is received.
     * @note FLARM is preferred over TRANSPONDER,
     *       in case an aircraft has both available.
     */
    enum class TargetType
        : std::uint32_t
        {
            FLARM, TRANSPONDER
    };
    /**
     * @fn update
     * @brief Update this Aircraft, reset update age.
     * @param cr_ac Other Aircraft with new data
     * @param prio  Priority of the update
     */
    void update(const Aircraft& cr_ac, std::uint32_t prio);
    /**
     * @fn getId
     * @brief Get the Id.
     * @return the Id string
     */
    inline const std::string& getId() const
    {
        return mId;
    }
    /**
     * @fn getIdType
     * @brief Get the Id type.
     * @return the Id type
     */
    inline const std::uint32_t getIdType() const
    {
        return mIdType;
    }
    /**
     * @fn getTargetT
     * @brief Get the target type.
     * @see TargetType
     * @return the target type
     */
    inline const TargetType getTargetT() const
    {
        return mTargetT;
    }
    /**
     * @fn getAircraftT
     * @brief Get the aircraft type.
     * @return the aircraft type
     */
    inline const std::int32_t getAircraftT() const
    {
        return mAircraftT;
    }
    /**
     * @fn isFullInfo
     * @brief Is full information available?
     * @note Full information means, all about position, movement is given.
     * @return true, if full info is available, else false
     */
    inline const bool isFullInfo() const
    {
        return mFullInfo;
    }
    /**
     * @fn getUpdateAge
     * @brief Get the current update age.
     * @return the update age
     */
    inline std::uint64_t& getUpdateAge()
    {
        return mUpdateAge;
    }
    /**
     * @fn getLastPriority
     * @brief Get last written priority.
     * @return the last priority
     */
    inline const std::uint32_t getLastPriority() const
    {
        return mLastPriority;
    }

    inline void setLastPriority(std::uint32_t prio) {
        mLastPriority = prio;
    }

    /**
     * @fn getLatitude
     * @brief Get the last known latitude.
     * @return the latitude
     */
    inline const double getLatitude() const
    {
        return mPosition.latitude;
    }
    /**
     * @fn getLongitude
     * @brief Get the last known longitude.
     * @return the longitude
     */
    inline const double getLongitude() const
    {
        return mPosition.longitude;
    }
    /**
     * @fn getAltitude
     * @brief Get the last known altitude.
     * @return the altitude.
     */
    inline const std::int32_t getAltitude() const
    {
        return mPosition.altitude;
    }
    /**
     * @fn getGndSpeed
     * @brief Get the last known speed over ground.
     * @return the ground speed
     */
    inline const double getGndSpeed() const
    {
        return mGndSpeed;
    }
    /**
     * @fn getHeading
     * @brief Get the last known heading.
     * @return the heading
     */
    inline const double getHeading() const
    {
        return mHeading;
    }
    /**
     * @fn getClimbRate
     * @brief Get the last known climb rate.
     * @return the climb rate
     */
    inline const double getClimbRate() const
    {
        return mClimbRate;
    }
    /*inline const double getTurnR() const
     {
     return mTurnRate;
     }*/
    /**
     * @fn setId
     * @brief Set the Id.
     * @param cr_id The new Id
     */
    inline void setId(const std::string& cr_id)
    {
        mId = cr_id;
    }

    /**
     * @fn setPosition
     * @brief Set the GPS position.
     * @param cr_pos The new position
     */
    inline void setPosition(const struct util::GpsPosition& cr_pos)
    {
        mPosition = cr_pos;
    }
    /**
     * @fn setAircraftT
     * @brief Set the Aircraft type.
     * @param c_act The new Aircraft type
     */
    inline void setAircraftT(const std::int32_t c_act)
    {
        mAircraftT = c_act;
    }
    /**
     * @fn setIdType
     * @brief Set the Id type.
     * @param c_idt The new Id type
     */
    inline void setIdType(const std::uint32_t c_idt)
    {
        mIdType = c_idt;
    }
    /**
     * @fn setClimbRate
     * @brief Set the climb rate.
     * @param c_climb The new climb rate; default A_VALUE_NA
     */
    inline void setClimbRate(const double c_climb = A_VALUE_NA)
    {
        mClimbRate = c_climb;
    }
    /**
     * @fn setHeading
     * @brief Set the heading.
     * @param c_head The new heading; default A_VALUE_NA
     */
    inline void setHeading(const double c_head = A_VALUE_NA)
    {
        mHeading = c_head;
    }
    /**
     * @fn setGndSpeed
     * @brief Set the ground speed.
     * @param c_gndspd The new ground speed; default  A_VALUE_NA
     */
    inline void setGndSpeed(const double c_gndspd = A_VALUE_NA)
    {
        mGndSpeed = c_gndspd;
    }
    /**
     * @fn setTargetT
     * @brief Set the target type.
     * @see TargetType
     * @param tt The new target type
     */
    inline void setTargetT(TargetType tt)
    {
        mTargetT = tt;
    }
    /**
     * @fn setFullInfo
     * @brief Set full information to be available.
     * @param info Is available, or not (default: true)
     */
    inline void setFullInfo(bool info = true)
    {
        mFullInfo = info;
    }

    inline std::uint64_t& getUpdateAttempts()
    {
        return mUpdateAttempts;
    }

private:
    /// Aircraft ID (address), identifier; Uniqueness is assumed and must be guaranteed by input feed.
    std::string mId;
    /// ID (address) type.
    std::uint32_t mIdType = 1;
    /// Aircraft type encoded as number (glider, powered airplane ...)
    std::int32_t mAircraftT = 8;
    /// Target type; Got the last update from which device.
    TargetType mTargetT = TargetType::FLARM;
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

} // namespace aircraft

#endif /* SRC_AIRCRAFT_AIRCRAFT_HPP_ */
