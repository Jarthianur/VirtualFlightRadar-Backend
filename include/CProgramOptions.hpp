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

#pragma once

#include <optional>

#include "error/IError.hpp"
#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb
{
namespace error
{
class CMissingArgumentError : public IError
{
    String const m_msg;

public:
    explicit CMissingArgumentError(String const& arg_);

    [[nodiscard]] auto
    Message() const noexcept -> str override;
};

class CMissingOptionError : public IError
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override;
};

class COptsCalledForHelp : public IError
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override;
};

class CUnknownOptionError : public IError
{
    String const m_msg;

public:
    explicit CUnknownOptionError(String const& arg_);

    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error

class CProgramOptions
{
public:
    CTCONST OPT_KEY_CONF = "CONF";
    CTCONST OPT_KEY_GNDM = "GNDM";
    CTCONST OPT_KEY_LOGF = "LOGF";

    void
    Parse(usize argc_, str* argv_);

    auto
    GetOpt(str key_) const -> std::optional<String>;

    auto
    RequireOpt(str key_) const -> std::optional<String>;

private:
    struct SMatched
    {};

    struct SOption
    {
        str m_flagLong;
        str m_flagShort;

        template<typename Fn>
        auto
        operator()(String const& arg_, Fn&& fn_) const -> decltype(*this)& {
            if (arg_ == m_flagLong || arg_ == m_flagShort) {
                fn_();
                throw SMatched{};
            }
            return *this;
        }
    };

    template<typename Fn>
    void
    evalArg(String const& arg_, Fn&& getvalFn_) {
        try {
            SOption{"--help", "-h"}(arg_, [&] { printHelp(); });
            SOption{"--config", "-c"}(arg_, [&] { m_opts.emplace(OPT_KEY_CONF, getvalFn_()); });
            SOption{"--ground", "-g"}(arg_, [&] { m_opts.emplace(OPT_KEY_GNDM, "true"); });
            SOption{"--log-file", "-l"}(arg_, [&] { m_opts.emplace(OPT_KEY_LOGF, getvalFn_()); });
        } catch ([[maybe_unused]] SMatched) {
            return;
        }
        throw error::CUnknownOptionError(arg_);
    }

    void
    printHelp();

    static auto
    tokenizeArgs(usize argc_, str* argv_) -> Vector<String>;

    HashMap<String, std::optional<String>> m_opts;
    str                                    m_progname{nullptr};
};
}  // namespace vfrb
