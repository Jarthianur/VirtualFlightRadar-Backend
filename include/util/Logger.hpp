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
#include <string>
#include <utility>

/**
 * @brief Logger with different levels.
 */
class Logger
{
    std::ofstream      m_logFile;                    ///< The logfile stream
    std::ostream*      m_outStream    = &std::cout;  ///< Stream to log INFO,DEBUG,WARN
    std::ostream*      m_errStream    = &std::cerr;  ///< Stream to log ERROR
    bool               m_debugEnabled = false;       ///< Enabling state of debug level
    mutable std::mutex m_mutex;

    /**
     * @brief Get current date-time as string.
     * @return the date-time-string
     */
    static std::string get_time();

    /**
     * @brief Write to the given stream.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @param out The stream
     */
    template<typename T, typename... TRest>
    void log(std::ostream* out, T&& head, TRest&&... tail)
    {
        *out << std::forward<T>(head);
        log(out, std::forward<TRest>(tail)...);
    }
    template<typename T>
    void log(std::ostream* out, T&& last)
    {
        *out << std::forward<T>(last) << std::endl;
    }

public:
    Logger()           = default;
    ~Logger() noexcept = default;

    /**
     * @brief Log on INFO level.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    void info(T&& msg, TRest&&... tail)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *m_outStream << "\r[INFO]  " << get_time() << ":: ";
        log(m_outStream, std::forward<T>(msg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    void info(T&& msg)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *m_outStream << "\r[INFO]  " << get_time() << ":: ";
        log(m_outStream, std::forward<T>(msg));
    }

    /**
     * @brief Log on DEBUG level.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    void debug(T&& msg, TRest&&... tail)
    {
        if (m_debugEnabled)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            *m_outStream << "\r[DEBUG] " << get_time() << ":: ";
            log(m_outStream, std::forward<T>(msg), std::forward<TRest>(tail)...);
        }
    }
    template<typename T>
    void debug(T&& msg)
    {
        if (m_debugEnabled)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            *m_outStream << "\r[DEBUG] " << get_time() << ":: ";
            log(m_outStream, std::forward<T>(msg));
        }
    }

    /**
     * @brief Log on WARN level.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    void warn(T&& msg, TRest&&... tail)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *m_outStream << "\r[WARN]  " << get_time() << ":: ";
        log(m_outStream, std::forward<T>(msg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    void warn(T&& msg)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *m_outStream << "\r[WARN]  " << get_time() << ":: ";
        log(m_outStream, std::forward<T>(msg));
    }

    /**
     * @brief Log on ERROR level.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    void error(T&& msg, TRest&&... tail)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *m_errStream << "\r[ERROR] " << get_time() << ":: ";
        log(m_errStream, std::forward<T>(msg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    void error(T&& msg)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *m_errStream << "\r[ERROR] " << get_time() << ":: ";
        log(m_errStream, std::forward<T>(msg));
    }

    /**
     * @brief Enable/Disable debug level.
     * @param enable true to enable, false to disable (default: true)
     */
    void setDebug(bool enable = true);

    /**
     * @brief Set a logfile instead of stdout/stderr.
     * @param file The filename
     */
    void setLogFile(const std::string& file);
};

/// Extern Logger instance
extern Logger logger;
