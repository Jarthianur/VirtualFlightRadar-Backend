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

#include <cstdint>
#include <string>

#include "../util/SensorInfo.h"
#include "Data.hpp"
#include "Wrapper.hpp"

/// ICAO standard atmospheric pressure at MSL
#define ICAO_STD_A 1013.25

namespace data
{

/**
 * @class SensorData implements Data
 * @brief Manage sensor information.
 * @see Data.hpp
 */
class SensorData: public Data<struct util::SensorInfo>
{
public:
    /**
     * @fn SensorData
     * @brief Constructor
     */
    SensorData();
    /**
     * @fn ~SensorData
     * @brief Destructor
     */
    virtual ~SensorData() noexcept;
    /**
     * @fn init
     * @brief Initialize the sensor information.
     * @param cr_info The initial data
     */
    void init(const struct util::SensorInfo& cr_info) override;
    /**
     * @fn update
     * @brief Try to update the sensor information.
     * @note Splits the given info, using setters.
     * @param cr_info The new sensor information.
     * @param prio    The attempts priority
     * @threadsafe
     */
    void update(const struct util::SensorInfo& cr_info, std::int32_t prio) override;
    /**
     * @fn getMwvStr
     * @brief Get the MWV sentence.
     * @note MWV is invalid after this operation.
     * @return the MWV sentence, if valid, else empty string
     * @threadsafe
     */
    std::string getMwvStr();
    /**
     * @fn getMdaStr
     * @brief Get the MDA sentence.
     * @note MDA is invalid after this operation.
     * @return the MDA sentence, if valid, else empty string
     * @threadsafe
     */
    std::string getMdaStr();
    /**
     * @fn getPress
     * @brief Get pressure.
     * @return the pressure
     * @threadsafe
     */
    double getPress();

private:
    /**
     * @fn setMdaStr
     * @brief Set MDA sentence.
     * @note MDA is valid after this operation.
     * @param cr_mda The new MDA sentence
     * @param prio   The attempts priority
     * @threadsafe
     */
    void setMdaStr(const std::string& cr_mda, std::int32_t prio);
    /**
     * @fn setMwvStr
     * @brief Set MWV sentence.
     * @note MWV is valid after this operation.
     * @param cr_mwv The new MWV sentence
     * @param prio   The attempts priority
     * @threadsafe
     */
    void setMwvStr(const std::string& cr_mwv, std::int32_t prio);
    /**
     * @fn setPress
     * @brief Set pressure.
     * @param p    The new pressure
     * @param prio The attempts priority
     * @threadsafe
     */
    void setPress(double p, std::int32_t prio);
    /// Holding pressure
    struct Wrapper<double> mPress;
    /// Holding MWV sentence
    struct TmpWrapper<std::string> mMwvData;
    /// Holding MDA sentence
    struct TmpWrapper<std::string> mMdaData;
};

} // namespace data

#endif /* SRC_DATA_SENSORDATA_H_ */
