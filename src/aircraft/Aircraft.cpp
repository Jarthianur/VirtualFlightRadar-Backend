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

Aircraft::~Aircraft() noexcept
{
}

Aircraft::Aircraft(std::string& r_id, struct GPSPosition& r_pos)
        : mID(r_id),
          mPosition(r_pos)
{
}

Aircraft::Aircraft(std::string& r_id, struct GPSPosition& r_pos, double gnd_spd,
                   std::uint32_t id_t, std::int32_t ac_t, double climb_r,
                   double turn_r, double heading)
        : mID(r_id),
          mIDtype(id_t),
          mAircraftType(ac_t),
          mPosition(r_pos),
          mGndSpeed(gnd_spd),
          mHeading(heading),
          mClimbRate(climb_r)
/*,mTurnRate(turn_r)*/
{
}

bool Aircraft::operator==(const Aircraft& r_other) const
{
    return this->mID == r_other.mID;
}

void Aircraft::update(const Aircraft& r_ac, Priority prio)
{
    // no update for ID
    this->mIDtype = r_ac.mIDtype;
    this->mAircraftType = r_ac.mAircraftType;
    this->mPosition = r_ac.mPosition;
    this->mGndSpeed = r_ac.mGndSpeed;
    this->mHeading = r_ac.mHeading;
    this->mClimbRate = r_ac.mClimbRate;
    /*this->mTurnRate = r_ac.mTurnRate;*/
    this->mFullInfo = r_ac.mFullInfo;
    this->mTargetType = r_ac.mTargetType;
    this->mUpdateAge = 0;
    this->mLastPriority = prio;
    this->mAttemptValid = (prio == Priority::LESSER);
}
