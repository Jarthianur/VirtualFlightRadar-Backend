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

#include "ConfigReader.h"

#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace config
{
ConfigReader::ConfigReader(std::istream& stream) : m_stream(stream) {}

Properties ConfigReader::read()
{
    boost::property_tree::ptree tree;
    try
    {
        boost::property_tree::read_ini(m_stream, tree);
    }
    catch (const boost::property_tree::ini_parser_error& e)
    {
        throw std::invalid_argument(e.filename() + " is not a valid INI file");
    }
    return Properties(std::move(tree));
}

}  // namespace config
