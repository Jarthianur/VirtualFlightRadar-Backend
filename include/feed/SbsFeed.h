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

#include "concurrency/WorkerThread.hpp"

#include "Feed.h"

namespace vfrb::feed::parser
{
class SbsParser;
}  // namespace vfrb::feed::parser

namespace vfrb::data
{
class AircraftData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Extend Feed for SBS protocol.
 */
class SbsFeed : public Feed
{
    NOT_COPYABLE(SbsFeed)

    static parser::SbsParser      s_parser;  ///< Parser to unpack response from Client
    concurrent::WorkerThread<str> m_worker;

public:
    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The AircraftData container
     * @param maxHeight  The max height filter
     * @throw std::logic_error from parent constructor
     */
    SbsFeed(str const& m_name, config::Properties const& m_properties, s_ptr<data::AircraftData> data,
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
    bool process(str const& response) override;
};
}  // namespace vfrb::feed
