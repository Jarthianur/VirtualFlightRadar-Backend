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

#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <boost/thread/lock_guard.hpp>

Logger::Logger()
{
}

Logger::~Logger()
{
}

boost::mutex Logger::mutex;

void Logger::info(const std::string& msg, const std::string& subject)
{
    boost::lock_guard<boost::mutex> lock(Logger::mutex);
    std::time_t tt = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
    std::string time(asctime(gmtime(&tt)));
    time.pop_back();
    std::cout << "[INFO]  " << time << ":: " << msg << subject << std::endl;
}

void Logger::debug(const std::string& msg, const std::string& subject)
{
    boost::lock_guard<boost::mutex> lock(Logger::mutex);
    std::time_t tt = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
    std::string time(asctime(gmtime(&tt)));
    time.pop_back();
    std::cout << "[DEBUG] " << time << ":: " << msg << subject << std::endl;
}

void Logger::warn(const std::string& msg, const std::string& subject)
{
    boost::lock_guard<boost::mutex> lock(Logger::mutex);
    std::time_t tt = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
    std::string time(asctime(gmtime(&tt)));
    time.pop_back();
    std::cout << "[WARN]  " << time << ":: " << msg << subject << std::endl;
}

void Logger::error(const std::string& msg, const std::string& subject)
{
    boost::lock_guard<boost::mutex> lock(Logger::mutex);
    std::time_t tt = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
    std::string time(asctime(gmtime(&tt)));
    time.pop_back();
    std::cerr << "[ERROR] " << time << ":: " << msg << subject << std::endl;
}
