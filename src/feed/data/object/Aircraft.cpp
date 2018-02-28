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

namespace feed
{
namespace data
{
namespace object
{
Aircraft::Aircraft() : Aircraft(0)
{}

Aircraft::Aircraft(std::uint32_t vPriority)
    : Object(vPriority),
      mIdType(IdType::ICAO),
      mAircraftType(AircraftType::POWERED_AIRCRAFT),
      mTargetType(TargetType::TRANSPONDER)
{}

Aircraft::Aircraft(std::string& rId, GpsPosition& rPos)
    : Object(),
      mId(rId),
      mIdType(IdType::ICAO),
      mAircraftType(AircraftType::POWERED_AIRCRAFT),
      mTargetType(TargetType::TRANSPONDER),
      mPosition(rPos)
{}

Aircraft::Aircraft(std::string& rId, IdType vIdType, AircraftType vAircraftType,
                   GpsPosition& rPos, Movement& rMove)
    : Object(),
      mId(rId),
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

void Aircraft::assign(const Object& crOther)
{
    const Aircraft& crUpdate = static_cast<const Aircraft&>(crOther);
    this->mIdType            = crUpdate.mIdType;
    this->mAircraftType      = crUpdate.mAircraftType;
    this->mTargetType        = crUpdate.mTargetType;
    this->mPosition          = crUpdate.mPosition;
    this->mMovement          = crUpdate.mMovement;
    this->mFullInfo          = crUpdate.mFullInfo;
    this->mUpdateAge         = 0;
    this->mLastPriority      = crUpdate.mLastPriority;
}

bool Aircraft::canUpdate(const Object& crOther, std::uint64_t vAttempts) const
{
    const Aircraft& crKnown = static_cast<const Aircraft&>(crOther);
    return (crKnown.mTargetType == TargetType::TRANSPONDER
            || this->mTargetType == TargetType::FLARM)
           && (this->mLastPriority * vAttempts >= crKnown.mLastPriority);
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

const std::string& Aircraft::getSerialized() const
{
    return mSerialized;
}

void Aircraft::setId(const std::string& crId)
{
    mId = crId;
}

void Aircraft::setPosition(const GpsPosition& crPos)
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

void Aircraft::setSerialized(const std::string& crSerial)
{
    mSerialized = crSerial;
}
}
}  // namespace object
}  // namespace data
