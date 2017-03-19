/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

class Logger
{
public:
    Logger();
    virtual ~Logger() noexcept;

    /**
     * Log informations, like succeeding connections etc
     * to stdout.
     * Given a message and a subject.
     */
    static void info(const std::string&, const std::string& = "");
    /**
     * Log debug informations to stdout.
     * Given a message and a subject.
     */
    static void debug(const std::string&, const std::string& = "");
    /**
     * Log warnings and not-critical errors, like lost connections etc
     * to stdout.
     * Given a message and a subject.
     */
    static void warn(const std::string&, const std::string& = "");
    /**
     * Log fatal errors, like socket failures etc
     * to stderr.
     * Given a message and a subject.
     */
    static void error(const std::string&, const std::string& = "");

private:
    static boost::mutex mMutex;

    static const std::string getTime();
};

#endif /* SRC_UTIL_LOGGER_H_ */
