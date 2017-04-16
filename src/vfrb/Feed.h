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

#include <cstdint>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

class Client;

class Feed
{
public:
    enum class InputType
        : std::uint32_t
        {
            APRSC,
        SBS,
        GPS,
        SENSOR
    };

    Feed(const std::string& /*name*/, std::uint32_t /*prio*/, InputType /*type*/,
         std::unordered_map<std::string, std::string>& /*kvmap*/);
    virtual ~Feed() noexcept;

    void run(boost::asio::signal_set& /*sigset*/);

    const std::string mName; //?
    const std::uint32_t mPriority;
    const InputType mType;

private:
    std::unordered_map<std::string, std::string> mKVmap;
    std::unique_ptr<Client> mClient;
};

#endif /* SRC_VFRB_FEED_H_ */
