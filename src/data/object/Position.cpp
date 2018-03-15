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

#include "Position.h"

namespace data
{
namespace object
{
GpsPosition::GpsPosition() : Object(0)
{}

GpsPosition::GpsPosition(std::uint32_t vPriority, bool vGround)
    : Object(vPriority), mGround(vGround)
{}

GpsPosition::GpsPosition(const Position& crPosition, double vGeoid, bool vGround)
    : Object(0), mPosition(crPosition), mGeoid(vGeoid), mGround(vGround)
{}

GpsPosition::~GpsPosition() noexcept
{}

void GpsPosition::assign(const Object& crOther)
{
    Object::assign(crOther);
    const GpsPosition& crUpdate = static_cast<const GpsPosition&>(crOther);
    this->mPosition             = crUpdate.mPosition;
    this->mNrOfSatellites       = crUpdate.mNrOfSatellites;
    this->mFixQuality           = crUpdate.mFixQuality;
    this->mGeoid                = crUpdate.mGeoid;
    this->mDilution             = crUpdate.mDilution;
    this->mGround               = crUpdate.mGround;
}
}  // namespace object
}  // namespace data
