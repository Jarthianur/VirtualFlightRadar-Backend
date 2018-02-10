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
Aircraft::Aircraft()
{}

Aircraft::Aircraft(std::string& rId, struct GpsPosition& rPos)
    : mId(rId),
      mPosition(rPos),
      mIdType(IdType::ICAO),
      mTargetType(TargetType::TRANSPONDER),
      mAircraftType(AircraftType::POWERED_AIRCRAFT)
{}

Aircraft::Aircraft(std::string& rId, struct GpsPosition& rPos, double vGndSpd,
                   IdType vIdType, AircraftType vAircraftType, double vClimbRate,
                   double vHeading)
    : mId(rId),
      mIdType(vIdType),
      mAircraftType(vAircraftType),
      mPosition(rPos),
      mGndSpeed(vGndSpd),
      mHeading(vHeading),
      mClimbRate(vClimbRate),
      mTargetType(TargetType::FLARM)
{}

Aircraft::~Aircraft() noexcept
{}

bool Aircraft::operator==(const Aircraft& crOther) const
{
    return this->mId == crOther.mId;
}

void Aircraft::update(const Aircraft& crUpdate, std::uint32_t vPriority)
{
    this->mIdType         = crUpdate.mIdType;
    this->mAircraftType   = crUpdate.mAircraftType;
    this->mPosition       = crUpdate.mPosition;
    this->mGndSpeed       = crUpdate.mGndSpeed;
    this->mHeading        = crUpdate.mHeading;
    this->mClimbRate      = crUpdate.mClimbRate;
    this->mFullInfo       = crUpdate.mFullInfo;
    this->mTargetType     = crUpdate.mTargetType;
    this->mUpdateAge      = 0;
    this->mLastPriority   = vPriority;
    this->mUpdateAttempts = 0;
}

inline const std::string& Aircraft::getId() const
{
    return mId;
}

inline Aircraft::IdType Aircraft::getIdType() const
{
    return mIdType;
}

inline Aircraft::TargetType Aircraft::getTargetType() const
{
    return mTargetType;
}

inline Aircraft::AircraftType Aircraft::getAircraftType() const
{
    return mAircraftType;
}

inline bool Aircraft::hasFullInfo() const
{
    return mFullInfo;
}

inline std::uint64_t& Aircraft::getUpdateAge()
{
    return mUpdateAge;
}

inline std::uint32_t Aircraft::getLastPriority() const
{
    return mLastPriority;
}

inline double Aircraft::getLatitude() const
{
    return mPosition.latitude;
}

inline double Aircraft::getLongitude() const
{
    return mPosition.longitude;
}

inline std::int32_t Aircraft::getAltitude() const
{
    return mPosition.altitude;
}

inline double Aircraft::getGndSpeed() const
{
    return mGndSpeed;
}

inline double Aircraft::getHeading() const
{
    return mHeading;
}

inline double Aircraft::getClimbRate() const
{
    return mClimbRate;
}

inline std::uint64_t& Aircraft::getUpdateAttempts()
{
    return mUpdateAttempts;
}

inline void Aircraft::setId(const std::string& crId)
{
    mId = crId;
}

inline void Aircraft::setPosition(const struct util::GpsPosition& crPos)
{
    mPosition = crPos;
}

inline void Aircraft::setAircraftType(Aircraft::AircraftType vType)
{
    mAircraftType = vType;
}

inline void Aircraft::setIdType(Aircraft::IdType vType)
{
    mIdType = vType;
}

inline void Aircraft::setClimbRate(double vRate)
{
    mClimbRate = vRate;
}

inline void Aircraft::setHeading(double vHeading)
{
    mHeading = vHeading;
}

inline void Aircraft::setGndSpeed(double vGndSpd)
{
    mGndSpeed = vGndSpd;
}

inline void Aircraft::setTargetType(Aircraft::TargetType vType)
{
    mTargetType = vType;
}

inline void Aircraft::setFullInfo(bool vInfo)
{
    mFullInfo = vInfo;
}

inline void Aircraft::setLastPriority(std::uint32_t vPriority)
{
    mLastPriority = vPriority;
}

}  // namespace aircraft
