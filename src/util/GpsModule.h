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

#ifndef SRC_UTIL_GPSMODULE_H_
#define SRC_UTIL_GPSMODULE_H_

#include <string>

namespace util
{

#define GPSM_BUFF_S 8191
#define GPSM_L_BUFF_S 128

/**
 * The GpsModule class.
 *
 * This class provides functionality to build NMEA GGA and RMC
 * sentences from given ExtGPSPositions.
 */
class GpsModule
{
public:
    /**
     * Constructor
     */
    GpsModule();
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~GpsModule() noexcept;
    /**
     * Build a GPGGA sentence from given GPS information.
     * The sentence contains trailing <cr><lf>.
     *
     * @param cr_pos the extended GPS information
     *
     * @return the GPGGA sentence
     */
    std::string genGpggaStr(const struct ExtGpsPosition& cr_pos);
    /**
     * Build a GPRMC sentence from given GPS information.
     * The sentence contains trailing <cr><lf>.
     *
     * @param cr_pos the extended GPS information
     *
     * @return the GPRMC sentence
     */
    std::string genGprmcStr(const struct ExtGpsPosition& cr_pos);

private:
    /// Formatstring buffer
    char mBuffer[GPSM_BUFF_S + 1];
};

}  // namespace util

#endif /* SRC_UTIL_GPSMODULE_H_ */
