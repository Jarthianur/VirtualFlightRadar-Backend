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

#include <memory>

#include "client/net/Endpoint.hpp"
#include "config/Properties.h"
#include "util/defines.h"
#include "util/types.h"

namespace data
{
class Data;
}  // namespace data

namespace feed
{
/**
 * @brief Base class for input feeds.
 */
class Feed
{
    NOT_COPYABLE(Feed)

    /**
     * @brief Initialize the priority from the given properties.
     */
    u32 initPriority() const noexcept;

protected:
    std::shared_ptr<data::Data> m_data;       ///< Respective Data container
    u32 const                   m_priority;   ///< Priority
    char const* const           m_logPrefix;  ///< Component string

    /**
     * @param name       The Feeds unique name
     * @param component  The component string
     * @param properties The Properties
     * @throw std::logic_error if host or port are not given in properties
     */
    Feed(str const& name, char const* logPrefix, config::Properties const& propertyMap,
         std::shared_ptr<data::Data> data);

public:
    /**
     * @brief The protocol that the Feed supports.
     */
    enum class Protocol : enum_t
    {
        APRS,
        SBS,
        GPS,
        SENSOR
    };

    str const                name;        ///< Unique name
    config::Properties const properties;  ///< Properties

    virtual ~Feed() noexcept = default;

    /**
     * @brief Get the supported Protocol.
     * @return the protocol
     */
    virtual Protocol protocol() const = 0;

    /**
     * @brief Get the feeds required Endpoint.
     * @return the endpoint
     */
    client::net::Endpoint endpoint() const;

    /**
     * @brief Handle client's response.
     * @param response The response
     */
    virtual bool process(str const& response) = 0;
};
}  // namespace feed
