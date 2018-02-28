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

#include <string>
#include "Object.h"

/// ICAO standard atmospheric pressure at MSL
#define ICAO_STD_A 1013.25
namespace feed
{
namespace data
{
namespace object
{
class Atmosphere : public Object
{
public:
    Atmosphere();
    explicit Atmosphere(std::uint32_t vPriority);
    explicit Atmosphere(double vPress);
    virtual ~Atmosphere() noexcept;

    void assign(const Object& crOther);

    const std::string& getMdaStr() const;
    double getPressure() const;
    void setMdaStr(const std::string& crStr);
    void setPressure(double vPress);

private:
    friend struct Climate;
    std::string mMdaStr;
    double mPressure = ICAO_STD_A;
};

class Wind : public Object
{
public:
    Wind();
    explicit Wind(std::uint32_t vPriority);
    virtual ~Wind() noexcept;

    void assign(const Object& crOther);

    const std::string& getMwvStr() const;
    void setMwvStr(const std::string& crStr);

private:
    friend struct Climate;
    std::string mMwvStr;
};

struct Climate
{
    Wind mWind;
    Atmosphere mAtmosphere;

    inline bool hasAtmosphere()
    {
        return !mAtmosphere.mMdaStr.empty();
    }

    inline bool hasWind()
    {
        return !mWind.mMwvStr.empty();
    }
};

}  // namespace util
}
}
