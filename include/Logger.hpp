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

#include <array>
#include <fstream>
#include <iostream>
#include <utility>

#include "concurrent/Mutex.hpp"
#include "error/Error.hpp"
#include "util/class_utils.h"
#include "util/utility.hpp"

#include "types.h"

namespace vfrb
{
/// Application wide logger
class CLogger
{
    NOT_COPYABLE(CLogger)

    /// Log levels
    enum class Elevel : enum_type
    {
        INFO  = 0,
        WARN  = 1,
        ERROR = 2,
        DEBUG = 3
    };

    /**
     * Get the log line prefix.
     * @param stream_ The stream to log to
     * @param l_      The log level
     * @return an expression to log the prefix
     */
    constexpr auto prefix(std::ostream& stream_, Elevel l_) const
    {
        constexpr std::array<char const*, 4> levels = {"[INFO ]  ", "[WARN ]  ", "[ERROR]  ", "[DEBUG]  "};
        return [&, l = levels[util::AsUnderlyingType(l_)]] { stream_ << l << time() << ":: "; };
    }

    concurrent::Mutex mutable m_mutex;
    std::ofstream GUARDED_BY(m_mutex) m_logFile;                    ///< The logfile stream
    std::ostream* PT_GUARDED_BY(m_mutex) m_outStream = &std::cout;  ///< Stream to log INFO,DEBUG,WARN
    std::ostream* PT_GUARDED_BY(m_mutex) m_errStream = &std::cerr;  ///< Stream to log ERROR

    /**
     * Get current date and time.
     * @return the date-time
     */
    Str time() const;

    CLogger() = default;

public:
    ~CLogger() noexcept = default;

    /**
     * Get the logger instance.
     * @return the logger
     */
    static CLogger& Instance();

    /**
     * Log with INFO level.
     * @tparam args_ The arguments to log
     */
    template<typename... Args>
    void Info(Args&&... args_) const REQUIRES(!m_mutex);

    /**
     * Log with DEBUG level.
     * @note Requires LOG_ENABLE_DEBUG to be defined, else does nothing.
     * @tparam args_ The arguments to log
     */
    template<typename... Args>
    void Debug(Args&&... args_) const REQUIRES(!m_mutex);

    /**
     * Log with WARN level.
     * @tparam args_ The arguments to log
     */
    template<typename... Args>
    void Warn(Args&&... args_) const REQUIRES(!m_mutex);

    /**
     * Log with ERROR level.
     * @tparam args_ The arguments to log
     */
    template<typename... Args>
    void Error(Args&&... args_) const REQUIRES(!m_mutex);

    /**
     * Set a logfile instead of stdout/stderr.
     * @param file_ The filename
     */
    void LogFile(Str const& file_) REQUIRES(!m_mutex);
};

template<typename... Args>
void CLogger::Info(Args&&... args_) const REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    prefix(*m_outStream, Elevel::INFO)();
    (*m_outStream << ... << args_) << std::endl;
}

template<typename... Args>
void CLogger::Debug([[maybe_unused]] Args&&... args_) const REQUIRES(!m_mutex)
{
#ifdef LOG_ENABLE_DEBUG
    concurrent::LockGuard lk(m_mutex);
    prefix(*m_outStream, Elevel::DEBUG)();
    (*m_outStream << ... << args_) << std::endl;
#endif
}

template<typename... Args>
void CLogger::Warn(Args&&... args_) const REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    prefix(*m_outStream, Elevel::WARN)();
    (*m_outStream << ... << args_) << std::endl;
}

template<typename... Args>
void CLogger::Error(Args&&... args_) const REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    prefix(*m_errStream, Elevel::ERROR)();
    (*m_errStream << ... << args_) << std::endl;
}

namespace error
{
/// Error to indicate, that the logfile could not be opened.
class COpenLogfileError : public vfrb::error::IError
{
public:
    COpenLogfileError()                    = default;
    ~COpenLogfileError() noexcept override = default;

    char const* Message() const noexcept override;
};
}  // namespace error
}  // namespace vfrb
