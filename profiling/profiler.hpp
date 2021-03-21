/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <utility>

#include "Types.hpp"

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
        : m_testSize(size), m_fn(std::move(fn)), m_descr(descr) {}

    friend class ProfilerBuilder;

public:
    void
    run() {
        std::invoke(m_fn, *this);
    }

    template<typename FnT, typename... Args>
    void
    measure(FnT&& fn, Args&&... args) {
        auto t1 = clock();
        std::invoke(std::forward<FnT>(fn), std::forward<Args>(args)...);
        m_tickCount += clock() - t1;
    }

    template<typename FnT>
    void
    profile(FnT&& fn) {
        for (usize x = 0; x < std::max<usize>(1, m_testSize / 2); ++x) {
            std::invoke(std::forward<FnT>(fn));
        }
        m_tickCount = 0;
        for (usize x = 0; x < m_testSize; ++x) {
            std::invoke(std::forward<FnT>(fn));
        }
    }

    f64
    avgTime(TimeVal t) const {
        return ((static_cast<f64>(m_tickCount) / static_cast<f64>(m_testSize)) /
                static_cast<f64>(CLOCKS_PER_SEC)) *
               static_cast<f64>(t);
    }

    void
    print(TimeVal t) const {
        std::cout << m_descr << " - avg. duration: " << std::fixed << std::setprecision(6) << avgTime(t);
        switch (t) {
            case TimeVal::SECONDS: std::cout << " s"; break;
            case TimeVal::MILLIS: std::cout << " ms"; break;
            case TimeVal::MICROS: std::cout << " µs"; break;
            case TimeVal::NANOS: std::cout << " ns"; break;
        }
        std::cout << " - testsize: " << m_testSize << std::endl;
    }
};

class ProfilerGroup
{
    List<Profiler> m_profilers;
    char const*    m_name;

    friend class ProfilerBuilder;

public:
    explicit ProfilerGroup(char const* name) : m_name(name) {}

    void
    run() {
        for (auto& p : m_profilers) {
            p.run();
        }
    }

    void
    print(TimeVal t) const {
        std::cout << m_name << ": " << std::endl;
        for (auto const& p : m_profilers) {
            p.print(t);
        }
        std::cout << std::endl;
    }
};

class ProfilerBuilder
{
    usize                          m_testSize = 0;
    std::function<void(Profiler&)> m_fn;
    char const*                    m_descr = nullptr;
    char const*                    m_group = nullptr;

public:
    ProfilerBuilder() = default;

    static HashMap<char const*, ProfilerGroup>&
    profilers() {
        static HashMap<char const*, ProfilerGroup> profiler_list;
        return profiler_list;
    }

    ProfilerBuilder const&
    build() {
        profilers()
            .emplace(m_group, ProfilerGroup(m_group))
            .first->second.m_profilers.push_back(Profiler(std::move(m_fn), m_testSize, m_descr));
        return *this;
    }

    ProfilerBuilder&
    withTestSize(usize size) {
        m_testSize = size;
        return *this;
    }

    ProfilerBuilder&
    withTestFn(std::function<void(Profiler&)>&& fn) {
        m_fn = std::move(fn);
        return *this;
    }

    ProfilerBuilder&
    withDescription(char const* descr) {
        m_descr = descr;
        return *this;
    }

    ProfilerBuilder&
    withGroup(char const* group) {
        m_group = group;
        return *this;
    }
};
}  // namespace vfrb::profiling
