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
#include "Data.hpp"

namespace data
{

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
     * Get MWV sentence.
     * Wind data is invalid after this operation.
     *
     * @return the MWV sentence, if valid, else empty string
     */
    std::string getMWVstr();
    /**
     * Set MWV sentence.
     * Wind data is valid after this operation.
     * May fail due to priority.
     *
     * @param prio the priority attempting to write
     * @param cr_mwv the new MWV sentence to write
     */
    void setMWVstr(std::int32_t /*prio*/, const std::string& /*cr_mwv*/);
    /**
     * Get MDA sentence.
     * Data is invalid after this operation.
     *
     * @return the MDA sentence, if valid, else empty string
     */
    std::string getMDAstr();
    /**
     * Set MDA sentence.
     * Data is valid after this operation.
     * May fail due to priority.
     *
     * @param prio the priority attempting to write
     * @param cr_mda the new MDA sentence to write
     */
    void setMDAstr(std::int32_t /*prio*/, const std::string& /*cr_mda*/);
    /**
     * Get the last registered pressure.
     *
     * @return the pressure
     */
    double getPress();
    /**
     * Set the new pressure.
     * May fail due to priority.
     *
     * @param prio the priority attempting to write
     * @param p the new pressure
     */
    void setPress(std::int32_t /*prio*/, double /*p*/);

private:
    /// Data holding pressure
    struct Data<double> mPress;
    /// TmpData holding MWV sentence
    struct TmpData<std::string> mMWV;
    /// TmpData holding MDA sentence
    struct TmpData<std::string> mMDA;
};

} // namespace data

#endif /* SRC_DATA_SENSORDATA_H_ */
