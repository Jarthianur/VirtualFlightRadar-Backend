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

class GPSData
{
public:
    GPSData();
    virtual ~GPSData();

    void insertRMC();
    void insertGGA();
    std::string extractRMC();
    std::string extractGGA();

    std::int32_t getBaseAlt();
    double getBaseLat();
    double getBaseLong();

private:
    std::int32_t mBaseAlt = 0, mNrSats = 0, mFixQa = 0;
    /**
     * Base position in degree
     */
    double mBaseLat = 0.0, mBaseLong = 0.0;

    std::string mGPGGAstr, mGPRMCstr;
};

#endif /* SRC_DATA_GPSDATA_H_ */
