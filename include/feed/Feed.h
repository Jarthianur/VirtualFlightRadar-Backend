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

#include "client/net/Endpoint.hpp"
#include "config/Properties.h"
#include "error/Error.hpp"
#include "util/class_utils.h"

#include "types.h"

namespace vfrb::data
{
class Data;
}  // namespace vfrb::data

namespace vfrb::feed
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
    u32 initPriority() const;

protected:
    config::Properties const m_properties;  ///< Properties
    Str const                m_name;        ///< Unique name
    u32 const                m_priority;    ///< Priority
    SPtr<data::Data>        m_data;        ///< Respective Data container

    /**
     * @param name       The Feeds unique name
     * @param component  The component string
     * @param properties The Properties
     * @throw std::logic_error if host or port are not given in properties
     */
    Feed(Str const& m_name, config::Properties const& propertyMap, SPtr<data::Data> data);

public:
    /**
     * @brief The protocol that the Feed supports.
     */
    enum class Protocol : enum_type
    {
        APRS,
        SBS,
        GPS,
        SENSOR
    };

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
    virtual bool process(Str response) = 0;

    auto name() const -> decltype(m_name) const&;
    auto priority() const -> std::remove_const<decltype(m_priority)>::type;
};

namespace error
{
class InvalidPropertyError : public vfrb::error::IError
{
    Str const m_msg;

public:
    explicit InvalidPropertyError(Str const& msg);
    ~InvalidPropertyError() noexcept override = default;

    char const* what() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::feed
