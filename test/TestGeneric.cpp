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

#include "CProgramOptions.hpp"
#include "Helper.hpp"

using namespace vfrb;

DESCRIBE_PAR("test_CProgramOptions") {
    IT("should signal help called") {
        CProgramOptions uut;
        {
            str testArgs[]{"prog", "-h"};
            ASSERT_THROWS(uut.Parse(2, testArgs), error::COptsCalledForHelp);
        }
        {
            str testArgs[]{"prog", "--help"};
            ASSERT_THROWS(uut.Parse(2, testArgs), error::COptsCalledForHelp);
        }
    }
    IT("should signal help called amongst others") {
        CProgramOptions uut;
        {
            str testArgs[]{"prog", "-g", "-c", "test.conf", "-h"};
            ASSERT_THROWS(uut.Parse(5, testArgs), error::COptsCalledForHelp);
        }
        {
            str testArgs[]{"prog", "-g", "--help", "-c", "test.conf"};
            ASSERT_THROWS(uut.Parse(5, testArgs), error::COptsCalledForHelp);
        }
    }

    IT("should throw if argument is missing") {
        CProgramOptions uut;
        {
            str testArgs[]{"prog", "-c"};
            ASSERT_THROWS(uut.Parse(2, testArgs), error::CMissingArgumentError);
        }
        {
            str testArgs[]{"prog", "-l"};
            ASSERT_THROWS(uut.Parse(2, testArgs), error::CMissingArgumentError);
        }
    }
    IT("should throw if unknown option is given") {
        CProgramOptions uut;
        str             testArgs[]{"prog", "--not-existing"};
        ASSERT_THROWS(uut.Parse(2, testArgs), error::CUnknownOptionError);
    }

    IT("should get options") {
        CProgramOptions uut;
        {
            str testArgs[]{"prog", "-c", "test.conf"};
            ASSERT_NOTHROW(uut.Parse(3, testArgs));
            auto opt = uut.GetOpt(CProgramOptions::OPT_KEY_CONF);
            ASSERT_NOT(opt, EQ, std::nullopt);
            ASSERT(*opt, EQ, "test.conf");
        }
        {
            str testArgs[]{"prog", "-g"};
            ASSERT_NOTHROW(uut.Parse(2, testArgs));
            auto opt = uut.GetOpt(CProgramOptions::OPT_KEY_GNDM);
            ASSERT_NOT(opt, EQ, std::nullopt);
            ASSERT(*opt, EQ, "true");
        }
        {
            str testArgs[]{"prog", "-g"};
            ASSERT_NOTHROW(uut.Parse(2, testArgs));
            auto opt = uut.GetOpt(CProgramOptions::OPT_KEY_CONF);
            ASSERT(opt, EQ, std::nullopt);
        }
    }
    IT("should throw if required option is missing") {
        CProgramOptions uut;
        str             testArgs[]{"prog"};
        ASSERT_NOTHROW(uut.Parse(1, testArgs));
        ASSERT_THROWS(uut.RequireOpt(CProgramOptions::OPT_KEY_CONF), error::CMissingOptionError);
    }
    IT("should not throw if required option is given") {
        CProgramOptions uut;
        str             testArgs[]{"prog", "-c", "test.conf"};
        ASSERT_NOTHROW(uut.Parse(3, testArgs));
        auto opt = ASSERT_NOTHROW(return uut.RequireOpt(CProgramOptions::OPT_KEY_CONF));
        ASSERT_NOT(opt, EQ, std::nullopt);
        ASSERT(*opt, EQ, "test.conf");
    };
};
