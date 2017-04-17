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

#include "../util/Priority.h"

class Parser;
class Client;

class Feed
{
BOOST_MOVABLE_BUT_NOT_COPYABLE(Feed)

public:
    enum class InputType
        : std::uint32_t
        {
            APRSC,
        SBS,
        GPS,
        SENSOR
    };

    Feed(const std::string& /*name*/, Priority /*prio*/, InputType /*type*/,
         const std::unordered_map<std::string, std::string>& /*kvmap*/);
    virtual ~Feed() noexcept;

    Feed(BOOST_RV_REF(Feed));
    Feed& operator=(BOOST_RV_REF(Feed));

    void run(boost::asio::signal_set& /*sigset*/);
    std::int32_t process(const std::string& /*data*/) noexcept;

    const std::string mName; //?
    const Priority mPriority;
    const InputType mType;

private:
    std::unordered_map<std::string, std::string> mKVmap;
    std::unique_ptr<Client> mpClient;
    std::unique_ptr<Parser> mpParser;
};

#endif /* SRC_VFRB_FEED_H_ */
