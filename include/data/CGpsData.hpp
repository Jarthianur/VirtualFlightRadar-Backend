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

#include "concurrent/Mutex.hpp"
#include "error/IError.hpp"
#include "object/CGpsPosition.hpp"
#include "processor/CGpsProcessor.hpp"

#include "IData.hpp"
#include "Types.hpp"

namespace vfrb::data
{
class CGpsData : public IData
{
    CTCONST GPS_NR_SATS_GOOD      = 7;
    CTCONST GPS_FIX_GOOD          = 1;
    CTCONST GPS_HOR_DILUTION_GOOD = 2.0;

    concurrent::Mutex mutable m_mutex;
    object::CGpsPosition     GUARDED_BY(m_mutex) m_position;
    processor::CGpsProcessor GUARDED_BY(m_mutex) m_processor;
    bool                     GUARDED_BY(m_mutex) m_positionLocked = false;
    bool                     GUARDED_BY(m_mutex) m_groundMode     = false;

    auto
    isPositionGood() const -> bool REQUIRES(m_mutex);

public:
    CGpsData(object::CGpsPosition const& pos_, bool gnd_);

    auto
    Update(object::CObject&& pos_) -> bool override REQUIRES(!m_mutex);

    void
    CollectInto(str_util::StringInserter si_) override REQUIRES(!m_mutex);

    auto
    Location() const -> decltype(m_position.Location()) REQUIRES(!m_mutex);

    auto
    Size() const -> usize override REQUIRES(!m_mutex);
};

namespace error
{
class IGpsDataException : public vfrb::error::IError
{};

class CPositionAlreadyLocked : public IGpsDataException
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override;
};

class CReceivedGoodPosition : public IGpsDataException
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error
}  // namespace vfrb::data
