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

#include <cstdint>
#include <memory>
#include <string>

#include "config/Properties.h"
#include "util/defines.h"

#include "Feed.h"

namespace feed
{
namespace parser
{
class SbsParser;
}  // namespace parser
}  // namespace feed

namespace data
{
class AircraftData;
}  // namespace data

namespace feed
{
/**
 * @brief Extend Feed for SBS protocol.
 */
class SbsFeed : public Feed
{
public:
    NOT_COPYABLE(SbsFeed)
    DEFAULT_DTOR(SbsFeed)

    /**
     * @brief Constructor
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The AircraftData container
     * @param maxHeight  The max height filter
     * @throw std::logic_error from parent constructor
     */
    SbsFeed(const std::string& name, const config::Properties& properties,
            std::shared_ptr<data::AircraftData> data, std::int32_t maxHeight);

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::SBS
     */
    Protocol get_protocol() const override;

    /**
     * @brief Feed::process.
     */
    bool process(const std::string& response) override;

private:
    /// Parser to unpack response from Client
    static parser::SbsParser s_parser;
};

}  // namespace feed
