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

#include "Data.h"
#include <algorithm>

namespace data
{
Data::Data()
{}

Data::~Data() noexcept
{}

std::size_t Data::registerFeed()
{
    mFeedAttempts.push_back(0);
    return mFeedAttempts.size() - 1;
}

void Data::clearAttempts(std::vector<std::uint64_t>& rVec)
{
    std::fill(rVec.begin(), rVec.end(), 0);
}
}
