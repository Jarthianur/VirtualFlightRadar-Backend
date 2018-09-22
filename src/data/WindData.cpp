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

#include "WindData.h"

using namespace object;

namespace data
{
WindData::WindData() : Data()
{}

WindData::WindData(const object::Wind& wind) : Data(), m_wind(wind)
{}

WindData::~WindData() noexcept
{}

std::string WindData::get_serialized()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string tmp((++m_wind).get_serialized());
    m_wind.set_serialized("");
    return tmp;
}

bool WindData::update(Object&& wind)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_wind.tryUpdate(std::move(wind));
}

}  // namespace data
