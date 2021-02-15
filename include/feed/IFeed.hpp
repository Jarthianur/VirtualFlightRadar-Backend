/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "client/net/SEndpoint.hpp"
#include "config/CProperties.hpp"
#include "error/IError.hpp"
#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::data
{
class IData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Base class for input feeds.
 */
class IFeed
{
    NOT_COPYABLE(IFeed)
    NOT_MOVABLE(IFeed)

    /**
     * @brief Initialize the priority from the given properties.
     */
    [[nodiscard]] auto initPriority() const -> u32;

protected:
    config::CProperties const m_properties;  ///< Properties
    String const              m_name;        ///< Unique name
    u32 const                 m_priority;    ///< Priority
    SPtr<data::IData>         m_data;        ///< Respective Data container

    /**
     * @param name       The Feeds unique name
     * @param component  The component string
     * @param properties The Properties
     * @throw std::logic_error if host or port are not given in properties
     */
    IFeed(String const& name_, config::CProperties const& prop_, SPtr<data::IData> data_);

public:
    /**
     * @brief The protocol that the Feed supports.
     */
    enum class EProtocol : enum_type
    {
        APRS,
        SBS,
        GPS,
        SENSOR
    };

    virtual ~IFeed() noexcept = default;

    /**
     * @brief Get the supported Protocol.
     * @return the protocol
     */
    [[nodiscard]] virtual auto
    Protocol() const -> EProtocol = 0;

    /**
     * @brief Get the feeds required Endpoint.
     * @return the endpoint
     */
    [[nodiscard]] auto
    Endpoint() const -> client::net::SEndpoint;

    /**
     * @brief Handle client's response.
     * @param response The response
     */
    virtual auto
    Process(String str_) -> bool = 0;

    [[nodiscard]] auto
    Priority() const -> std::remove_const<decltype(m_priority)>::type;

    GETTER_REF(Name, m_name)
};

namespace error
{
class CInvalidPropertyError : public vfrb::error::IError
{
    String const m_msg;

public:
    explicit CInvalidPropertyError(String const& msg_);

    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error
}  // namespace vfrb::feed
