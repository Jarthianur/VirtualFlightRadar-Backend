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
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <list>
#include <utility>

#include "types.h"

namespace vfrb::profiling
{
enum class TimeVal : usize
{
    SECONDS = 1,
    MILLIS  = 1000,
    MICROS  = 1000000,
    NANOS   = 1000000000
};

class Profiler
{
    usize const                          m_testSize  = 0;
    clock_t                              m_tickCount = 0;
    std::function<void(Profiler&)> const m_fn;
    char const*                          m_descr = nullptr;

    Profiler(std::function<void(Profiler&)>&& fn, usize size, char const* descr)
        : m_testSize(size), m_fn(std::move(fn)), m_descr(descr)
    {}

    friend class ProfilerBuilder;

public:
    ~Profiler() noexcept = default;

    void run()
    {
        std::invoke(m_fn, *this);
    }

    template<typename FnT, typename... Args>
    void measure(FnT&& fn, Args&&... args)
    {
        auto t1 = clock();
        std::invoke(std::forward<FnT>(fn), std::forward<Args>(args)...);
        m_tickCount += clock() - t1;
    }

    template<typename FnT>
    void profile(FnT&& fn)
    {
        m_tickCount = 0;
        for (usize x = 0; x < m_testSize; ++x)
        {
            std::invoke(std::forward<FnT>(fn));
        }
    }

    f64 avgTime(TimeVal t) const
    {
        return ((static_cast<f64>(m_tickCount) / static_cast<f64>(m_testSize)) /
                static_cast<f64>(CLOCKS_PER_SEC)) *
               static_cast<f64>(t);
    }

    void print(TimeVal t) const
    {
        std::cout << m_descr << " - avg. duration: " << avgTime(t);
        switch (t)
        {
            case TimeVal::SECONDS: std::cout << " s"; break;
            case TimeVal::MILLIS: std::cout << " ms"; break;
            case TimeVal::MICROS: std::cout << " µs"; break;
            case TimeVal::NANOS: std::cout << " ns"; break;
        }
        std::cout << " - testsize: " << m_testSize << std::endl;
    }
};

class ProfilerBuilder
{
    usize                          m_testSize = 0;
    std::function<void(Profiler&)> m_fn;
    char const*                    m_descr = nullptr;

public:
    ProfilerBuilder()           = default;
    ~ProfilerBuilder() noexcept = default;

    static std::list<Profiler>& profilers()
    {
        static std::list<Profiler> profiler_list;
        return profiler_list;
    }

    ProfilerBuilder const& build()
    {
        profilers().push_back(Profiler(std::move(m_fn), m_testSize, m_descr));
        return *this;
    }

    ProfilerBuilder& withTestSize(usize size)
    {
        m_testSize = size;
        return *this;
    }

    ProfilerBuilder& withTestFn(std::function<void(Profiler&)>&& fn)
    {
        m_fn = std::move(fn);
        return *this;
    }

    ProfilerBuilder& withDescription(char const* descr)
    {
        m_descr = descr;
        return *this;
    }
};
}  // namespace vfrb::profiling
