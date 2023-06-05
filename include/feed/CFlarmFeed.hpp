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

#include "concurrent/CWorkerThread.hpp"
#include "parser/CFlarmParser.hpp"

#include "CHashQueue.hpp"
#include "IFeed.hpp"

namespace vfrb::data
{
class CAircraftData;
}  // namespace vfrb::data

namespace vfrb::feed
{
class CFlarmFeed : public IFeed
{
    struct SCombinedUpdate
    {
        CTCONST HAS_NONE     = 0b00U;
        CTCONST HAS_POSITION = 0b01U;
        CTCONST HAS_MOVEMENT = 0b10U;
        CTCONST HAS_ALL      = 0b11U;

        parser::SPositionUpdate PosUpdate;
        parser::SMovementUpdate MovUpdate;
        u32                     DataState = HAS_NONE;

        inline auto
        ToAircraft(u32 prio_, object::CAircraft::IdString const& id_) const -> object::CAircraft;

        inline void
        Position(parser::SPositionUpdate const& pos_);

        inline void
        Movement(parser::SMovementUpdate const& mov_);

        template<typename T>
        static auto
        From(parser::FlarmResult const& item_) -> SCombinedUpdate;
    };

    parser::CFlarmParser const                               m_parser;
    CHashQueue<object::CAircraft::IdString, SCombinedUpdate> m_queue;
    concurrent::CWorkerThread<String>                        m_worker;

public:
    CFlarmFeed(String const& name_, config::CProperties const& prop_, Shared<data::CAircraftData> data_,
               i32 maxHeight_);

    auto
    Protocol() const -> EProtocol override;

    auto
    Consume(String&& str_) -> bool override;

#ifdef VFRB_TEST_MODE
    void
    Await() const {
        while (!m_worker.Idle()) {
            std::this_thread::yield();
        }
    }
#endif
};
}  // namespace vfrb::feed
