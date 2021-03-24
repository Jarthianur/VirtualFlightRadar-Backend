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

#include "feed/CSbsFeed.hpp"

#include "config/CConfiguration.hpp"
#include "config/CProperties.hpp"
#include "data/CAircraftData.hpp"
#include "feed/parser/CSbsParser.hpp"
#include "object/CAircraft.hpp"

using vfrb::config::CProperties;
using vfrb::concurrent::ImmutableLock;
using vfrb::concurrent::MutableLock;
using vfrb::object::CTimestamp;
using vfrb::object::CAircraft;
using vfrb::feed::parser::SPositionUpdate;
using vfrb::feed::parser::SMovementUpdate;

namespace vfrb::feed
{
auto
CSbsFeed::CAssocQueue::SCombinedUpdate::ToAircraft(u32 prio_, object::CAircraft::IdString const& id_) const
    -> object::CAircraft {
    return {prio_,
            *id_,
            CAircraft::EIdType::ICAO,
            CAircraft::EAircraftType::POWERED_AIRCRAFT,
            CAircraft::ETargetType::TRANSPONDER,
            PosUpdate.Location,
            MovUpdate.Movement,
            PosUpdate.Timestamp};
}

void
CSbsFeed::CAssocQueue::SCombinedUpdate::Position(SPositionUpdate const& pos_) {
    PosUpdate = pos_;
    DataState = HAS_POSITION;
}

void
CSbsFeed::CAssocQueue::SCombinedUpdate::Movement(SMovementUpdate const& mov_) {
    MovUpdate = mov_;
    DataState = HAS_MOVEMENT;
}

template<>
auto
CSbsFeed::CAssocQueue::SCombinedUpdate::From<parser::SPositionUpdate>(parser::SbsResult const& item_)
    -> SCombinedUpdate {
    return {std::get<parser::SPositionUpdate>(item_.second), {}, SCombinedUpdate::HAS_POSITION};
}

template<>
auto
CSbsFeed::CAssocQueue::SCombinedUpdate::From<parser::SMovementUpdate>(parser::SbsResult const& item_)
    -> SCombinedUpdate {
    return {{}, std::get<parser::SMovementUpdate>(item_.second), SCombinedUpdate::HAS_MOVEMENT};
}

void
CSbsFeed::CAssocQueue::clean() {
    CTimestamp ts;
    std::this_thread::sleep_for(std::chrono::seconds(2));  // let it age a bit
    ImmutableLock lk(m_mutex);
    for (auto it = m_data.begin(); it != m_data.end();) {
        if ((it->second.DataState == SCombinedUpdate::HAS_POSITION && ts > it->second.PosUpdate.Timestamp) ||
            (it->second.DataState == SCombinedUpdate::HAS_MOVEMENT && ts > it->second.MovUpdate.Timestamp)) {
            it = m_data.erase(it);
        } else {
            ++it;
        }
    }
}

void
CSbsFeed::CAssocQueue::Push(parser::SbsResult const& item_, u32 prio_) {
    bool          holdsPos{std::holds_alternative<SPositionUpdate>(item_.second)};
    ImmutableLock lk(m_mutex);
    Iterator      it{m_data.find(item_.first)};
    if (it != m_data.end()) {
        if (holdsPos) {
            it->second.Position(std::get<SPositionUpdate>(item_.second));
            if (it->second.DataState == SCombinedUpdate::HAS_MOVEMENT) {
                m_targetData->Update(it->second.ToAircraft(prio_, item_.first));
                m_data.erase(it);
            }
        } else {
            it->second.Movement(std::get<SMovementUpdate>(item_.second));
            if (it->second.DataState == SCombinedUpdate::HAS_POSITION) {
                m_targetData->Update(it->second.ToAircraft(prio_, item_.first));
                m_data.erase(it);
            }
        }
    } else {
        if (holdsPos) {
            m_data.emplace(item_.first, SCombinedUpdate::From<SPositionUpdate>(item_));
        } else {
            m_data.emplace(item_.first, SCombinedUpdate::From<SMovementUpdate>(item_));
        }
    }
}

CSbsFeed::CAssocQueue::CAssocQueue(Shared<data::CAircraftData> data_)
    : m_targetData(data_), m_running(true), m_thd([this]() NO_THREAD_SAFETY_ANALYSIS {
          MutableLock lk(m_mutex);
          while (m_running) {
              m_cond.wait_for(lk, std::chrono::seconds(58),
                              [this]() NO_THREAD_SAFETY_ANALYSIS { return !m_running; });
              if (!m_running) {
                  break;
              }
              lk.unlock();
              clean();
              lk.lock();
          }
      }) {}

CSbsFeed::CAssocQueue::~CAssocQueue() noexcept {
    ImmutableLock lk(m_mutex);
    m_running = false;
    m_cond.notify_all();
}

CSbsFeed::CSbsFeed(String const& name_, CProperties const& prop_, Shared<data::CAircraftData> data_,
                   i32 maxHeight_)
    : IFeed(name_, prop_, data_), m_queue(data_), m_parser(maxHeight_), m_worker([this](String&& work_) {
          try {
              m_queue.Push(m_parser.Parse(std::move(work_), 0), m_priority);
          } catch ([[maybe_unused]] parser::error::CParseError const&) {
          }
      }) {}

auto
CSbsFeed::Protocol() const -> IFeed::EProtocol {
    return EProtocol::SBS;
}

auto
CSbsFeed::Process(String&& str_) -> bool {
    m_worker.Push(std::move(str_));
    return true;
}
}  // namespace vfrb::feed
