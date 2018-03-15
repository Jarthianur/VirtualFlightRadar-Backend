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
#include "Processor.hpp"

namespace data
{
namespace processor
{
/**
 * @class GpsModule
 * @brief Provides functionalities to build NMEA GGA and RMC sentences.
 */
class GpsProcessor : public Processor<object::GpsPosition>
{
public:
    /**
     * @fn GpsModule
     * @brief Constructor
     */
    GpsProcessor();

    /**
     * @fn ~GpsModule
     * @brief Destructor
     */
    virtual ~GpsProcessor() noexcept;

    std::string process(const object::GpsPosition& crPosition) override;

private:
    /**
     * @fn genGpggaStr
     * @brief Build a GPGGA sentence.
     * @note Contains trailing <cr><lf>.
     * @param cr_pos The extended GPS information
     * @return the GPGGA sentence
     */
    std::string genGpggaStr(const object::GpsPosition& crPosition);

    /**
     * @fn genGprmcStr
     * @brief Build a GPRMC sentence.
     * @note Contains trailing <cr><lf>.
     * @param cr_pos The extended GPS information
     * @return the GPRMC sentence
     */
    std::string genGprmcStr(const object::GpsPosition& crPosition);

    void evalPosition(double vLatitude, double vLongitude);

    char mtLatStr    = 'x';
    char mtLongStr   = 'x';
    double mtLatDeg  = 0.0;
    double mtLongDeg = 0.0;
    double mtLatMin  = 0.0;
    double mtLongMin = 0.0;
};

}  // namespace util
}
