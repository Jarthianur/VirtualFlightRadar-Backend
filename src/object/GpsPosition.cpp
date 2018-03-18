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

#include "GpsPosition.h"

namespace object
{
GpsPosition::GpsPosition() : Object(0)
{}

GpsPosition::GpsPosition(std::uint32_t vPriority) : Object(vPriority)
{}

GpsPosition::GpsPosition(const Position& crPosition, double vGeoid)
    : Object(0), mPosition(crPosition), mGeoid(vGeoid)
{}

GpsPosition::~GpsPosition() noexcept
{}

void GpsPosition::assign(Object&& rvOther)
{
    Object::assign(std::move(rvOther));
    GpsPosition&& rvUpdate = static_cast<GpsPosition&&>(rvOther);
    this->mPosition        = rvUpdate.mPosition;
    this->mNrOfSatellites  = rvUpdate.mNrOfSatellites;
    this->mFixQuality      = rvUpdate.mFixQuality;
    this->mGeoid           = rvUpdate.mGeoid;
    this->mDilution        = rvUpdate.mDilution;
}
}  // namespace object
