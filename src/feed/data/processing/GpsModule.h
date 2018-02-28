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

#include <string>
#include "../object/Position.h"

#define GPSM_BUFF_S 8191
#define GPSM_L_BUFF_S 128

namespace feed
{
namespace data
{
namespace processing
{
/**
 * @class GpsModule
 * @brief Provides functionalities to build NMEA GGA and RMC sentences.
 */
class GpsModule
{
public:
    /**
     * @fn GpsModule
     * @brief Constructor
     */
    GpsModule();
    /**
     * @fn ~GpsModule
     * @brief Destructor
     */
    virtual ~GpsModule() noexcept;
    /**
     * @fn genGpggaStr
     * @brief Build a GPGGA sentence.
     * @note Contains trailing <cr><lf>.
     * @param cr_pos The extended GPS information
     * @return the GPGGA sentence
     */
    std::string genGpggaStr(const object::ExtGpsPosition& cr_pos);
    /**
     * @fn genGprmcStr
     * @brief Build a GPRMC sentence.
     * @note Contains trailing <cr><lf>.
     * @param cr_pos The extended GPS information
     * @return the GPRMC sentence
     */
    std::string genGprmcStr(const object::ExtGpsPosition& cr_pos);

private:
    /// Formatstring buffer
    char mBuffer[GPSM_BUFF_S + 1];
};

}  // namespace util
}
}
