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

#ifndef SRC_FEED_SBSFEED_H_
#define SRC_FEED_SBSFEED_H_

#include "Feed.h"

namespace feed
{

/**
 * The SbsFeed class.
 *
 * This inherits from Feed and
 * resolves to SBS handling Client and Parser.
 */
class SbsFeed: public Feed
{
public:
    /**
     * Construct an SbsFeed.
     * Pass all parameters to parent constructor.
     */
    SbsFeed(const std::string& cr_name, std::int32_t prio,
            const config::keyValueMap& cr_kvmap);
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~SbsFeed() noexcept;
    std::int32_t process(const std::string& cr_res) noexcept override;
};

} // namespace feed

#endif /* SRC_FEED_SBSFEED_H_ */
