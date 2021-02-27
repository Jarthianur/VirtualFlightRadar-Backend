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

#pragma once

#include <fstream>
#include <iostream>

#include "concurrent/Mutex.hpp"
#include "error/IError.hpp"

#include "Types.hpp"

namespace vfrb
{
class CLogger
{
    concurrent::Mutex mutable m_mutex;
    std::ofstream GUARDED_BY(m_mutex) m_logFile;
    std::ostream* PT_GUARDED_BY(m_mutex) m_outStream = &std::cout;
    std::ostream* PT_GUARDED_BY(m_mutex) m_errStream = &std::cerr;

    static auto
    time() -> String;

    CLogger() = default;

public:
    static auto
    Instance() noexcept -> CLogger&;

    template<typename... Args>
    void
    Info(Args&&... args_) const REQUIRES(!m_mutex);

    template<typename... Args>
    void
    Debug(Args&&... args_) const REQUIRES(!m_mutex);

    template<typename... Args>
    void
    Warn(Args&&... args_) const REQUIRES(!m_mutex);

    template<typename... Args>
    void
    Error(Args&&... args_) const REQUIRES(!m_mutex);

    void
    LogFile(String const& file_) REQUIRES(!m_mutex);
};

template<typename... Args>
void
CLogger::Info(Args&&... args_) const REQUIRES(!m_mutex) {
    concurrent::LockGuard lk(m_mutex);
    *m_outStream << "[INFO ]  " << time() << ":: ";
    (*m_outStream << ... << args_) << std::endl;
}

template<typename... Args>
void
CLogger::Debug([[maybe_unused]] Args&&... args_) const REQUIRES(!m_mutex) {
#ifdef LOG_ENABLE_DEBUG
    concurrent::LockGuard lk(m_mutex);
    *m_outStream << "[DEBUG]  " << time() << ":: ";
    (*m_outStream << ... << args_) << std::endl;
#endif
}

template<typename... Args>
void
CLogger::Warn(Args&&... args_) const REQUIRES(!m_mutex) {
    concurrent::LockGuard lk(m_mutex);
    *m_outStream << "[WARN ]  " << time() << ":: ";
    (*m_outStream << ... << args_) << std::endl;
}

template<typename... Args>
void
CLogger::Error(Args&&... args_) const REQUIRES(!m_mutex) {
    concurrent::LockGuard lk(m_mutex);
    *m_errStream << "[ERROR]  " << time() << ":: ";
    (*m_errStream << ... << args_) << std::endl;
}

namespace error
{
class COpenLogfileError : public vfrb::error::IError
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error
}  // namespace vfrb
