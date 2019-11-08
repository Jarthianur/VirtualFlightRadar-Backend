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

#include "util/Logger.hpp"

#include <chrono>
#include <ctime>
#include <stdexcept>

namespace vfrb
{
void Logger::logFile(Str const& file)
{
    concurrent::LockGuard lk(m_mutex);
    m_logFile = std::ofstream(file);
    if (!m_logFile)
    {
        throw std::runtime_error("Could not open log file");
    }
    m_outStream = &m_logFile;
    m_errStream = &m_logFile;
}

Str Logger::time()
{
    std::time_t tt       = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    static char time[32] = "";
    std::strftime(time, 32, "%c", gmtime(&tt));
    return time;
}

Logger& Logger::Instance()
{
    static Logger log;
    return log;
}
}  // namespace vfrb
