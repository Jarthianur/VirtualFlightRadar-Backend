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
using vfrb::concurrent::MutableLock;
using vfrb::object::CTimestamp;
using vfrb::object::CAircraft;
using vfrb::feed::parser::SPositionUpdate;
using vfrb::feed::parser::SMovementUpdate;

namespace vfrb::feed
{
auto
CSbsFeed::SCombinedUpdate::ToAircraft(u32 prio_, object::CAircraft::IdString const& id_) const
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
CSbsFeed::SCombinedUpdate::Position(SPositionUpdate const& pos_) {
    PosUpdate = pos_;
    DataState |= HAS_POSITION;
}

void
CSbsFeed::SCombinedUpdate::Movement(SMovementUpdate const& mov_) {
    MovUpdate = mov_;
    DataState |= HAS_MOVEMENT;
}

template<>
auto
CSbsFeed::SCombinedUpdate::From<parser::SPositionUpdate>(parser::SbsResult const& item_) -> SCombinedUpdate {
    return {std::get<parser::SPositionUpdate>(item_.second), {}, SCombinedUpdate::HAS_POSITION};
}

template<>
auto
CSbsFeed::SCombinedUpdate::From<parser::SMovementUpdate>(parser::SbsResult const& item_) -> SCombinedUpdate {
    return {{}, std::get<parser::SMovementUpdate>(item_.second), SCombinedUpdate::HAS_MOVEMENT};
}

CSbsFeed::CSbsFeed(String const& name_, CProperties const& prop_, Shared<data::CAircraftData> data_,
                   i32 maxHeight_)
    : IFeed(name_, prop_, data_),
      m_queue(58U,
              [this] {
                  CTimestamp ts;
                  std::this_thread::sleep_for(std::chrono::seconds(2));  // let it age a bit
                  m_queue.RemoveIf([&ts](auto const& it_) {
                      return (it_->second.DataState == SCombinedUpdate::HAS_POSITION &&
                              ts > it_->second.PosUpdate.Timestamp) ||
                             (it_->second.DataState == SCombinedUpdate::HAS_MOVEMENT &&
                              ts > it_->second.MovUpdate.Timestamp) ||
                             it_->second.DataState == SCombinedUpdate::HAS_ALL;
                  });
              }),
      m_parser(maxHeight_),
      m_worker([this](String&& work_) {
          try {
              m_queue.Push(
                  m_parser.Parse(std::move(work_), 0),
                  [this](auto& it_, parser::SbsResult const& item_) {
                      if (std::holds_alternative<SPositionUpdate>(item_.second)) {
                          it_->second.Position(std::get<SPositionUpdate>(item_.second));
                      } else {
                          it_->second.Movement(std::get<SMovementUpdate>(item_.second));
                      }
                      if (it_->second.DataState == SCombinedUpdate::HAS_ALL &&
                          it_->second.MovUpdate.Timestamp.Diff(it_->second.PosUpdate.Timestamp) < 3000) {
                          m_data->Update(it_->second.ToAircraft(m_priority, item_.first));
                      }
                  },
                  [this](parser::SbsResult const& item_) {
                      return std::holds_alternative<SPositionUpdate>(item_.second) ?
                                 SCombinedUpdate::From<SPositionUpdate>(item_) :
                                 SCombinedUpdate::From<SMovementUpdate>(item_);
                  });
          } catch ([[maybe_unused]] parser::error::CParseError const&) {
          }
      }) {}

auto
CSbsFeed::Protocol() const -> IFeed::EProtocol {
    return EProtocol::SBS;
}

auto
CSbsFeed::Consume(String&& str_) -> bool {
    m_worker.Put(std::move(str_));
    return true;
}
}  // namespace vfrb::feed
