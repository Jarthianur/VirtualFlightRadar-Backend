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

#pragma once

#include <istream>

#include "error/Error.hpp"

#include "Properties.h"
#include "types.h"

namespace vfrb::config
{
/**
 * @brief Read a config in INI format.
 */
class ConfigReader
{
    std::istream& m_stream;  ///< The input stream

public:
    explicit ConfigReader(std::istream& stream);  ///< @param stream The input stream to read from
    ~ConfigReader() noexcept = default;

    /**
     * @brief Read the given stream and return read properties.
     * @return the Properties
     */
    Properties read();
};

namespace error
{
class ReadFileError : public vfrb::error::Error
{
    str const m_fname;

public:
    explicit ReadFileError(str const& file);
    ~ReadFileError() noexcept override = default;

    char const* what() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::config
