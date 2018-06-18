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

Aircraft::~Aircraft() noexcept
{}

void Aircraft::assign(Object&& rvOther)
{
    Object::assign(std::move(rvOther));
    Aircraft&& rvUpdate = static_cast<Aircraft&&>(rvOther);
    this->mIdType       = rvUpdate.mIdType;
    this->mAircraftType = rvUpdate.mAircraftType;
    this->mTargetType   = rvUpdate.mTargetType;
    this->mPosition     = rvUpdate.mPosition;
    this->mMovement     = rvUpdate.mMovement;
    this->mTimeStamp    = rvUpdate.mTimeStamp;
    this->mFullInfo     = rvUpdate.mFullInfo;
    this->mUpdateAge    = 0;
}

bool Aircraft::canUpdate(const Object& crOther, std::uint32_t vAttempts) const
{
    const Aircraft& crUpdate = static_cast<const Aircraft&>(crOther);
    return (crUpdate.mTargetType == TargetType::TRANSPONDER
            || this->mTargetType == TargetType::FLARM)
           && (this->mLastPriority * vAttempts >= crUpdate.mLastPriority);
}

void Aircraft::setAircraftType(Aircraft::AircraftType vType)
{
    mAircraftType = vType < AircraftType::UNKNOWN || vType > AircraftType::STATIC_OBJECT
                        ? AircraftType::UNKNOWN
                        : vType;
}

void Aircraft::setIdType(Aircraft::IdType vType)
{
    mIdType = vType < IdType::UNRECOGNIZED || vType > IdType::OGN ? IdType::UNRECOGNIZED : vType;
}

}  // namespace object
