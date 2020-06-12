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

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Types.hpp"

namespace vfrb::object::date_time
{
auto Now() -> s64 {
    return static_cast<s64>(
        boost::posix_time::time_duration(boost::posix_time::microsec_clock::universal_time().time_of_day())
            .total_milliseconds());
}

auto Day() -> u32 {
    return static_cast<u32>(boost::posix_time::microsec_clock::universal_time().date().modjulian_day());
}
}  // namespace vfrb::object::date_time
