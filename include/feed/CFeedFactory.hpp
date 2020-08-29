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

#include "config/CProperties.hpp"
#include "error/IError.hpp"
#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::config
{
class CConfiguration;
}  // namespace vfrb::config

namespace vfrb::data
{
class CAircraftData;
class CAtmosphereData;
class CGpsData;
class CWindData;
}  // namespace vfrb::data

namespace vfrb::feed
{
class IFeed;

/**
 * @brief Factory for Feed creation.
 */
class CFeedFactory
{
    SPtr<config::CConfiguration> m_config;        ///< Pointer to the Configuration
    SPtr<data::CAircraftData>    m_aircraftData;  ///< Pointer to the AircraftData
    SPtr<data::CAtmosphereData>  m_atmosData;     ///< Pointer to the AtmosphereData
    SPtr<data::CGpsData>         m_gpsData;       ///< Pointer to the GpsData
    SPtr<data::CWindData>        m_windData;      ///< Pointer to the WindData

    /**
     * @brief Make a new Feed.
     * @tparam T The Feed type
     * @note Must be fully specialized for every concrete Feed type T.
     * @param name  The feed name
     * @return a pointer to the concrete Feed
     * @throw std::logic_error from invoked constructors
     */
    template<typename T, ENABLE_IF(EXTENDS(T, IFeed))>
    auto
    makeFeed(String const& name_) -> SPtr<T>;

public:
    /**
     * @param config       The Configuration
     * @param aircraftData The AircraftData pointer
     * @param atmosData    The AtmosphereData pointer
     * @param gpsData      The GpsData pointer
     * @param windData     The WindData pointer
     */
    CFeedFactory(SPtr<config::CConfiguration> config_, SPtr<data::CAircraftData> aircraftData_,
                 SPtr<data::CAtmosphereData> atmosData_, SPtr<data::CGpsData> gpsData_,
                 SPtr<data::CWindData> windData_);

    /**
     * @brief Create a Feed.
     * @param name  The feed name
     * @return an optional unique pointer to the feed
     * @throw std::logic_error from invoked methods
     */
    auto
    createFeed(String const& name_) -> SPtr<IFeed>;
};

namespace error
{
class CFeedCreationError : public vfrb::error::IError
{
    String const m_msg;

public:
    CFeedCreationError();

    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error
}  // namespace vfrb::feed
