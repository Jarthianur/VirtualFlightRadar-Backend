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

#include "client/Endpoint.hpp"
#include "config/Properties.h"
#include "util/defines.h"

namespace data
{
class Data;
} /* namespace data */

namespace feed
{
/**
 * @brief Base class for input feeds.
 */
class Feed
{
public:
    NOT_COPYABLE(Feed)

    /**
     * @brief The protocol that the Feed supports.
     */
    enum class Protocol : std::uint8_t
    {
        APRS,
        SBS,
        GPS,
        SENSOR
    };

    /**
     * @brief Destructor
     */
    virtual ~Feed() noexcept = default;

    /**
     * @brief Get the supported Protocol.
     * @return the protocol
     */
    virtual Protocol get_protocol() const = 0;

    /**
     * @brief Get the feeds required Endpoint.
     * @return the endpoint
     */
    client::Endpoint get_endpoint() const;

    /**
     * @brief Handle client's response.
     * @param response The response
     */
    virtual bool process(const std::string& response) = 0;

protected:
    /**
     * @brief Constructor
     * @param name       The Feeds unique name
     * @param component  The component string
     * @param properties The Properties
     * @throw std::logic_error if host or port are not given in properties
     */
    Feed(const std::string& name, const char* component, const config::Properties& propertyMap,
         std::shared_ptr<data::Data> data);

    /// Unique name
    const std::string m_name;

    /// Component string
    const char* const m_component;

    /// Properties
    const config::Properties m_properties;

    /// Respective Data container
    std::shared_ptr<data::Data> m_data;

private:
    /**
     * @brief Initialize the priority from the given properties.
     */
    void initPriority() noexcept;

    /// Priority
    std::uint32_t m_priority;

public:
    /**
     * Getters
     */
    GETTER_CR(name)
    GETTER_V(priority)
};

}  // namespace feed
