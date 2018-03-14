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

#include <cstdio>
#include <string>

#include "../../Math.hpp"

#define PROC_BUFF_S 4096

namespace data
{
/// @namespace processor
namespace processor
{
template<typename T>
class Processor
{
public:
    Processor()
    {}
    virtual ~Processor() noexcept
    {}

    virtual std::string process(const T& _1) = 0;

protected:
    void finishSentence(std::string& rDestStr)
    {
        std::snprintf(mBuffer, sizeof(mBuffer), "%02x\r\n",
                      math::checksum(mBuffer, sizeof(mBuffer)));
        rDestStr.append(mBuffer);
    }

    char mBuffer[PROC_BUFF_S] = "";
};
}
}
