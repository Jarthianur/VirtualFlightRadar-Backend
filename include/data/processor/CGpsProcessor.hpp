/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <ctime>

#include "object/CGpsPosition.hpp"
#include "util/StringUtils.hpp"

#include "Types.hpp"

namespace vfrb::data::processor
{
class CGpsProcessor
{
private:
    char mutable m_directionSN = 'x';
    char mutable m_directionEW = 'x';
    f64 mutable m_degLatitude  = 0.0;
    f64 mutable m_degLongitude = 0.0;
    f64 mutable m_minLatitude  = 0.0;
    f64 mutable m_minLongitude = 0.0;

    void
    appendGpgga(object::CGpsPosition const& pos_, std::tm const* utc_, str_util::StringInserter& nmea_) const;

    void
    appendGprmc(std::tm const* utc_, str_util::StringInserter& nmea_) const;

    void
    evalPosition(f64 lat_, f64 lon_) const;

public:
    void
    Process(object::CGpsPosition const& pos_, str_util::StringInserter& nmea_) const;
};
}  // namespace vfrb::data::processor
