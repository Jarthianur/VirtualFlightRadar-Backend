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

#include <array>
#include <chrono>
#include <ctime>
#include <stdexcept>

#include "util/ClassUtils.hpp"

namespace vfrb
{
namespace
{
constexpr auto TIME_BUF_SIZE = 32;
}  // namespace

void CLogger::LogFile(String const& file_) {
    concurrent::LockGuard lk(m_mutex);
    m_logFile = std::ofstream(file_);
    if (!m_logFile) {
        throw error::COpenLogfileError();
    }
    m_outStream = &m_logFile;
    m_errStream = &m_logFile;
}

auto CLogger::time() -> String {
    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::array<char, TIME_BUF_SIZE> time{};
    std::strftime(time.data(), TIME_BUF_SIZE, "%c", gmtime(&tt));
    return time.data();
}

auto CLogger::Instance() noexcept -> CLogger& {
    static CLogger log;
    return log;
}

namespace error
{
auto COpenLogfileError::Message() const noexcept -> str {
    return "failed to open logfile";
}
}  // namespace error
}  // namespace vfrb