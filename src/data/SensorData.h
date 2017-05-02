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

/// ICAO standard atmospheric pressure at MSL
#define ICAO_STD_A 1013.25
/// ICAO standard temperature at MSL
/// @deprecated
#define ICAO_STD_T 15.0

/**
 * The SensorData class.
 *
 * This class holds data gathered from any sensor.
 * E.g. wind sensor, pressure sensor
 */
class SensorData
{
public:
	/**
	 * Constructor
	 */
    SensorData();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~SensorData() noexcept;
    /**
     * Get WIMWV sentence.
     * Wind data is invalid after this operation.
     *
     * @return the WIMWV sentence
     */
    std::string getWVstr();
    /**
     * Set WIMWV sentence.
     * Wind data is valid after this operation.
     * May fail due to Priority.
     *
     * @param prio the Priority attempting to write
     * @param r_wv the new WIMWV sentence to write
     */
    void setWVstr(Priority /*prio*/, const std::string& /*r_wv*/);
/**
 * Get the last registered pressure.
 *
 * @return the pressure
 */
    double getPress();
    /**
     * Set the new pressure.
     * May fail due to Priority.
     *
     * @param prio the Priority attempting to write
     * @param p the new pressure
     */
    void setPress(Priority /*prio*/, double /*p*/);

private:
    /// Data holding pressure
    struct Data<double> mPress;
    /// TmpData holding WIMWV sentence
    struct TmpData<std::string> mWV;
};

#endif /* SRC_DATA_SENSORDATA_H_ */
