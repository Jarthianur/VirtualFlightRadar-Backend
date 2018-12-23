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

#include "object/impl/DateTimeImplBoost.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace object
{
namespace timestamp
{
std::int64_t DateTimeImplBoost::now()
{
    return static_cast<std::int64_t>(
        boost::posix_time::time_duration(
            boost::posix_time::microsec_clock::universal_time().time_of_day())
            .total_milliseconds());
}

std::uint32_t DateTimeImplBoost::day()
{
    return static_cast<std::uint32_t>(
        boost::posix_time::microsec_clock::universal_time().date().modjulian_day());
}

}  // namespace timestamp
}  // namespace object
