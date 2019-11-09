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
#include "parser/SbsParser.h"

#include "Feed.h"

namespace vfrb::data
{
class CAircraftData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Extend Feed for SBS protocol.
 */
class SbsFeed : public Feed
{
    NOT_COPYABLE(SbsFeed)

    parser::SbsParser const       m_parser;  ///< Parser to unpack response from Client
    concurrent::CWorkerThread<Str> m_worker;

public:
    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The AircraftData container
     * @param maxHeight  The max height filter
     * @throw std::logic_error from parent constructor
     */
    SbsFeed(Str const& m_name, config::CProperties const& m_properties, SPtr<data::CAircraftData> data,
            s32 maxHeight);
    ~SbsFeed() noexcept override = default;

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::SBS
     */
    Protocol protocol() const override;

    /**
     * @brief Feed::process.
     */
    bool process(Str response) override;
};
}  // namespace vfrb::feed
