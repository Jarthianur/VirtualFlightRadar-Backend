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
#include "../data/Position.hpp"

#define A_VALUE_NA -1024.0

class Aircraft
{
public:
    /**
     * Construct an aircraft with a minimum
     * set of information, as are the ID
     * and the current position.
     *
     * @param r_id ID
     * @param lat  latitude
     * @param lon  longitude
     * @param alt  altitude
     */
    Aircraft(std::string& /*r_id*/, struct GPSPosition& /*r_pos*/);
    /**
     * Construct an aircraft with a full
     * set of information, as are the ID,
     * the current position and movement.
     *
     * @param r_id    ID
     * @param lat     latitude
     * @param lon     longitude
     * @param alt     altitude
     * @param gnd_spd ground speed
     * @param id_t    ID type
     * @param ac_t    aircraft type
     * @param climb_r climb rate
     * @param turn_r  turn rate
     * @param head    heading
     */
    Aircraft(std::string& /*r_id*/, struct GPSPosition& /*r_pos*/,
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
     * @param r_other aircraft to compare
     * @return IDs equal?
     */
    bool operator==(const Aircraft& /*r_other*/) const;
    /**
     * Aircraft information received from
     * device type.
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
     * @param r_ac aircraft reference holding new information
     * @param prio update from feed with priority
     */
    void update(const Aircraft& /*r_ac*/, Priority /*prio*/);
    /**
     * Get ID.
     *
     * @return const reference to ID
     */
    inline const std::string& getID() const
    {
        return mID;
    }
    /**
     * Get ID type.
     *
     * @return ID type
     */
    inline const std::uint32_t getIDtype() const
    {
        return mIDtype;
    }
    /**
     * Get target type.
     *
     * @return target type
     */
    inline const TargetType getTargetT() const
    {
        return mTargetType;
    }
    /**
     * Get aircraft type.
     *
     * @return aircraft type
     */
    inline const std::int32_t getAircraftT() const
    {
        return mAircraftType;
    }
    /**
     * Full information available?
     *
     * @return full info avail
     */
    inline const bool isFullInfo() const
    {
        return mFullInfo;
    }
    /**
     * Get update age.
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
    inline const Priority getLastPriority() const
    {
        return mLastPriority;
    }
    /**
     * Is update attempt valid?
     *
     * @return attempt valid
     */
    inline const bool isAttemptValid() const
    {
        return mAttemptValid;
    }
    /**
     * Get latitude
     *
     * @return
     */
    inline const double getLatitude() const
    {
        return mPosition.latitude;
    }
    inline const double getLongitude() const
    {
        return mPosition.longitude;
    }
    inline const std::int32_t getAltitude() const
    {
        return mPosition.altitude;
    }
    inline const double getGndSpeed() const
    {
        return mGndSpeed;
    }
    inline const double getHeading() const
    {
        return mHeading;
    }
    inline const double getClimbR() const
    {
        return mClimbRate;
    }
    //const double getTurnR() const;

    /**
     * Setter
     */
    inline void incUpdateAge()
    {
        ++mUpdateAge;
    }
    inline void setTargetT(TargetType tt)
    {
        mTargetType = tt;
    }
    inline void setFullInfo(bool info = true)
    {
        mFullInfo = info;
    }
    inline void setAttemptValid()
    {
        mAttemptValid = true;
    }

private:
    /**
     * information
     */
    // id
    std::string mID;
    std::uint32_t mIDtype = 1;

    std::int32_t mAircraftType = 8;
    TargetType mTargetType = TargetType::FLARM;

    // full info available
    bool mFullInfo = false;

    //cycles without update
    std::uint32_t mUpdateAge = 0;

    //last update by priority
    Priority mLastPriority = Priority::DONTCARE;
    bool mAttemptValid = true;

    /**
     * position
     */
    struct GPSPosition mPosition;

    /**
     * movement
     */
    // m/s
    double mGndSpeed = A_VALUE_NA;

    // deg [0-359]
    double mHeading = A_VALUE_NA;

    // m/s
    double mClimbRate = A_VALUE_NA;

    // currently unused
    // deg/s
    double mTurnRate = A_VALUE_NA;
};

#endif /* SRC_AIRCRAFT_AIRCRAFT_H_ */
