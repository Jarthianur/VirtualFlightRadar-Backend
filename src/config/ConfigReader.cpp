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

#include "config/ConfigReader.h"

#include <utility>

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace boost::property_tree;

namespace vfrb::config
{
ConfigReader::ConfigReader(std::istream& stream) : m_stream(stream) {}

Properties ConfigReader::read()
{
    ptree tree;
    try
    {
        read_ini(m_stream, tree);
    }
    catch (ini_parser_error const& e)
    {
        throw error::ReadFileError(e.filename());
    }
    return Properties(std::move(tree));
}

namespace error
{
ReadFileError::ReadFileError(Str const& file) : m_fname(file + " is not a valid INI file") {}

char const* ReadFileError::what() const noexcept
{
    return m_fname.c_str();
}
}  // namespace error
}  // namespace vfrb::config
