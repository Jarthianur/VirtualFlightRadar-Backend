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

#include <boost/date_time/posix_time/posix_time.hpp>

#include "types.h"

namespace vfrb::object::date_time
{
s64 now()
{
    return static_cast<s64>(
        boost::posix_time::time_duration(boost::posix_time::microsec_clock::universal_time().time_of_day())
            .total_milliseconds());
}

u32 day()
{
    return static_cast<u32>(boost::posix_time::microsec_clock::universal_time().date().modjulian_day());
}
}  // namespace vfrb::object::date_time
