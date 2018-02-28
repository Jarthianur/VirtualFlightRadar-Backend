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

#include "Logger.h"

#include <chrono>
#include <ctime>
#include <boost/chrono.hpp>
#include <boost/thread/lock_guard.hpp>

Logger::Logger()
{}

Logger::~Logger() noexcept
{}

boost::mutex Logger::mMutex;
std::ostream* Logger::mpOutStream = &std::cout;
std::ostream* Logger::mpErrStream = &std::cerr;

std::string Logger::getTime()
{
    std::time_t tt
        = boost::chrono::system_clock::to_time_t(boost::chrono::system_clock::now());
    std::string time(asctime(gmtime(&tt)));
    time.pop_back();
    return time;
}

void Logger::info(Message crMsg)
{
    boost::lock_guard<boost::mutex> lock(Logger::mMutex);
    *mpOutStream << "[INFO]  " << getTime() << ":: ";
    for(const auto& it : crMsg)
    {
        *mpOutStream << it;
    }
    *mpOutStream << std::endl;
}

// cppcheck-suppress unusedFunction
void Logger::debug(Message crMsg)
{
    boost::lock_guard<boost::mutex> lock(Logger::mMutex);
    *mpOutStream << "[DEBUG] " << getTime() << ":: ";
    for(const auto& it : crMsg)
    {
        *mpOutStream << it;
    }
    *mpOutStream << std::endl;
}

void Logger::warn(Message crMsg)
{
    boost::lock_guard<boost::mutex> lock(Logger::mMutex);
    *mpOutStream << "[WARN]  " << getTime() << ":: ";
    for(const auto& it : crMsg)
    {
        *mpOutStream << it;
    }
    *mpOutStream << std::endl;
}

void Logger::error(Message crMsg)
{
    boost::lock_guard<boost::mutex> lock(Logger::mMutex);
    *mpErrStream << "[ERROR] " << getTime() << ":: ";
    for(const auto& it : crMsg)
    {
        *mpErrStream << it;
    }
    *mpErrStream << std::endl;
}

void Logger::setLogFile(std::ostream* pOut)
{
    Logger::mpOutStream = pOut;
    Logger::mpErrStream = pOut;
}
