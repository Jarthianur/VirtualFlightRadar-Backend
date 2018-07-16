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

#include <boost/thread/lock_guard.hpp>

#include "Signals.h"

Signals::Signals() : mIoService(), mSigSet(mIoService)
{
    mSigSet.add(SIGINT);
    mSigSet.add(SIGTERM);
#ifdef SIGQUIT
    mSigSet.add(SIGQUIT);
#endif
}

Signals::~Signals() noexcept
{}

void Signals::run()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    mThread = boost::thread([this]() { mIoService.run(); });
}

void Signals::stop()
{
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        if(!mIoService.stopped())
        {
            mIoService.stop();
        }
    }
    if(mThread.joinable())
    {
        mThread.join();
    }
}

void Signals::addHandler(const SignalHandler& crHandler)
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    mSigSet.async_wait(crHandler);
}
