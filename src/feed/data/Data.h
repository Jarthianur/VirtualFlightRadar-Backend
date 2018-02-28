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

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <boost/thread/mutex.hpp>

namespace feed
{
namespace data
{
namespace object
{
class Object;
}

class Data
{
public:
    Data();
    virtual ~Data() noexcept;
    virtual std::string getSerialized()                              = 0;
    virtual bool update(const object::Object& _1, std::size_t vSlot) = 0;
    virtual std::size_t registerFeed();

protected:
    virtual void clearAttempts(std::vector<std::uint64_t>& rVec);
    /// @var mMutex
    /// Used for RW on this data
    boost::mutex mMutex;
    std::vector<std::uint64_t> mFeedAttempts;
};
}
}
