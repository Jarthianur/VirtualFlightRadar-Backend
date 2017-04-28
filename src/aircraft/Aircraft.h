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

#define A_ADSB_T 8
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
    Aircraft(std::string& /*r_id*/, double /*lat*/, double /*lon*/, std::int32_t /*alt*/);
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
    Aircraft(std::string& /*r_id*/, double /*lat*/, double /*lon*/, std::int32_t /*alt*/,
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
     *
     */
    inline const TargetType getTargetT() const
    {
        return mTargetType;
    }
    inline const std::int32_t getAircraftT() const
    {
        return mAircraftType;
    }
    inline const bool isFullInfo() const
    {
        return mFullInfo;
    }
    inline const std::uint32_t getUpdateAge() const
    {
        return mUpdateAge;
    }
    inline const Priority getLastPriority() const
    {
        return mLastPriority;
    }
    inline const bool getAttemptValid() const
    {
        return mAttemptValid;
    }
    inline const double getLatitude() const
    {
        return mLatitude;
    }
    inline const double getLongitude() const
    {
        return mLongitude;
    }
    inline const std::int32_t getAltitude() const
    {
        return mAltitude;
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

    std::int32_t mAircraftType = A_ADSB_T;
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
    // deg
    double mLatitude = A_VALUE_NA;
    double mLongitude = A_VALUE_NA;
    // m
    std::int32_t mAltitude = 0;

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
