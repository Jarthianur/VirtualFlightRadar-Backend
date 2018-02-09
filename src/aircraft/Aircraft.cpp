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

#include "Aircraft.hpp"

using namespace util;

namespace aircraft
{
Aircraft::~Aircraft() noexcept
{}

Aircraft::Aircraft()
{}

Aircraft::Aircraft(std::string& r_id, struct GpsPosition& r_pos)
    : mId(r_id), mPosition(r_pos)
{}

Aircraft::Aircraft(std::string& r_id, struct GpsPosition& r_pos, double gnd_spd,
                   std::uint32_t id_t, std::int32_t ac_t,
                   double climb_r, /*double turn_r,*/
                   double heading)
    : mId(r_id),
      mIdType(id_t),
      mAircraftType(ac_t),
      mPosition(r_pos),
      mGndSpeed(gnd_spd),
      mHeading(heading),
      mClimbRate(climb_r)
{}

bool Aircraft::operator==(const Aircraft& cr_other) const
{
    return this->mId == cr_other.mId;
}

void Aircraft::update(const Aircraft& cr_ac, std::uint32_t prio)
{
    // no update for ID
    this->mIdType       = cr_ac.mIdType;
    this->mAircraftType = cr_ac.mAircraftType;
    this->mPosition     = cr_ac.mPosition;
    this->mGndSpeed     = cr_ac.mGndSpeed;
    this->mHeading      = cr_ac.mHeading;
    this->mClimbRate    = cr_ac.mClimbRate;
    // this->mTurnRate = r_ac.mTurnRate;
    this->mFullInfo       = cr_ac.mFullInfo;
    this->mTargetType     = cr_ac.mTargetType;
    this->mUpdateAge      = 0;
    this->mLastPriority   = prio;
    this->mUpdateAttempts = 0;
}

inline const std::string& Aircraft::getId() const
{
    return mId;
}

inline const std::uint32_t Aircraft::getIdType() const
{
    return mIdType;
}

inline const Aircraft::TargetType Aircraft::getTargetType() const
{
    return mTargetType;
}

inline const std::int32_t Aircraft::getAircraftType() const
{
    return mAircraftType;
}

inline const bool Aircraft::hasFullInfo() const
{
    return mFullInfo;
}

inline std::uint64_t& Aircraft::getUpdateAge()
{
    return mUpdateAge;
}

inline const std::uint32_t Aircraft::getLastPriority() const
{
    return mLastPriority;
}

inline const double Aircraft::getLatitude() const
{
    return mPosition.latitude;
}

inline const double Aircraft::getLongitude() const
{
    return mPosition.longitude;
}

inline const std::int32_t Aircraft::getAltitude() const
{
    return mPosition.altitude;
}

inline const double Aircraft::getGndSpeed() const
{
    return mGndSpeed;
}

inline const double Aircraft::getHeading() const
{
    return mHeading;
}

inline const double Aircraft::getClimbRate() const
{
    return mClimbRate;
}

inline std::uint64_t& Aircraft::getUpdateAttempts()
{
    return mUpdateAttempts;
}

inline void Aircraft::setId(const std::string& cr_id)
{
    mId = cr_id;
}

inline void Aircraft::setPosition(const struct util::GpsPosition& cr_pos)
{
    mPosition = cr_pos;
}

inline void Aircraft::setAircraftT(const std::int32_t c_act)
{
    mAircraftType = c_act;
}

inline void Aircraft::setIdType(const std::uint32_t c_idt)
{
    mIdType = c_idt;
}

inline void Aircraft::setClimbRate(const double c_climb = A_VALUE_NA)
{
    mClimbRate = c_climb;
}

inline void Aircraft::setHeading(const double c_head = A_VALUE_NA)
{
    mHeading = c_head;
}

inline void Aircraft::setGndSpeed(const double c_gndspd = A_VALUE_NA)
{
    mGndSpeed = c_gndspd;
}

inline void Aircraft::setTargetT(Aircraft::TargetType tt)
{
    mTargetType = tt;
}

inline void Aircraft::setFullInfo(bool info = true)
{
    mFullInfo = info;
}

inline void Aircraft::setLastPriority(std::uint32_t prio)
{
    mLastPriority = prio;
}

}  // namespace aircraft
