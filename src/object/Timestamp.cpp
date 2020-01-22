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

#include "object/Timestamp.h"

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/spirit/include/qi.hpp>

namespace vfrb::object
{
namespace date_time
{
extern s64 Now();
extern u32 Day();
}  // namespace date_time

namespace error
{
char const* CTimestampParseError::Message() const noexcept
{
    return "";
}
}  // namespace error

std::tuple<u32, u32, u32, u32> parseTime(StrView::const_iterator&       first_,
                                         StrView::const_iterator const& last_)
{
    namespace qi = boost::spirit::qi;

    static thread_local const qi::int_parser<u32, 10, 2, 2> _2digit = {};
    static thread_local const qi::int_parser<u32, 10, 3, 3> _3digit = {};
    static thread_local const qi::rule<StrView::const_iterator, std::tuple<u32, u32, u32, u32>(),
                                       qi::space_type>
        time_r = (_2digit >> _2digit >> _2digit >> qi::attr(0)) |
                 (_2digit >> ":" >> _2digit >> ":" >> _2digit >> "." >> _3digit);

    std::tuple<u32, u32, u32, u32> time;
    if (qi::phrase_parse(first_, last_, time_r, qi::space, time) && (first_ == last_))
    {
        return time;
    }
    throw error::CTimestampParseError();
}

CTimestamp::CTimestamp(StrView const& sv_) : m_day(date_time::Day())
{
    auto it           = sv_.begin();
    auto [h, m, s, f] = parseTime(it, sv_.end());
    if (h > 23 || m > 59 || s > 59 || f > 999)
    {
        throw error::CTimestampParseError();
    }
    m_value = static_cast<s64>(h * 3600000 + m * 60000 + s * 1000 + f);
    if (m_value >= date_time::Now())
    {
        --m_day;
    }
}

CTimestamp::CTimestamp(CTimestamp const& other_) : m_value(other_.m_value), m_day(other_.m_day) {}

CTimestamp& CTimestamp::operator=(CTimestamp const& other_)
{
    this->m_value = other_.m_value;
    this->m_day   = other_.m_day;
    return *this;
}

bool CTimestamp::operator>(CTimestamp const& other_) const
{
    return (this->m_day > other_.m_day) || ((this->m_day == other_.m_day) && this->m_value > other_.m_value);
}
}  // namespace vfrb::object
