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

#include <fstream>
#include <iostream>
#include <mutex>
#include <utility>

#include "util/defines.h"
#include "util/types.h"

#define PREFIX(S, P) *S << "\r" P "  " << time() << ":: "

namespace vfrb
{
/**
 * @brief Logger with different levels.
 */
class Logger
{
    NOT_COPYABLE(Logger)

    std::ofstream      m_logFile;                 ///< The logfile stream
    std::ostream*      m_outStream = &std::cout;  ///< Stream to log INFO,DEBUG,WARN
    std::ostream*      m_errStream = &std::cerr;  ///< Stream to log ERROR
    mutable std::mutex m_mutex;

    /**
     * @brief Get current date-time as string.
     * @return the date-time-string
     */
    static str time();

    Logger() = default;

public:
    ~Logger() noexcept = default;

    static Logger& instance();

    /**
     * @brief Log on INFO level.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename... Args>
    void info(Args&&... args) const
    {
        std::lock_guard lk(m_mutex);
        PREFIX(m_outStream, "[INFO ]");
        (*m_outStream << ... << args) << std::endl;
    }

    /**
     * @brief Log on DEBUG level.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename... Args>
    void debug([[maybe_unused]] Args&&... args) const
    {
#ifdef LOG_ENABLE_DEBUG
        std::lock_guard lk(m_mutex);
        PREFIX(m_outStream, "[DEBUG]");
        (*m_outStream << ... << args) << std::endl;
#endif
    }

    /**
     * @brief Log on WARN level.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename... Args>
    void warn(Args&&... args) const
    {
        std::lock_guard lk(m_mutex);
        PREFIX(m_outStream, "[WARN ]");
        (*m_outStream << ... << args) << std::endl;
    }

    /**
     * @brief Log on ERROR level.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename... Args>
    void error(Args&&... args) const
    {
        std::lock_guard lk(m_mutex);
        PREFIX(m_errStream, "[ERROR]");
        (*m_errStream << ... << args) << std::endl;
    }

    /**
     * @brief Set a logfile instead of stdout/stderr.
     * @param file The filename
     */
    void logFile(str const& file);
};
}  // namespace vfrb
#undef PREFIX
