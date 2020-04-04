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

#include "client/net/SEndpoint.hpp"
#include "config/CProperties.hpp"
#include "error/IError.hpp"
#include "util/class_utils.hpp"

#include "types.hpp"

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

    /**
     * @brief Initialize the priority from the given properties.
     */
    u32 initPriority() const;

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
    virtual EProtocol Protocol() const = 0;

    /**
     * @brief Get the feeds required Endpoint.
     * @return the endpoint
     */
    client::net::SEndpoint Endpoint() const;

    /**
     * @brief Handle client's response.
     * @param response The response
     */
    virtual bool Process(String str_) = 0;

    auto Priority() const -> std::remove_const<decltype(m_priority)>::type;

    GETTER_REF(Name, m_name)
};

namespace error
{
class CInvalidPropertyError : public vfrb::error::IError
{
    String const m_msg;

public:
    explicit CInvalidPropertyError(String const& msg_);

    str Message() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::feed
