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

#pragma once

#include "concurrent/WorkerThread.hpp"
#include "parser/AprsParser.h"

#include "Feed.h"

namespace vfrb::data
{
class CAircraftData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Extend Feed for APRSC protocol.
 */
class CAprscFeed : public IFeed
{
    NOT_COPYABLE(CAprscFeed)

    static constexpr auto LOG_PREFIX = "(AprscFeed) ";

    parser::CAprsParser const      m_parser;  ///< Parser to unpack response from Client
    concurrent::CWorkerThread<Str> m_worker;

public:
    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The AircraftData container
     * @param maxHeight  The max height filter
     * @throw std::logic_error if login is not given, or from parent constructor
     */
    CAprscFeed(Str const& name_, config::CProperties const& prop_, SPtr<data::CAircraftData> data_,
               s32 maxHeight_);
    ~CAprscFeed() noexcept override = default;

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::APRS
     */
    EProtocol Protocol() const override;

    /**
     * @brief Implement Feed::process.
     */
    bool Process(Str str_) override;

    /**
     * @brief Get the login string.
     * @return the login
     */
    Str Login() const;
};
}  // namespace vfrb::feed
