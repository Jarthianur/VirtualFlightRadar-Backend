/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include "ParserAPRS.h"

#include <stdexcept>

#include "AircraftContainer.h"
#include "Configuration.h"
#include "Logger.h"
#include "Math.h"

ParserAPRS::ParserAPRS()
        : Parser(),
          aprs_re("^(?:\\S+)>APRS,\\S+(?:,\\S+)?:/(\\d{6})h(\\d{4}\\.\\d{2})([NS])[^]+?(\\d{5}\\.\\d{2})([EW])[^]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+([^]+?)$",
                  std::regex_constants::optimize),
          comm_re("^[^]*id(\\S{2})(\\S{6})\\s+(?:([\\+-]\\d+)fpm\\s+)?(?:([\\+-]\\d+\\.\\d+)rot)?[^]*$",
                  std::regex_constants::optimize)
{
}

ParserAPRS::~ParserAPRS()
{
}

int ParserAPRS::unpack(const std::string& sentence, AircraftContainer& ac_cont)
{
    if (sentence.at(0) == '#')
    {
        return MSG_UNPACK_IGN;
    }
    try
    {
        std::smatch match;
        if (std::regex_match(sentence, match, aprs_re))
        {

            try
            {
                //time
                time = std::stoi(match.str(1));

                //latitude
                lat = Math::dmsToDeg(std::stod(match.str(2)) / 100.0);
                if (match.str(3).compare("S") == 0)
                {
                    lat = -lat;
                }

                //longitue
                lon = Math::dmsToDeg(std::stod(match.str(4)) / 100.0);
                if (match.str(5).compare("W") == 0)
                {
                    lon = -lon;
                }

                //altitude
                alt = Math::dToI(std::stod(match.str(8)) * Math::feet2m);
                if (alt > Configuration::filter_maxHeight)
                {
                    return MSG_UNPACK_IGN;
                }
            }
            catch (const std::logic_error& e)
            {
                return MSG_UNPACK_ERR;
            }

            //comment
            // climbrate / address / id / type
            if (match.str(9).size() > 0)
            {
                std::smatch comm_match;
                if (std::regex_match(match.str(9), comm_match, comm_re))
                {
                    id = comm_match.str(2);
                    id_t = std::stoi(comm_match.str(1), nullptr, 16) & 0x03;
                    ac_t = (std::stoi(comm_match.str(1), nullptr, 16) & 0x7C) >> 2;
                    try
                    {
                        climb_r = std::stod(comm_match.str(3)) * Math::fpm2ms;
                    }
                    catch (const std::logic_error& e)
                    {
                        climb_r = VALUE_NA;
                    }
                    try
                    {
                        turn_r = std::stod(comm_match.str(4)) * 3.0; // 1rot = 1 halfcircle / 1 min => 3° / 1s
                    }
                    catch (const std::logic_error& e)
                    {
                        turn_r = VALUE_NA;
                    }
                }
                else
                {
                    return MSG_UNPACK_ERR;
                }
            }
            else
            {
                return MSG_UNPACK_ERR;
            }

            //track/gnd_speed
            try
            {
                heading = std::stod(match.str(6));
            }
            catch (const std::logic_error& e)
            {
                heading = VALUE_NA;
            }
            try
            {
                gnd_spd = std::stod(match.str(7)) * Math::kts2ms;
            }
            catch (const std::logic_error& e)
            {
                gnd_spd = VALUE_NA;
            }

            ac_cont.insertAircraft(id, lat, lon, alt, gnd_spd, id_t, ac_t, climb_r,
                                   turn_r, heading);
        }
        else
        {
            return MSG_UNPACK_ERR;
        }
    }
    catch (std::regex_error& e)
    {
        Logger::error("while parsing APRS regex: ", e.what());
        return APRS_REGEX_ERR;
    }
    return MSG_UNPACK_SUC;
}
