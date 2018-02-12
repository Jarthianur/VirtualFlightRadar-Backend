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

#include "Aircraft.h"

using namespace util;

namespace aircraft
{
Aircraft::Aircraft()
{}

Aircraft::Aircraft(std::uint32_t vPriority) : mLastPriority(vPriority)
{}

Aircraft::Aircraft(std::string& rId, struct GpsPosition& rPos)
    : mId(rId),
      mIdType(IdType::ICAO),
      mAircraftType(AircraftType::POWERED_AIRCRAFT),
      mTargetType(TargetType::TRANSPONDER),
      mPosition(rPos)
{}

Aircraft::Aircraft(std::string& rId, IdType vIdType, AircraftType vAircraftType,
                   struct GpsPosition& rPos, struct Movement& rMove)
    : mId(rId),
      mIdType(vIdType),
      mAircraftType(vAircraftType),
      mTargetType(TargetType::FLARM),
      mPosition(rPos),
      mMovement(rMove)
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
    this->mTargetType     = crUpdate.mTargetType;
    this->mPosition       = crUpdate.mPosition;
    this->mMovement       = crUpdate.mMovement;
    this->mFullInfo       = crUpdate.mFullInfo;
    this->mUpdateAge      = 0;
    this->mLastPriority   = vPriority;
    this->mUpdateAttempts = 0;
}

const std::string& Aircraft::getId() const
{
    return mId;
}

Aircraft::IdType Aircraft::getIdType() const
{
    return mIdType;
}

Aircraft::TargetType Aircraft::getTargetType() const
{
    return mTargetType;
}

Aircraft::AircraftType Aircraft::getAircraftType() const
{
    return mAircraftType;
}

bool Aircraft::hasFullInfo() const
{
    return mFullInfo;
}

std::uint64_t& Aircraft::getUpdateAge()
{
    return mUpdateAge;
}

std::uint32_t Aircraft::getLastPriority() const
{
    return mLastPriority;
}

double Aircraft::getLatitude() const
{
    return mPosition.latitude;
}

double Aircraft::getLongitude() const
{
    return mPosition.longitude;
}

std::int32_t Aircraft::getAltitude() const
{
    return mPosition.altitude;
}

double Aircraft::getGndSpeed() const
{
    return mMovement.gndSpeed;
}

double Aircraft::getHeading() const
{
    return mMovement.heading;
}

double Aircraft::getClimbRate() const
{
    return mMovement.climbRate;
}

std::uint64_t& Aircraft::getUpdateAttempts()
{
    return mUpdateAttempts;
}

void Aircraft::setId(const std::string& crId)
{
    mId = crId;
}

void Aircraft::setPosition(const struct util::GpsPosition& crPos)
{
    mPosition = crPos;
}

void Aircraft::setAircraftType(Aircraft::AircraftType vType)
{
    mAircraftType = vType < AircraftType::UNKNOWN || vType > AircraftType::STATIC_OBJECT
                        ? AircraftType::UNKNOWN
                        : vType;
}

void Aircraft::setIdType(Aircraft::IdType vType)
{
    mIdType = vType < IdType::UNRECOGNIZED || vType > IdType::OGN ? IdType::UNRECOGNIZED
                                                                  : vType;
}

void Aircraft::setClimbRate(double vRate)
{
    mMovement.climbRate = vRate;
}

void Aircraft::setHeading(double vHeading)
{
    mMovement.heading = vHeading;
}

void Aircraft::setGndSpeed(double vGndSpd)
{
    mMovement.gndSpeed = vGndSpd;
}

void Aircraft::setTargetType(Aircraft::TargetType vType)
{
    mTargetType = vType;
}

void Aircraft::setFullInfo(bool vInfo)
{
    mFullInfo = vInfo;
}

void Aircraft::setLastPriority(std::uint32_t vPriority)
{
    mLastPriority = vPriority;
}

}  // namespace aircraft
