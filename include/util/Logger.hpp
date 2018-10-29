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
 * @class Logger
 * @brief Provides static functions for threadsafe logging.
 */
class Logger
{
public:
    Logger();

    ~Logger() noexcept;

    /**
     * @fn info
     * @brief Log on INFO level to stdout.
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
     * @fn debug
     * @brief Log on DEBUG level to stdout.
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
     * @fn warn
     * @brief Log on WARN level to stdout.
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
     * @fn error
     * @brief Log on ERROR level to stderr.
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

    void set_debug(bool enable = true);

    void set_logFile(const std::string& file);

private:
    /// @var mMutex
    /// Mutex for threadsafe logging
    mutable std::mutex m_mutex;

    std::ofstream m_logFile;

    /// @var m_outStream
    /// Stream to log INFO,DEBUG,WARN
    std::ostream* m_outStream;

    /// @var mpErrStream
    /// Stream to log ERROR
    std::ostream* m_errStream;

    bool m_debugEnabled = false;

    /**
     * @fn getTime
     * @brief Get current date-time as string.
     * @return the date-time-string
     */
    static std::string get_time();

    /**
     * @fn log
     * @brief Write to the given stream.
     * @param pOut   The stream
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
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
};

extern Logger logger;
