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

#include <functional>
#include <mutex>
#include <thread>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "Defines.h"

using SignalHandler = std::function<void(const boost::system::error_code&, const int)>;

class Signals
{
public:
    NOT_COPYABLE(Signals)

    Signals();
    ~Signals() noexcept;

    void run();
    void stop();
    void addHandler(const SignalHandler& crHandler);

private:
    boost::asio::io_service mIoService;
    boost::asio::signal_set mSigSet;
    std::thread mThread;
    std::mutex mMutex;
};
