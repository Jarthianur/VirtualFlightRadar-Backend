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
ExtGpsPosition::ExtGpsPosition() : Object(0)
{}

ExtGpsPosition::ExtGpsPosition(std::uint32_t vPriority, bool vGround)
    : Object(vPriority), ground(vGround)
{}

ExtGpsPosition::ExtGpsPosition(const GpsPosition& crPosition, double vGeoid)
    : Object(0), position(crPosition), geoid(vGeoid)
{}

ExtGpsPosition::~ExtGpsPosition() noexcept
{}

void ExtGpsPosition::assign(const Object& crOther)
{
    const ExtGpsPosition& crUpdate = static_cast<const ExtGpsPosition&>(crOther);
    this->position                 = crUpdate.position;
    this->nrSats                   = crUpdate.nrSats;
    this->fixQa                    = crUpdate.fixQa;
    this->geoid                    = crUpdate.geoid;
    this->dilution                 = crUpdate.dilution;
    this->ground                   = crUpdate.ground;
    this->mLastPriority            = crUpdate.mLastPriority;

}
}
}
