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

#ifndef SRC_DATA_SENSORDATA_H_
#define SRC_DATA_SENSORDATA_H_

#include <string>

#include "../util/Priority.h"
#include "Data.hpp"

#define ICAO_STD_A 1013.25
#define ICAO_STD_T 15.0

class SensorData
{
public:
    SensorData();
    virtual ~SensorData() noexcept;

    /**
     * Get WIMWV sentence.
     * Set valid to false.
     */
    std::string getWVstr();

    /**
     * Set WIMWV sentence.
     * Set valid to true.
     */
    void setWVstr(Priority /*prio*/, const std::string& /*wv*/);

    double getPress();
    void setPress(Priority /*prio*/, double /*p*/);

private:
    struct Data<double> mPress;
    struct TmpData<std::string> mWV;
};

#endif /* SRC_DATA_SENSORDATA_H_ */
