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
extern s64 now();
extern u32 day();
}  // namespace date_time

namespace error
{
char const* TimestampParseError::what() const noexcept
{
    return "";
}
}  // namespace error

std::tuple<u32, u32, u32, u32> parseTime(std::string_view::const_iterator&       first,
                                         std::string_view::const_iterator const& last)
{
    namespace qi = boost::spirit::qi;

    static const qi::int_parser<u32, 10, 2, 2> _2digit = {};
    static const qi::int_parser<u32, 10, 3, 3> _3digit = {};
    static const qi::rule<std::string_view::const_iterator, std::tuple<u32, u32, u32, u32>(), qi::space_type>
        time_r = (_2digit >> _2digit >> _2digit >> qi::attr(0)) |
                 (_2digit >> ":" >> _2digit >> ":" >> _2digit >> "." >> _3digit);

    std::tuple<u32, u32, u32, u32> time;
    if (qi::phrase_parse(first, last, time_r, qi::space, time) && (first == last))
    {
        return time;
    }
    throw error::TimestampParseError();
}

Timestamp::Timestamp(std::string_view const& value) : m_day(date_time::day())
{
    auto it           = value.begin();
    auto [h, m, s, f] = parseTime(it, value.end());
    if (h > 23 || m > 59 || s > 59 || f > 999)
    {
        throw error::TimestampParseError();
    }
    m_value = static_cast<s64>(h * 3600000 + m * 60000 + s * 1000 + f);
    if (m_value >= date_time::now())
    {
        --m_day;
    }
}

Timestamp::Timestamp(Timestamp const& other) : m_value(other.m_value), m_day(other.m_day) {}

Timestamp& Timestamp::operator=(Timestamp const& other)
{
    this->m_value = other.m_value;
    this->m_day   = other.m_day;
    return *this;
}

bool Timestamp::operator>(Timestamp const& other) const
{
    return (this->m_day > other.m_day) || ((this->m_day == other.m_day) && this->m_value > other.m_value);
}
}  // namespace vfrb::object
