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

#ifndef SRC_FEED_FEED_H_
#define SRC_FEED_FEED_H_

#include <cstdint>
#include <memory>
#include <string>
#include <boost/asio/signal_set.hpp>
#include <boost/move/core.hpp>

#include "../config/PropertyMap.h"

namespace network
{
namespace client
{
class Client;
}
}

namespace feed
{
/**
 * @class Feed
 * @brief Incomplete base-class representing an input feed.
 * @note A Feed is unique and only movable.
 */
class Feed
{
    BOOST_MOVABLE_BUT_NOT_COPYABLE(Feed)

public:
    /**
     * @fn ~Feed
     * @brief Destructor
     */
    virtual ~Feed() noexcept;

    /**
     * @fn Feed
     * @brief Move-constructor
     * @param The Feed to move
     */
    Feed(BOOST_RV_REF(Feed));

    /**
     * @fn operator=
     * @brief Move-assignment
     * @param The Feed to assign
     */
    Feed& operator=(BOOST_RV_REF(Feed));

    /**
     * @fn run
     * @brief Run a Feed.
     * @param r_sigset The signal set to pass to the Client
     */
    void run(boost::asio::signal_set& rSigset) noexcept;

    /**
     * @fn process
     * @brief Handle Clients response.
     * @note To be implemented.
     * @param cr_res The response to process
     */
    virtual void process(const std::string& crResponse) noexcept = 0;

    const std::string& getName() const;
    std::uint32_t getPriority() const;

protected:
    /**
     * @fn Feed
     * @brief Constructor
     * @param cr_name  The Feeds unique name
     * @param cr_kvmap The properties map
     * @throws std::logic_error if host or port are not given
     */
    Feed(const std::string& crName, const config::KeyValueMap& crKvMap);

    /// Unique name
    const std::string mName;

    /// Key-value-map holding the properties.
    const config::KeyValueMap mKvMap;

    /// Client, later resolved according to InpuType
    std::unique_ptr<network::client::Client> mpClient;

private:
    void initPriority();

    /// Priority to write data
    std::uint32_t mPriority;
};

}  // namespace feed

#endif /* SRC_FEED_FEED_H_ */
