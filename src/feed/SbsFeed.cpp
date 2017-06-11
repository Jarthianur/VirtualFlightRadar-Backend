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

#include "SbsFeed.h"

#include "../parser/SBSParser.h"
#include "../tcp/client/SBSClient.h"
#include "../config/Configuration.h"

namespace feed
{

SbsFeed::SbsFeed(const std::string& cr_name, std::int32_t prio,
                 const std::unordered_map<std::string, std::string>& cr_kvmap)
        : Feed(cr_name, prio, cr_kvmap)
{
    mpParser = std::unique_ptr<parser::Parser>(new parser::SBSParser());
    mpClient = std::unique_ptr<tcp::client::Client>(
            new tcp::client::SBSClient(mKVmap.find(KV_KEY_HOST)->second,
                    mKVmap.find(KV_KEY_PORT)->second, *this));
}

SbsFeed::~SbsFeed() noexcept
{
}

} // namespace feed
