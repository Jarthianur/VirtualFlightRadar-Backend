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

#ifndef SRC_VFRB_FEED_H_
#define SRC_VFRB_FEED_H_

#include <boost/asio/signal_set.hpp>
#include <boost/move/core.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace parser
{
class Parser;
}
namespace tcp
{
namespace client
{
class Client;
}
}

namespace vfrb
{

/**
 * The Feed class.
 *
 * This class represents an input feed.
 * A Feed is unique and handles its Client and Parser
 * according to its type.
 */
class Feed
{
BOOST_MOVABLE_BUT_NOT_COPYABLE(Feed)

public:
    /**
     * The InputType of a Feed.
     * Determines the type of Client and Parser to use.
     */
    enum class InputType
        : std::uint32_t
        {
            /// Use APRSCClient and APRSParser
        APRSC,
        /// Use SBSClient and SBSParser
        SBS,
        /// Use GPSDClient and GPSParser
        GPS,
        /// Use SensorClient and SensorParser
        SENSOR
    };
    /**
     * Construct a Feed with its meta-data and
     * a key-value-map holding all properties.
     *
     * @param cr_name  the Feeds unique name
     * @param prio     the Priority
     * @param type     the InputType
     * @param cr_kvmap the properties map
     */
    Feed(const std::string& /*cr_name*/, std::int32_t /*prio*/,
         InputType /*type*/,
         const std::unordered_map<std::string, std::string>& /*cr_kvmap*/);
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
     * This function resolves the Feeds properties
     * from the key-value-map and wraps the Clients
     * run method.
     *
     * @param r_sigset the signal set to pass to the Client
     *
     * @exceptsafe strong
     */
    void run(boost::asio::signal_set& /*r_sigset*/) noexcept;
    /**
     * Pass the Clients response to the Parser.
     *
     * @param cr_res the response to process
     *
     * @return the Parsers return value
     *
     * @exceptsafe no-throw
     */
    std::int32_t process(const std::string& /*cr_res*/) noexcept;

    /// Unique name
    const std::string mName;
    /// Priority to write data
    const std::int32_t mPriority;
    /// Type
    const InputType mType;

private:
    /// Key-value-map holding the properties.
    std::unordered_map<std::string, std::string> mKVmap;
    /// Client, later resolved according to InpuType
    std::unique_ptr<tcp::client::Client> mpClient;
    /// Parser, later resolved according to InpuType
    std::unique_ptr<parser::Parser> mpParser;
};

}  // namespace vfrb

#endif /* SRC_VFRB_FEED_H_ */
