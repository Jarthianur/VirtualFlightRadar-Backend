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

#ifndef SRC_CONFIG_CONFIGREADER_H_
#define SRC_CONFIG_CONFIGREADER_H_

#include <boost/regex.hpp>
#include <string>
#include <unordered_map>

class ConfigReader
{
public:
    ConfigReader(const std::string& /*fname*/);
    virtual ~ConfigReader() noexcept;

    /**
     * Read given file.
     * Store recognized key/value properties into map.
     */
    void read();

    /**
     * Get value to given key from config map.
     * If key not found return given default.
     */
    const std::string& getProperty(const std::string& /*section*/,
                                   const std::string& /*key*/,
                                   const std::string& /*def_val*/= "") const;

private:
    const std::string mFile;
    /**
     * map:section
     *     ->map:key-value
     */
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mConfig;
    const boost::regex mConfRE;
};

#endif /* SRC_CONFIG_CONFIGREADER_H_ */
