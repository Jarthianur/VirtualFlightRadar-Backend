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
#include "parser/CSbsParser.hpp"

#include "IFeed.hpp"

namespace vfrb::data
{
class CAircraftData;
}  // namespace vfrb::data

namespace vfrb::feed
{
class CSbsFeed : public IFeed
{
    // A queue that allows buffering position and movement updates until both are received.
    // Has an internal thread that iterates every item once a minute and erases all that are older than x via
    // timestamps.
    class CAssocQueue
    {
    public:
        struct SCombinedUpdate
        {
            enum EDataState : enum_type
            {
                HAS_NONE,
                HAS_POSITION,
                HAS_MOVEMENT
            };

            parser::SPositionUpdate PosUpdate;
            parser::SMovementUpdate MovUpdate;
            EDataState              DataState = HAS_NONE;

            inline auto
            ToAircraft(u32 prio_, object::CAircraft::IdString const& id_) const -> object::CAircraft;

            inline void
            Position(parser::SPositionUpdate const& pos_);

            inline void
            Movement(parser::SMovementUpdate const& mov_);

            template<typename T>
            static auto
            From(parser::SbsResult const& item_) -> SCombinedUpdate;
        };

    private:
        GUARDED_BY(m_mutex) HashMap<object::CAircraft::IdString, SCombinedUpdate> m_data;
        Shared<data::CAircraftData> m_targetData;
        concurrent::Mutex mutable m_mutex;
        GUARDED_BY(m_mutex) bool m_running;
        std::condition_variable_any m_cond;
        concurrent::CGuardedThread  m_thd;

        REQUIRES(!m_mutex) void clean();

    public:
        using Iterator = decltype(m_data)::iterator;

        explicit CAssocQueue(Shared<data::CAircraftData> data_);
        ~CAssocQueue() noexcept;

        REQUIRES(!m_mutex) void Push(parser::SbsResult const& item_, u32 prio_);
    } m_queue;
    parser::CSbsParser const          m_parser;
    concurrent::CWorkerThread<String> m_worker;

public:
    CSbsFeed(String const& name_, config::CProperties const& prop_, Shared<data::CAircraftData> data_,
             i32 maxHeight_);

    auto
    Protocol() const -> EProtocol override;

    auto
    Process(String&& str_) -> bool override;
};
}  // namespace vfrb::feed
