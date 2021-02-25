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

#include "object/CTimestamp.hpp"

#include "boost/fusion/adapted/std_tuple.hpp"
#include "boost/spirit/include/qi.hpp"

namespace vfrb::object
{
namespace date_time
{
extern auto
Now() -> i64;
extern auto
Day() -> u32;
}  // namespace date_time

namespace error
{
auto
CTimestampParseError::Message() const noexcept -> str {
    return "";
}
}  // namespace error

auto
parseTime(StringView::const_iterator& first_, StringView::const_iterator const& last_)
    -> Tuple<u32, u32, u32, u32> {
    namespace qi = boost::spirit::qi;

    static thread_local const qi::int_parser<u32, 10, 2, 2> _2digit = {};
    static thread_local const qi::int_parser<u32, 10, 3, 3> _3digit = {};
    static thread_local const qi::rule<StringView::const_iterator, Tuple<u32, u32, u32, u32>(),
                                       qi::space_type>
        time_r = (_2digit >> _2digit >> _2digit >> qi::attr(0)) |
                 (_2digit >> ":" >> _2digit >> ":" >> _2digit >> "." >> _3digit);

    Tuple<u32, u32, u32, u32> time;
    if (qi::phrase_parse(first_, last_, time_r, qi::space, time) && (first_ == last_)) {
        return time;
    }
    throw error::CTimestampParseError();
}

CTimestamp::CTimestamp(StringView const& sv_) : m_day(date_time::Day()) {
    auto const* it    = sv_.begin();
    auto [h, m, s, f] = parseTime(it, sv_.end());
    if (h > 23 || m > 59 || s > 59 || f > 999) {
        throw error::CTimestampParseError();
    }
    m_value = static_cast<i64>(h * 3600000 + m * 60000 + s * 1000 + f);
    if (m_value >= date_time::Now()) {
        --m_day;
    }
}

auto
CTimestamp::operator>(CTimestamp const& other_) const -> bool {
    return (this->m_day > other_.m_day) || ((this->m_day == other_.m_day) && this->m_value > other_.m_value);
}

auto
CTimestamp::operator==(CTimestamp const& other_) const -> bool {
    return m_day == other_.m_day && m_value == other_.m_value;
}
}  // namespace vfrb::object
