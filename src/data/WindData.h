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

#ifndef SRC_DATA_WINDDATA_H_
#define SRC_DATA_WINDDATA_H_

#include <cstdint>
#include <string>

#include <boost/thread/mutex.hpp>
#include "../util/Sensor.h"
#include "Data.hpp"
#include "../util/Wrapper.hpp"

namespace feed {
class SensorFeed;
}

namespace data
{

/**
 * @class WindData implements Data
 * @brief Manage sensor information.
 * @see Data.hpp
 */
class WindData: public Data<struct util::Wind>
{
public:
	/**
	 * @fn WindData
	 * @brief Constructor
	 */
	WindData();

	/**
	 * @fn ~WindData
	 * @brief Destructor
	 */
	virtual ~WindData() noexcept;

	/**
	 * @fn init
	 * @brief Initialize the sensor information.
	 * @param info The initial data
	 * @override Data::init
	 */
	void init(struct util::Wind vWind) override;

        /**
	 * @fn getMwvStr
	 * @brief Get the MWV sentence.
	 * @note MWV is invalid after this operation.
	 * @return the MWV sentence, if valid, else empty string
	 * @threadsafe
	 */
	std::string getMwvStr();

protected:
    friend class feed::SensorFeed;
    boost::mutex mMutex;
    /**
     * @fn update
     * @brief Try to update the sensor information.
     * @note Splits the given info, using setters.
     * @param cr_info The new sensor information.
     * @param prio    The attempts priority
     * @override Data::update
     * @threadsafe
     */
    void update(const struct util::Wind& crWind, std::uint32_t vPriority,
            std::uint64_t& rAttempts) override;

private:

    /// Holding MDA sentence
    struct util::TmpWrapper<util::Wind> mWind;
};

} // namespace data

#endif /* SRC_DATA_WINDDATA_H_ */
