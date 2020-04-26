/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "CLogger.hpp"

#include <chrono>
#include <ctime>
#include <stdexcept>

namespace vfrb
{
void CLogger::LogFile(String const& file_) {
    concurrent::LockGuard lk(m_mutex);
    m_logFile = std::ofstream(file_);
    if (!m_logFile) {
        throw error::COpenLogfileError();
    }
    m_outStream = &m_logFile;
    m_errStream = &m_logFile;
}

String CLogger::time() const {
    std::time_t tt       = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char        time[32] = "";
    std::strftime(time, 32, "%c", gmtime(&tt));
    return time;
}

CLogger& CLogger::Instance() {
    static CLogger log;
    return log;
}

namespace error
{
str COpenLogfileError::Message() const noexcept {
    return "failed to open logfile";
}
}  // namespace error
}  // namespace vfrb
