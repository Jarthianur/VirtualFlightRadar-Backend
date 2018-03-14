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
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

/**
 * @class Logger
 * @brief Provides static functions for threadsafe logging.
 */
class Logger
{
public:
    /**
     * @fn Logger
     * @brief Constructor
     */
    Logger();

    /**
     * @fn ~Logger
     * @brief Destructor
     */
    virtual ~Logger() noexcept;

    /**
     * @fn info
     * @brief Log informations to stdout.
     * @param cr_subj The subject
     * @param cr_msg  The msg, default empty
     * @threadsafe
     */
    template<typename T, typename... TRest>
    static void info(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "[INFO]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void info(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "[INFO]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg));
    }

    /**
     * @fn debug
     * @brief Log debug informations to stdout.
     * @param cr_subj The subject
     * @param cr_msg  The msg, default empty
     * @threadsafe
     */
    template<typename T, typename... TRest>
    static void debug(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "[DEBUG] " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void debug(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "[DEBUG] " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg));
    }

    /**
     * @fn warn
     * @brief Log warnings to stdout.
     * @param cr_subj The subject
     * @param cr_msg  The msg, default empty
     * @threadsafe
     */
    template<typename T, typename... TRest>
    static void warn(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "[WARN]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void warn(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpOutStream << "[WARN]  " << getTime() << ":: ";
        log(mpOutStream, std::forward<T>(crMsg));
    }

    /**
     * @fn error
     * @brief Log fatal errors to stderr.
     * @param cr_subj The subject
     * @param cr_msg  The msg, default empty
     * @threadsafe
     */
    template<typename T, typename... TRest>
    static void error(T crMsg, TRest... tail)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpErrStream << "[ERROR] " << getTime() << ":: ";
        log(mpErrStream, std::forward<T>(crMsg), std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void error(T crMsg)
    {
        boost::lock_guard<boost::mutex> lock(Logger::mMutex);
        *mpErrStream << "[ERROR] " << getTime() << ":: ";
        log(mpErrStream, std::forward<T>(crMsg));
    }

    // static void setLogFile(std::ostream* pOut);

private:
    /// Mutex
    static boost::mutex mMutex;

    static std::ostream* mpOutStream;
    static std::ostream* mpErrStream;

    /**
     * @fn getTime
     * @brief Get current date-time as string.
     * @return the date-time-string
     */
    static std::string getTime();

    template<typename T, typename... TRest>
    static void log(std::ostream* pOut, T head, TRest... tail)
    {
        *pOut << std::forward<T>(head);
        log(pOut, std::forward<TRest>(tail)...);
    }
    template<typename T>
    static void log(std::ostream* pOut, T last)
    {
        *pOut << std::forward<T>(last) << std::endl;
    }
};
