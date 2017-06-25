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

#include <boost/asio/signal_set.hpp>
#include <boost/move/core.hpp>
#include <cstdint>
#include <memory>
#include <string>

#include "../config/PropertyMap.h"

namespace tcp
{
namespace client
{
class Client;
}
}

namespace feed
{

/**
 * The Feed class.
 *
 * This class represents an input feed.
 * A Feed is unique and movable but not copyable.
 * The Client and Parser are resolved by concrete
 * derived classes.
 */
class Feed
{
BOOST_MOVABLE_BUT_NOT_COPYABLE(Feed)

public:
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~Feed() noexcept;
    /**
     * Move-constructor
     */
    Feed(BOOST_RV_REF(Feed));
    /**
     * Move-assignment
     */
    Feed& operator=(BOOST_RV_REF(Feed));
    /**
     * Run a Feed.
     *
     * @param r_sigset the signal set to pass to the Client
     *
     * @exceptsafe strong
     */
    void run(boost::asio::signal_set& r_sigset) noexcept;
    /**
     * Pass the Clients response to the Parser.
     *
     * @param cr_res the response to process
     *
     * @return the Parsers return value
     *
     * @exceptsafe no-throw
     */
    virtual std::int32_t process(const std::string& cr_res) noexcept = 0;

    /// Unique name
    const std::string mName;
    /// Priority to write data
    const std::int32_t mPriority;

protected:
    /**
     * Construct a Feed with its meta-data and
     * a key-value-map holding all properties.
     * Host and port for the Client get resolved in here.
     *
     * @param cr_name  the Feeds unique name
     * @param prio     the priority
     * @param cr_kvmap the properties map
     *
     * @throws std::runtime_error if host or port are not given
     */
    Feed(const std::string& cr_name, std::int32_t prio,
            const config::keyValueMap& cr_kvmap);
    /// Key-value-map holding the properties.
    const config::keyValueMap mKvMap;
    /// Client, later resolved according to InpuType
    std::unique_ptr<tcp::client::Client> mpClient;
};

}  // namespace feed

#endif /* SRC_FEED_FEED_H_ */
