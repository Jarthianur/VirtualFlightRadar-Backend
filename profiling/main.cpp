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

#include "profiler.hpp"

using namespace vfrb::profiling;

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    std::for_each(ProfilerBuilder::profilers().begin(), ProfilerBuilder::profilers().end(), [](auto& p) {
        p.second.run();
        p.second.print(TimeVal::MICROS);
    });
    return 0;
}
