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

#ifndef SRC_UTIL_LOGGER_H_
#define SRC_UTIL_LOGGER_H_

#include <string>
#include <boost/thread/mutex.hpp>

namespace util
{

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
    static void info(const std::string& cr_subj, const std::string& cr_msg = "");
    /**
     * @fn debug
     * @brief Log debug informations to stdout.
     * @param cr_subj The subject
     * @param cr_msg  The msg, default empty
     * @threadsafe
     */
    static void debug(const std::string& cr_subj, const std::string& cr_msg = "");
    /**
     * @fn warn
     * @brief Log warnings to stdout.
     * @param cr_subj The subject
     * @param cr_msg  The msg, default empty
     * @threadsafe
     */
    static void warn(const std::string& cr_subj, const std::string& cr_msg = "");
    /**
     * @fn error
     * @brief Log fatal errors to stderr.
     * @param cr_subj The subject
     * @param cr_msg  The msg, default empty
     * @threadsafe
     */
    static void error(const std::string& cr_subj, const std::string& cr_msg = "");

private:
    /// Mutex
    static boost::mutex mMutex;

    /**
     * @fn getTime
     * @brief Get current date-time as string.
     * @return the date-time-string
     */
    static const std::string getTime();
};

}  // namespace util

#endif /* SRC_UTIL_LOGGER_H_ */
