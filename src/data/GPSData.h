/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#ifndef SRC_DATA_GPSDATA_H_
#define SRC_DATA_GPSDATA_H_

#include <cstdint>
#include <string>

#include "../util/GPSmodule.h"
#include "../util/Priority.h"
#include "Data.hpp"
#include "Position.hpp"

class GPSData
{
public:
    GPSData();
    virtual ~GPSData() noexcept;

    void setDefaults(double /*b_lat*/, double /*b_lon*/, std::int32_t /*b_alt*/,
                     double /*geoid*/);

    std::string getGPSstr();

    void setGGAstr(Priority /*prio*/, const std::string& /*gga*/);
    void setRMCstr(Priority /*prio*/, const std::string& /*rmc*/);

    void setBasePos(Priority /*prio*/, const struct ExtGPSPosition& /*pos*/);
    struct ExtGPSPosition getBasePos();
    std::int32_t getBaseAlt();
    double getBaseLat();
    double getBaseLong();

private:
    std::string getGGAstr();
    std::string getRMCstr();

    struct Data<struct ExtGPSPosition> mBasePos;
    struct TmpData<std::string> mGGAstr;
    struct TmpData<std::string> mRMCstr;
    GPSmodule mGPSfix;
};

#endif /* SRC_DATA_GPSDATA_H_ */
