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

#include "concurrent/CWorkerThread.hpp"
#include "parser/CSbsParser.hpp"

#include "IFeed.hpp"

namespace vfrb::data
{
class CAircraftData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Extend Feed for SBS protocol.
 */
class CSbsFeed : public IFeed
{
    NOT_COPYABLE(CSbsFeed)

    parser::CSbsParser const          m_parser;  ///< Parser to unpack response from Client
    concurrent::CWorkerThread<String> m_worker;

public:
    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The AircraftData container
     * @param maxHeight  The max height filter
     * @throw std::logic_error from parent constructor
     */
    CSbsFeed(String const& name_, config::CProperties const& prop_, SPtr<data::CAircraftData> data_,
             s32 maxHeight_);
    ~CSbsFeed() noexcept override = default;

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::SBS
     */
    EProtocol Protocol() const override;

    /**
     * @brief Feed::process.
     */
    bool Process(String str_) override;
};
}  // namespace vfrb::feed
