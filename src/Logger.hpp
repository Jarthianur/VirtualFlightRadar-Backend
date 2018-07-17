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

#include <iostream>
#include <string>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

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
    void info(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        *mpOutStream << "\r[INFO]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    void info(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        *mpOutStream << "\r[INFO]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg));
    }

    /**
     * @fn debug
     * @brief Log on DEBUG level to stdout.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    void debug(T crMsg, TRest... tail)
    {
        if(mDebugEnabled)
        {
            boost::lock_guard<boost::mutex> lock(mMutex);
            *mpOutStream << "\r[DEBUG] " << getTime() << ":: ";
            log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
        }
    }
    template<typename T>
    void debug(T crMsg)
    {
        if(mDebugEnabled)
        {
            boost::lock_guard<boost::mutex> lock(mMutex);
            *mpOutStream << "\r[DEBUG] " << getTime() << ":: ";
            log(mpOutStream, std::forward<T>(crMsg));
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
    void warn(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        *mpOutStream << "\r[WARN]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    void warn(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        *mpOutStream << "\r[WARN]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg));
    }

    /**
     * @fn error
     * @brief Log on ERROR level to stderr.
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     * @threadsafe
     */
    template<typename T, typename... TRest>
    void error(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        *mpErrStream << "\r[ERROR] " << getTime() << ":: ";
        log(mpErrStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    void error(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        *mpErrStream << "\r[ERROR] " << getTime() << ":: ";
        log(mpErrStream, std::forward<T>(crMsg));
    }

    void setDebug(bool vEnable);

private:
    /// @var mMutex
    /// Mutex for threadsafe logging
    boost::mutex mMutex;

    /// @var mpOutStream
    /// Stream to log INFO,DEBUG,WARN
    std::ostream* mpOutStream;

    /// @var mpErrStream
    /// Stream to log ERROR
    std::ostream* mpErrStream;

    bool mDebugEnabled = false;

    /**
     * @fn getTime
     * @brief Get current date-time as string.
     * @return the date-time-string
     */
    static std::string getTime();

    /**
     * @fn log
     * @brief Write to the given stream.
     * @param pOut   The stream
     * @tparam T     The first argument
     * @tparam TRest The rest of arguments
     */
    template<typename T, typename... TRest>
    void log(std::ostream* pOut, T head, TRest... tail)
    {
        *pOut << std::forward<T>(head);
        log(pOut, std::forward<TRest>(tail)...);
    }
    template<typename T>
    void log(std::ostream* pOut, T last)
    {
        *pOut << std::forward<T>(last) << std::endl;
    }
};

extern Logger logger;
