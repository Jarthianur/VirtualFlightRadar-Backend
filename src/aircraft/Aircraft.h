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

#ifndef SRC_AIRCRAFT_AIRCRAFT_H_
#define SRC_AIRCRAFT_AIRCRAFT_H_

#include <cstdint>
#include <string>

#include "../util/Priority.h"
#include "../util/Position.hpp"

namespace aircraft
{

#define A_VALUE_NA -1024.0

/**
 * The Aircraft class.
 *
 * Represents an aircraft, holding all necessary information
 * about its position, movement and some meta-data.
 */
class Aircraft
{
public:
    /**
     * Construct an aircraft with a minimum
     * set of information, as are the ID
     * and the current position.
     *
     * @param r_id  the ID
     * @param r_pos the position
     */
    Aircraft(std::string& /*r_id*/, struct util::GPSPosition& /*r_pos*/);
    /**
     * Construct an aircraft with a full
     * set of information, as are the ID,
     * the current position and movement.
     *
     * @param r_id    the ID
     * @param r_pos   the prosition
     * @param gnd_spd the ground speed
     * @param id_t    the ID type
     * @param ac_t    the aircraft type
     * @param climb_r the climb rate
     * @param turn_r  the turn rate
     * @param head    the heading
     */
    Aircraft(std::string& /*r_id*/, struct util::GPSPosition& /*r_pos*/,
             double /*gnd_spd*/, std::uint32_t /*id_t*/, std::int32_t /*ac_t*/,
             double /*climb_r*/, double /*turn_r*/, double /*head*/);
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~Aircraft() noexcept;
    /**
     * Compare aircrafts by ID.
     *
     * @param cr_other the aircraft to compare
     *
     * @return are IDs equal?
     */
    bool operator==(const Aircraft& /*cr_other*/) const;
    /**
     * Aircraft information received from
     * device type.
     * FLARM is preferred over TRANSPONDER,
     * in case an aircraft has both available.
     */
    enum class TargetType
        : std::uint32_t
        {
            FLARM,
        TRANSPONDER
    };
    /**
     * Update aircraft information.
     * Reset update age.
     *
     * @param cr_ac an aircraft reference holding new information
     * @param prio update from feed with priority
     */
    void update(const Aircraft& /*cr_ac*/, util::Priority /*prio*/);
    /**
     * Get the ID.
     *
     * @return const reference to ID
     */
    inline const std::string& getID() const
    {
        return mID;
    }
    /**
     * Get the ID type.
     *
     * @return ID type
     */
    inline const std::uint32_t getIDtype() const
    {
        return mIDtype;
    }
    /**
     * Get the last registered target type.
     *
     * @return target type
     */
    inline const TargetType getTargetT() const
    {
        return mTargetType;
    }
    /**
     * Get the aircraft type.
     *
     * @return aircraft type
     */
    inline const std::int32_t getAircraftT() const
    {
        return mAircraftType;
    }
    /**
     * Is full information available?
     *
     * @return full info available, or not
     */
    inline const bool isFullInfo() const
    {
        return mFullInfo;
    }
    /**
     * Get the current update age.
     *
     * @return update age
     */
    inline const std::uint32_t getUpdateAge() const
    {
        return mUpdateAge;
    }
    /**
     * Get last updated priority.
     *
     * @return last priority
     */
    inline const util::Priority getLastPriority() const
    {
        return mLastPriority;
    }
    /**
     * Is update attempt valid?
     *
     * @return is attempt valid
     */
    inline const bool isAttemptValid() const
    {
        return mAttemptValid;
    }
    /**
     * Get the last registered latitude.
     *
     * @return the latitude
     */
    inline const double getLatitude() const
    {
        return mPosition.latitude;
    }
    /**
     * Get the last registered longitude.
     *
     * @return the longitude
     */
    inline const double getLongitude() const
    {
        return mPosition.longitude;
    }
    /**
     * Get the last registered altitude.
     *
     * @return the altitude.
     */
    inline const std::int32_t getAltitude() const
    {
        return mPosition.altitude;
    }
    /**
     * Get the last registered speed over ground.
     *
     * @return the ground speed
     */
    inline const double getGndSpeed() const
    {
        return mGndSpeed;
    }
    /**
     * Get the last registered heading.
     *
     * @return the heading
     */
    inline const double getHeading() const
    {
        return mHeading;
    }
    /**
     * Get the last registered climb rate.
     *
     * @return the climb rate
     */
    inline const double getClimbR() const
    {
        return mClimbRate;
    }
    /*inline const double getTurnR() const
     {
     return mTurnRate;
     }*/
    /**
     * Increment the update age by one.
     */
    inline void incUpdateAge()
    {
        ++mUpdateAge;
    }
    /**
     * Set the new target type.
     *
     * @param tt the new target type
     */
    inline void setTargetT(TargetType tt)
    {
        mTargetType = tt;
    }
    /**
     * Set full information available.
     *
     * @param info is available, or not, defaults to true
     */
    inline void setFullInfo(bool info = true)
    {
        mFullInfo = info;
    }
    /**
     * Allow update attempts.
     */
    inline void setAttemptValid()
    {
        mAttemptValid = true;
    }

private:
    /// Aircraft ID (address), identifier; Uniqueness is assumed and must be guaranteed by input feed.
    std::string mID;
    /// ID (address) type.
    std::uint32_t mIDtype = 1;
/// Aircraft type encoded as number (glider, powered airplane ...)
    std::int32_t mAircraftType = 8;
    /// Target type; Got the last update from which device.
    TargetType mTargetType = TargetType::FLARM;
    /// Is full set of information available?
    bool mFullInfo = false;
    /// Times processed without update.
    std::uint32_t mUpdateAge = 0;
    /// Got last update with which priority.
    util::Priority mLastPriority = util::Priority::DONTCARE;
    /// Is an update attempt valid? If false, updates are only allowed with at least last priority.
    bool mAttemptValid = true;
/// Last registered position.
    struct util::GPSPosition mPosition;
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

#endif /* SRC_AIRCRAFT_AIRCRAFT_H_ */
