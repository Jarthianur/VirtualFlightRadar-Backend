/*
    Copyright (C) 2017  Jarthianur

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef TPP_RELEASE_TPP_HPP
#define TPP_RELEASE_TPP_HPP
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#ifndef TPP_VERSION_HPP
#    define TPP_VERSION_HPP
#    define TPP_VERSION_MAJOR "3"
#    define TPP_VERSION_MINOR "0"
#    define TPP_VERSION_REVISION "rc0"
#    define TPP_VERSION TPP_VERSION_MAJOR "." TPP_VERSION_MINOR "-" TPP_VERSION_REVISION
#endif
#ifndef TPP_CPP_META_HPP
#    define TPP_CPP_META_HPP
#    if __cplusplus >= 201703L
#        define TPP_INTERN_CPP_V17
#    elif __cplusplus >= 201402L
#        define TPP_INTERN_CPP_V14
#    elif __cplusplus >= 201103L
#        define TPP_INTERN_CPP_V11
#    else
#        error Test++ requires at least full C++11 compliance
#    endif
#    if defined(__GNUG__) || defined(__clang__)
#        define TPP_INTERN_SYS_UNIX
#    elif defined(_WIN32)
#        define TPP_INTERN_SYS_WIN
#    else
#        error Test++ is only supported for Linux (gcc), OSX (clang), and Windows (msvc)
#    endif
#    if __cpp_transactional_memory >= 201505
#        define TPP_INTERN_SYNC synchronized
#    else
#        define TPP_INTERN_SYNC
#    endif
#endif
#ifndef TPP_TRAITS_HPP
#    define TPP_TRAITS_HPP
#    define TPP_INTERN_ENABLE_IF(C) typename std::enable_if<C>::type* = nullptr
#    define TPP_INTERN_IS(T, ...) (T<__VA_ARGS__>::value)
#    define TPP_INTERN_IS_TYPE(T, R) (TPP_INTERN_IS(std::is_same, T, R))
#    define TPP_INTERN_IS_FLOAT(T) (TPP_INTERN_IS(std::is_floating_point, T))
#    define TPP_INTERN_IS_VOID(T) (TPP_INTERN_IS(std::is_void, T))
#    define TPP_INTERN_HAS_STREAM_CAPABILITY(T, S) (TPP_INTERN_IS(tpp::intern::stream_capability, S, T))
#    define TPP_INTERN_HAS_ITERATOR_CAPABILITY(T) (TPP_INTERN_IS(tpp::intern::iterator_capability, T))
namespace tpp
{
namespace intern
{
template<typename S, typename T>
class stream_capability
{
    template<typename SS, typename TT>
    static auto
    test(int) -> decltype(std::declval<SS&>() << std::declval<TT>(), std::true_type());
    template<typename, typename>
    static auto
    test(...) -> std::false_type;

public:
    static const bool value = decltype(test<S, T>(0))::value;
};
template<typename T>
class iterator_capability
{
    template<typename TT>
    static auto
    test(int) -> decltype(std::begin(std::declval<TT&>()) != std::end(std::declval<TT&>()), void(),
                          ++std::declval<decltype(std::begin(std::declval<TT&>()))&>(),
                          void(*std::begin(std::declval<TT&>())), std::true_type());
    template<typename>
    static auto
    test(...) -> std::false_type;

public:
    static const bool value = decltype(test<T>(0))::value;
};
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_DURATION_HPP
#    define TPP_DURATION_HPP
namespace tpp
{
namespace intern
{
class duration final
{
public:
    duration() : m_start(std::chrono::steady_clock::now()) {}
    auto
    get() -> double {
        return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_start).count();
    }

private:
    std::chrono::steady_clock::time_point const m_start;
};
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_REGEX_HPP
#    define TPP_REGEX_HPP
namespace tpp
{
namespace intern
{
struct regex final
{
    regex(char const* p_, std::regex_constants::syntax_option_type flags_) : pattern(p_), re(p_, flags_) {}
    operator std::regex() const {
        return re;
    }
    char const* const pattern;
    std::regex const  re;
};
}  // namespace intern
using regex = intern::regex;
inline auto operator"" _re(char const* lit_, std::size_t) -> regex {
    return regex(lit_, std::regex::ECMAScript);
}
inline auto operator"" _re_i(char const* lit_, std::size_t) -> regex {
    return regex(lit_, std::regex::ECMAScript | std::regex::icase);
}
}  // namespace tpp
#endif
#ifndef TPP_STRINGIFY_HPP
#    define TPP_STRINGIFY_HPP
#    ifdef TPP_INTERN_SYS_UNIX
#        include <cxxabi.h>
#    endif
namespace tpp
{
namespace intern
{
template<typename T>
static auto
name_for_type(T const& arg_) -> std::string const& {
    static thread_local std::string name;
    if (!name.empty()) {
        return name;
    }
#    ifdef TPP_INTERN_SYS_UNIX
    int                     status{-1};
    std::unique_ptr<char[]> sig(abi::__cxa_demangle(typeid(arg_).name(), nullptr, nullptr, &status));
    name = sig.get();
#    else
    name = typeid(arg_).name();
#    endif
    return name;
}
inline auto
escaped_char(char c_) -> std::string {
    switch (c_) {
        case '\r': return "\\r";
        case '\n': return "\\n";
        case '\t': return "\\t";
        case '\f': return "\\f";
        case '\v': return "\\v";
        case '\"': return "\\\"";
        case '\b': return "\\b";
        case '\\': return "\\\\";
        default: break;
    }
    return std::string(1, c_);
}
static auto
escaped_string(std::string const& str_) -> std::string {
    std::string s{str_};
    std::size_t p{0};
    while ((p = s.find_first_of("\r\n\t\f\v\"\b\\", p)) != std::string::npos) {
        s.replace(p, 1, escaped_char(s[p]));
        p += 2;
    }
    return s;
}
template<typename T, TPP_INTERN_ENABLE_IF(TPP_INTERN_HAS_STREAM_CAPABILITY(T, std::ostringstream) &&
                                          !TPP_INTERN_IS_FLOAT(T))>
inline auto
to_string(T const& arg_) -> std::string {
    std::ostringstream oss;
    oss << arg_;
    return oss.str();
}
template<typename T, TPP_INTERN_ENABLE_IF(TPP_INTERN_IS_FLOAT(T))>
inline auto
to_string(T const& arg_) -> std::string {
    std::ostringstream oss;
    oss << std::setprecision(std::numeric_limits<T>::max_digits10) << arg_;
    return oss.str();
}
template<typename T, TPP_INTERN_ENABLE_IF(!TPP_INTERN_HAS_STREAM_CAPABILITY(T, std::ostringstream))>
inline auto
to_string(T const& arg_) -> std::string {
    return name_for_type<T>(arg_);
}
inline auto
to_string(std::string const& arg_) -> std::string {
    return std::string("\"") + escaped_string(arg_) + "\"";
}
inline auto
to_string(char const* const& arg_) -> std::string {
    return std::string("\"") + escaped_string(arg_) + "\"";
}
inline auto
to_string(char const& arg_) -> std::string {
    return std::string("'") + escaped_char(arg_) + "'";
}
inline auto
to_string(std::nullptr_t const&) -> std::string {
    return "0";
}
inline auto
to_string(bool const& arg_) -> std::string {
    return arg_ ? "true" : "false";
}
inline auto
to_string(regex const& arg_) -> std::string {
    return to_string(arg_.pattern);
}
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_ASSERT_LOC_HPP
#    define TPP_ASSERT_LOC_HPP
namespace tpp
{
namespace intern
{
namespace assert
{
struct loc final
{
    char const* file;
    int const   line;
};
}  // namespace assert
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_ASSERT_ASSERTION_FAILURE_HPP
#    define TPP_ASSERT_ASSERTION_FAILURE_HPP
namespace tpp
{
namespace intern
{
namespace assert
{
class assertion_failure : public std::exception
{
public:
    assertion_failure(std::string const& msg_, loc const& loc_)
        : m_msg(msg_ + " at " + loc_.file + ":" + std::to_string(loc_.line)) {}
    inline auto
    what() const noexcept -> char const* override {
        return m_msg.c_str();
    }

private:
    std::string const m_msg;
};
}  // namespace assert
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_ASSERT_ASSERT_HPP
#    define TPP_ASSERT_ASSERT_HPP
namespace tpp
{
namespace intern
{
template<typename T>
class throwable
{
public:
    explicit throwable(T const& t_) : m_t(t_) {}
    auto
    cause() const -> T const& {
        return m_t;
    }

private:
    T m_t;
};
namespace assert
{
template<typename A, typename... Args>
static inline auto
make_assertion(Args&&... args) -> A {
    return A{std::forward<Args>(args)...};
}
static inline void
fail_assertion(std::tuple<std::string&&, char const*, std::string&&>&& asrt_, loc const& loc_) {
    throw assertion_failure{std::string("Expected ")
                                .append(std::get<0>(asrt_))
                                .append(" ")
                                .append(std::get<1>(asrt_))
                                .append(" ")
                                .append(std::get<2>(asrt_)),
                            loc_};
}
template<typename T, typename Fn>
static auto
assert_throws(Fn&& fn_, char const* tname_, loc&& loc_) -> throwable<T> {
    try {
        fn_();
    } catch (T const& e) {
        return throwable<T>{e};
    } catch (std::exception const& e) {
        throw assertion_failure("Wrong exception thrown, caught " + to_string(e) + "(\"" +
                                    escaped_string(e.what()) + "\")",
                                loc_);
    } catch (...) {
        throw assertion_failure("Wrong exception thrown", loc_);
    }
    throw assertion_failure(std::string("No exception thrown, expected ") + tname_, loc_);
}
template<typename Fn, TPP_INTERN_ENABLE_IF(!TPP_INTERN_IS_VOID(decltype(std::declval<Fn>()())))>
static auto
assert_nothrow(Fn&& fn_, loc&& loc_) -> decltype(fn_()) {
    try {
        return fn_();
    } catch (std::exception const& e) {
        throw assertion_failure(
            "Expected no exception, caught " + to_string(e) + "(\"" + escaped_string(e.what()) + "\")", loc_);
    } catch (...) {
        throw assertion_failure("Expected no exception", loc_);
    }
}
template<typename Fn, TPP_INTERN_ENABLE_IF(TPP_INTERN_IS_VOID(decltype(std::declval<Fn>()())))>
static void
assert_nothrow(Fn&& fn_, loc&& loc_) {
    try {
        fn_();
    } catch (std::exception const& e) {
        throw assertion_failure(
            "Expected no exception, caught " + to_string(e) + "(\"" + escaped_string(e.what()) + "\")", loc_);
    } catch (...) {
        throw assertion_failure("Expected no exception", loc_);
    }
}
template<typename Fn, TPP_INTERN_ENABLE_IF(!TPP_INTERN_IS_VOID(decltype(std::declval<Fn>()())))>
static auto
assert_runtime(Fn&& fn_, double max_ms_, loc&& loc_) -> decltype(fn_()) {
    TPP_INTERN_SYNC {
        duration        dur;
        decltype(fn_()) res{fn_()};
        double          dur_ms{dur.get()};
        if (dur_ms > max_ms_) {
            throw assertion_failure("Expected the runtime to be less " + to_string(max_ms_) + "ms, but was " +
                                        to_string(dur_ms) + "ms",
                                    loc_);
        }
        return res;
    }
}
template<typename Fn, TPP_INTERN_ENABLE_IF(TPP_INTERN_IS_VOID(decltype(std::declval<Fn>()())))>
static void
assert_runtime(Fn&& fn_, double max_ms_, loc&& loc_) {
    double dur_ms{.0};
    TPP_INTERN_SYNC {
        duration dur;
        fn_();
        dur_ms = dur.get();
    }
    if (dur_ms > max_ms_) {
        throw assertion_failure("Expected the runtime to be less " + to_string(max_ms_) + "ms, but was " +
                                    to_string(dur_ms) + "ms",
                                loc_);
    }
}
}  // namespace assert
}  // namespace intern
}  // namespace tpp
#    define TPP_DEFINE_ASSERTION(NAME, CMPSTR, PRED)                                                        \
        namespace tpp                                                                                       \
        {                                                                                                   \
        namespace intern                                                                                    \
        {                                                                                                   \
        namespace assert                                                                                    \
        {                                                                                                   \
        namespace ns_##NAME {                                                                               \
            static constexpr auto CMP_STR     = "to be " CMPSTR;                                            \
            static constexpr auto NEG_CMP_STR = "to be not " CMPSTR;                                        \
        }                                                                                                   \
        struct NAME                                                                                         \
        {                                                                                                   \
            template<typename V, typename E = V>                                                            \
            NAME(V&& v_, E&& e_, bool neg_, loc&& loc_) {                                                   \
                if ((PRED) == neg_) {                                                                       \
                    fail_assertion(std::forward_as_tuple(                                                   \
                                       to_string(v_), (neg_ ? ns_##NAME::NEG_CMP_STR : ns_##NAME::CMP_STR), \
                                       to_string(e_)),                                                      \
                                   loc_);                                                                   \
                }                                                                                           \
            }                                                                                               \
        };                                                                                                  \
        }                                                                                                   \
        }                                                                                                   \
        }
#    define TPP_PROVIDE_ASSERTION(A, S)   \
        namespace tpp                     \
        {                                 \
        namespace assert                  \
        {                                 \
        using S = tpp::intern::assert::A; \
        }                                 \
        }
#    define ASSERT(V, A, ...)                                                      \
        tpp::intern::assert::make_assertion<tpp::assert::A>(V, __VA_ARGS__, false, \
                                                            tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_NOT(V, A, ...)                                                   \
        tpp::intern::assert::make_assertion<tpp::assert::A>((V), __VA_ARGS__, true, \
                                                            tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_TRUE(V) ASSERT_EQ((V), true)
#    define ASSERT_FALSE(V) ASSERT_EQ((V), false)
#    define ASSERT_NULL(P)                                                                      \
        static_assert(TPP_INTERN_IS(std::is_pointer, std::remove_reference<decltype(P)>::type), \
                      "ASSERT_NULL may only be used with pointer types!");                      \
        ASSERT_EQ(static_cast<void const*>(P), nullptr)
#    define ASSERT_NOT_NULL(P)                                                                  \
        static_assert(TPP_INTERN_IS(std::is_pointer, std::remove_reference<decltype(P)>::type), \
                      "ASSERT_NOT_NULL may only be used with pointer types!");                  \
        ASSERT_NOT_EQ(static_cast<void const*>(P), nullptr)
#    define ASSERT_THROWS(F, T)                                                                             \
        tpp::intern::assert::assert_throws<T>([&] { F; }, #T, tpp::intern::assert::loc{__FILE__, __LINE__}) \
            .cause()
#    define ASSERT_NOTHROW(F) \
        tpp::intern::assert::assert_nothrow([&] { F; }, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_RUNTIME(F, M) \
        tpp::intern::assert::assert_runtime([&] { F; }, M, tpp::intern::assert::loc{__FILE__, __LINE__})
#endif
#ifndef TPP_ASSERT_ORDERING_HPP
#    define TPP_ASSERT_ORDERING_HPP
TPP_DEFINE_ASSERTION(assert_greater, "greater than", v_ > e_)
TPP_DEFINE_ASSERTION(assert_less, "less than", v_ < e_)
TPP_PROVIDE_ASSERTION(assert_greater, GT)
TPP_PROVIDE_ASSERTION(assert_less, LT)
#    define ASSERT_GT(...)                                                        \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_greater>( \
            __VA_ARGS__, false, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_NOT_GT(...)                                                    \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_greater>( \
            __VA_ARGS__, true, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_LT(...)                                                     \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_less>( \
            __VA_ARGS__, false, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_NOT_LT(...)                                                 \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_less>( \
            __VA_ARGS__, true, tpp::intern::assert::loc{__FILE__, __LINE__})
#endif
#ifndef TPP_ASSERT_EQUALITY_HPP
#    define TPP_ASSERT_EQUALITY_HPP
#    define TPP_EPSILON(E) double tpp::epsilon = E;
namespace tpp
{
extern double epsilon;
namespace intern
{
namespace assert
{
namespace ns_equals
{
static constexpr auto CMP_STR     = "to be equals";
static constexpr auto NEG_CMP_STR = "to be not equals";
}  // namespace ns_equals
struct assert_equals
{
    template<typename V, typename E = V,
             TPP_INTERN_ENABLE_IF(!TPP_INTERN_IS_FLOAT(V) || !TPP_INTERN_IS_FLOAT(E))>
    assert_equals(V&& v_, E&& e_, bool neg_, loc&& loc_) {
        if ((v_ == e_) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_),
                                                 (neg_ ? ns_equals::NEG_CMP_STR : ns_equals::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
    template<typename F, typename V, typename E = V,
             TPP_INTERN_ENABLE_IF(TPP_INTERN_IS_FLOAT(V) && TPP_INTERN_IS_FLOAT(E))>
    assert_equals(V&& v_, E&& e_, F&& eps_, bool neg_, loc&& loc_) {
        static_assert(TPP_INTERN_IS_FLOAT(F),
                      "An epsilon used in floating point comparison must be itself a floating point!");
        if ((std::abs(v_ - e_) <= std::max(std::abs(v_), std::abs(e_)) * eps_) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_),
                                                 (neg_ ? ns_equals::NEG_CMP_STR : ns_equals::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
    template<typename V, typename E = V,
             TPP_INTERN_ENABLE_IF(TPP_INTERN_IS_FLOAT(V) && TPP_INTERN_IS_FLOAT(E))>
    assert_equals(V&& v_, E&& e_, bool neg_, loc&& loc_) {
        typename std::decay<E>::type eps_ = static_cast<E>(epsilon);
        if ((std::abs(v_ - e_) <= std::max(std::abs(v_), std::abs(e_)) * eps_) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_),
                                                 (neg_ ? ns_equals::NEG_CMP_STR : ns_equals::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
};
}  // namespace assert
}  // namespace intern
}  // namespace tpp
TPP_PROVIDE_ASSERTION(assert_equals, EQ)
#    define ASSERT_EQ(...)                                                       \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_equals>( \
            __VA_ARGS__, false, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_NOT_EQ(...)                                                   \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_equals>( \
            __VA_ARGS__, true, tpp::intern::assert::loc{__FILE__, __LINE__})
#endif
#ifndef TPP_ASSERT_RANGE_HPP
#    define TPP_ASSERT_RANGE_HPP
namespace tpp
{
namespace intern
{
namespace assert
{
namespace ns_in
{
static constexpr auto CMP_STR     = "to be in";
static constexpr auto NEG_CMP_STR = "to be not in";
}  // namespace ns_in
struct assert_in
{
    template<typename V, typename E = V,
             TPP_INTERN_ENABLE_IF(TPP_INTERN_HAS_ITERATOR_CAPABILITY(E) &&
                                  !TPP_INTERN_IS_TYPE(E, std::string))>
    assert_in(V&& v_, E&& e_, bool neg_, loc&& loc_) {
        if ((std::find(e_.cbegin(), e_.cend(), v_) != e_.cend()) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_), (neg_ ? ns_in::NEG_CMP_STR : ns_in::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
    template<typename V, typename E = V, TPP_INTERN_ENABLE_IF(TPP_INTERN_IS_TYPE(E, std::string))>
    assert_in(V&& v_, E&& e_, bool neg_, loc&& loc_) {
        if ((e_.find(v_) != std::string::npos) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_), (neg_ ? ns_in::NEG_CMP_STR : ns_in::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
};
}  // namespace assert
}  // namespace intern
}  // namespace tpp
TPP_PROVIDE_ASSERTION(assert_in, IN)
#    define ASSERT_IN(...)                                                   \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_in>( \
            __VA_ARGS__, false, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_NOT_IN(...)                                               \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_in>( \
            __VA_ARGS__, true, tpp::intern::assert::loc{__FILE__, __LINE__})
#endif
#ifndef TPP_ASSERT_REGEX_HPP
#    define TPP_ASSERT_REGEX_HPP
namespace tpp
{
namespace intern
{
namespace assert
{
namespace ns_match
{
static constexpr auto CMP_STR     = "to match";
static constexpr auto NEG_CMP_STR = "to not match";
}  // namespace ns_match
namespace ns_like
{
static constexpr auto CMP_STR     = "to be like";
static constexpr auto NEG_CMP_STR = "to be not like";
}  // namespace ns_like
struct assert_match
{
    template<typename R, typename V, typename E = V>
    assert_match(V&& v_, E&& e_, R& r_, bool neg_, loc&& loc_) {
        if (std::regex_match(v_, r_, std::regex(e_)) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_),
                                                 (neg_ ? ns_match::NEG_CMP_STR : ns_match::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
    template<typename V, typename E = V>
    assert_match(V&& v_, E&& e_, bool neg_, loc&& loc_) {
        if (std::regex_match(v_, std::regex(e_)) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_),
                                                 (neg_ ? ns_match::NEG_CMP_STR : ns_match::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
};
struct assert_like
{
    template<typename R, typename V, typename E = V>
    assert_like(V&& v_, E&& e_, R& r_, bool neg_, loc&& loc_) {
        if (std::regex_search(v_, r_, std::regex(e_)) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_),
                                                 (neg_ ? ns_like::NEG_CMP_STR : ns_like::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
    template<typename V, typename E = V>
    assert_like(V&& v_, E&& e_, bool neg_, loc&& loc_) {
        if (std::regex_search(v_, std::regex(e_)) == neg_) {
            fail_assertion(std::forward_as_tuple(to_string(v_),
                                                 (neg_ ? ns_like::NEG_CMP_STR : ns_like::CMP_STR),
                                                 to_string(e_)),
                           loc_);
        }
    }
};
}  // namespace assert
}  // namespace intern
}  // namespace tpp
TPP_PROVIDE_ASSERTION(assert_match, MATCH)
TPP_PROVIDE_ASSERTION(assert_like, LIKE)
#    define ASSERT_MATCH(...)                                                   \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_match>( \
            __VA_ARGS__, false, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_NOT_MATCH(...)                                               \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_match>( \
            __VA_ARGS__, true, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_LIKE(...)                                                   \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_like>( \
            __VA_ARGS__, false, tpp::intern::assert::loc{__FILE__, __LINE__})
#    define ASSERT_NOT_LIKE(...)                                               \
        tpp::intern::assert::make_assertion<tpp::intern::assert::assert_like>( \
            __VA_ARGS__, true, tpp::intern::assert::loc{__FILE__, __LINE__})
#endif
#ifndef TPP_TEST_TESTCASE_HPP
#    define TPP_TEST_TESTCASE_HPP
namespace tpp
{
namespace intern
{
namespace test
{
using test_function = std::function<void()>;
struct test_context final
{
    char const* const tc_name;
    char const* const ts_name;
};
class testcase
{
public:
    testcase(testcase const&) = delete;
    ~testcase() noexcept      = default;
    auto
    operator=(testcase const&) -> testcase& = delete;
    testcase(test_context&& ctx_, test_function&& fn_)
        : m_name(ctx_.tc_name), m_suite_name(ctx_.ts_name), m_test_fn(std::move(fn_)) {}
    testcase(testcase&& other_) noexcept
        : m_name(other_.m_name),
          m_suite_name(other_.m_suite_name),
          m_result(other_.m_result),
          m_elapsed_t(other_.m_elapsed_t),
          m_err_msg(std::move(other_.m_err_msg)),
          m_test_fn(std::move(other_.m_test_fn)) {}
    auto
    operator=(testcase&& other_) noexcept -> testcase& {
        m_name       = other_.m_name;
        m_suite_name = other_.m_suite_name;
        m_result     = other_.m_result;
        m_elapsed_t  = other_.m_elapsed_t;
        m_err_msg    = std::move(other_.m_err_msg);
        m_test_fn    = std::move(other_.m_test_fn);
        return *this;
    }
    enum results
    {
        IS_UNDONE,
        HAS_PASSED,
        HAS_FAILED,
        HAD_ERROR
    };
    void
    operator()() {
        if (m_result != IS_UNDONE) {
            return;
        }
        class duration dur;
        try {
            m_test_fn();
            pass();
        } catch (assert::assertion_failure const& e) {
            fail(e.what());
        } catch (std::exception const& e) {
            error(e.what());
        } catch (...) {
            error();
        }
        m_elapsed_t = dur.get();
    }
    inline auto
    result() const -> results {
        return m_result;
    }
    inline auto
    elapsed_time() const -> double {
        return m_elapsed_t;
    }
    inline auto
    reason() const -> std::string const& {
        return m_err_msg;
    }
    inline auto
    name() const -> char const* {
        return m_name;
    }
    inline auto
    suite_name() const -> char const* {
        return m_suite_name;
    }
    inline void
    cout(std::string const& str_) {
        m_cout = str_;
    }
    inline void
    cerr(std::string const& str_) {
        m_cerr = str_;
    }
    inline auto
    cout() const -> std::string const& {
        return m_cout;
    }
    inline auto
    cerr() const -> std::string const& {
        return m_cerr;
    }

private:
    inline void
    pass() {
        m_result = HAS_PASSED;
    }
    inline void
    fail(char const* msg_) {
        m_result  = HAS_FAILED;
        m_err_msg = msg_;
    }
    inline void
    error(char const* msg_ = "unknown error") {
        m_result  = HAD_ERROR;
        m_err_msg = msg_;
    }
    char const*   m_name;
    char const*   m_suite_name;
    results       m_result{IS_UNDONE};
    double        m_elapsed_t{.0};
    std::string   m_err_msg;
    std::string   m_cout;
    std::string   m_cerr;
    test_function m_test_fn;
};
}  // namespace test
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_TEST_STREAMBUF_PROXY_HPP
#    define TPP_TEST_STREAMBUF_PROXY_HPP
#    ifdef _OPENMP
#        include <omp.h>
#    else
#        define omp_get_max_threads() 1
#        define omp_get_thread_num() 0
#    endif
namespace tpp
{
namespace intern
{
namespace test
{
class streambuf_proxy : public std::streambuf
{
public:
    streambuf_proxy(streambuf_proxy const&)     = delete;
    streambuf_proxy(streambuf_proxy&&) noexcept = delete;
    auto
    operator=(streambuf_proxy const&) -> streambuf_proxy& = delete;
    auto
    operator=(streambuf_proxy&&) noexcept -> streambuf_proxy& = delete;
    explicit streambuf_proxy(std::ostream& stream_)
        : m_orig_buf(stream_.rdbuf(this)), m_orig_stream(stream_) {}
    ~streambuf_proxy() noexcept override {
        m_orig_stream.rdbuf(m_orig_buf);
    }
    virtual auto
    str() -> std::string = 0;

protected:
    template<typename T>
    class clearer
    {
        T* m_buf;

    public:
        clearer(clearer const&)     = delete;
        clearer(clearer&&) noexcept = delete;
        auto
        operator=(clearer const&) -> clearer& = delete;
        auto
        operator=(clearer&&) noexcept -> clearer& = delete;
        explicit clearer(T* b_) : m_buf(b_) {}
        ~clearer() noexcept {
            m_buf->str("");
        }
    };
    std::streambuf* m_orig_buf;
    std::ostream&   m_orig_stream;
};
class streambuf_proxy_single : public streambuf_proxy
{
public:
    explicit streambuf_proxy_single(std::ostream& stream_) : streambuf_proxy(stream_) {}
    auto
    str() -> std::string override {
        clearer<std::stringbuf> _(&m_buffer);
        return m_buffer.str();
    }

private:
    auto
    overflow(int_type c_) -> int_type override {
        return m_buffer.sputc(std::stringbuf::traits_type::to_char_type(c_));
    }
    auto
    xsputn(char const* s_, std::streamsize n_) -> std::streamsize override {
        return m_buffer.sputn(s_, n_);
    }
    std::stringbuf m_buffer;
};
class streambuf_proxy_omp : public streambuf_proxy
{
#    define TPP_INTERN_CURRENT_THREAD_BUFFER() (m_thd_buffers[static_cast<std::size_t>(omp_get_thread_num())])
public:
    explicit streambuf_proxy_omp(std::ostream& stream_)
        : streambuf_proxy(stream_), m_thd_buffers(static_cast<std::size_t>(omp_get_max_threads())) {}
    auto
    str() -> std::string override {
        auto&                   buf = TPP_INTERN_CURRENT_THREAD_BUFFER();
        clearer<std::stringbuf> _(&buf);
        return buf.str();
    }

private:
    auto
    overflow(int_type c_) -> int_type override {
        return TPP_INTERN_CURRENT_THREAD_BUFFER().sputc(std::stringbuf::traits_type::to_char_type(c_));
    }
    auto
    xsputn(char const* s_, std::streamsize n_) -> std::streamsize override {
        return TPP_INTERN_CURRENT_THREAD_BUFFER().sputn(s_, n_);
    }
    std::vector<std::stringbuf> m_thd_buffers;
#    undef TPP_INTERN_CURRENT_THREAD_BUFFER
};
}  // namespace test
}  // namespace intern
}  // namespace tpp
#    ifndef _OPENMP
#        undef omp_get_max_threads
#        undef omp_get_thread_num
#    endif
#endif
#ifndef TPP_TEST_STATISTICS_HPP
#    define TPP_TEST_STATISTICS_HPP
namespace tpp
{
namespace intern
{
namespace test
{
class statistic
{
public:
    inline auto
    tests() const -> std::size_t {
        return m_num_tests;
    }
    inline auto
    successes() const -> std::size_t {
        return m_num_tests - m_num_errs - m_num_fails;
    }
    inline auto
    failures() const -> std::size_t {
        return m_num_fails;
    }
    inline auto
    errors() const -> std::size_t {
        return m_num_errs;
    }
    inline auto
    elapsed_time() const -> double {
        return m_elapsed_t;
    }

private:
    friend class testsuite;
    friend class testsuite_parallel;
    std::size_t m_num_tests{0};
    std::size_t m_num_fails{0};
    std::size_t m_num_errs{0};
    double      m_elapsed_t{.0};
};
}  // namespace test
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_TEST_TESTSUITE_HPP
#    define TPP_TEST_TESTSUITE_HPP
namespace tpp
{
namespace intern
{
namespace test
{
template<typename T>
struct streambuf_proxies
{
    T cout{std::cout};
    T cerr{std::cerr};
};
class testsuite;
using testsuite_ptr = std::shared_ptr<testsuite>;
class testsuite
{
protected:
    struct enable
    {};

public:
    using hook_function             = std::function<void()>;
    testsuite(testsuite const&)     = delete;
    testsuite(testsuite&&) noexcept = delete;
    virtual ~testsuite() noexcept   = default;
    auto
    operator=(testsuite const&) -> testsuite& = delete;
    auto
    operator=(testsuite&&) noexcept -> testsuite& = delete;
    static auto
    create(char const* name_) -> testsuite_ptr {
        return std::make_shared<testsuite>(enable{}, name_);
    }
    virtual void
    run() {
        if (m_state != IS_DONE) {
            duration d;
            m_stats.m_num_tests = m_testcases.size();
            streambuf_proxies<streambuf_proxy_single> bufs;
            m_setup_fn();
            std::for_each(m_testcases.begin(), m_testcases.end(), [&](testcase& tc_) {
                if (tc_.result() == testcase::IS_UNDONE) {
                    m_pretest_fn();
                    tc_();
                    switch (tc_.result()) {
                        case testcase::HAS_FAILED: ++m_stats.m_num_fails; break;
                        case testcase::HAD_ERROR: ++m_stats.m_num_errs; break;
                        default: break;
                    }
                    m_posttest_fn();
                    tc_.cout(bufs.cout.str());
                    tc_.cerr(bufs.cerr.str());
                }
            });
            m_teardown_fn();
            m_state = IS_DONE;
            m_stats.m_elapsed_t += d.get();
        }
    }
    void
    test(char const* name_, hook_function&& fn_) {
        m_testcases.emplace_back(test_context{name_, m_name}, std::move(fn_));
        m_state = IS_PENDING;
    }
    void
    setup(hook_function&& fn_) {
        m_setup_fn.fn = std::move(fn_);
    }
    void
    teardown(hook_function&& fn_) {
        m_teardown_fn.fn = std::move(fn_);
    }
    void
    before_each(hook_function&& fn_) {
        m_pretest_fn.fn = std::move(fn_);
    }
    void
    after_each(hook_function&& fn_) {
        m_posttest_fn.fn = std::move(fn_);
    }
    inline auto
    name() const -> char const* {
        return m_name;
    }
    inline auto
    timestamp() const -> std::chrono::system_clock::time_point const& {
        return m_create_time;
    }
    inline auto
    statistics() const -> statistic const& {
        return m_stats;
    }
    inline auto
    testcases() const -> std::vector<testcase> const& {
        return m_testcases;
    }
    testsuite(enable, char const* name_) : m_name(name_), m_create_time(std::chrono::system_clock::now()) {}

protected:
    struct optional_functor final
    {
        void
        operator()() const noexcept {
            if (fn) {
                fn();
            }
        }
        hook_function fn;
    };
    enum states
    {
        IS_PENDING,
        IS_DONE
    };
    char const* const                           m_name;
    std::chrono::system_clock::time_point const m_create_time;
    statistic                                   m_stats;
    std::vector<testcase>                       m_testcases;
    states                                      m_state{IS_PENDING};
    optional_functor                            m_setup_fn;
    optional_functor                            m_teardown_fn;
    optional_functor                            m_pretest_fn;
    optional_functor                            m_posttest_fn;
};
}  // namespace test
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_TEST_TESTSUITE_PARALLEL_HPP
#    define TPP_TEST_TESTSUITE_PARALLEL_HPP
namespace tpp
{
namespace intern
{
namespace test
{
class testsuite_parallel : public testsuite
{
public:
    static auto
    create(char const* name_) -> testsuite_ptr {
        return std::make_shared<testsuite_parallel>(enable{}, name_);
    }
    void
    run() override {
        if (m_state != IS_DONE) {
            duration d;
            if (m_testcases.size() > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max())) {
                throw std::overflow_error("Too many testcases! Size would overflow loop variant.");
            }
            auto const tc_size{static_cast<std::int64_t>(m_testcases.size())};
            m_stats.m_num_tests = m_testcases.size();
            streambuf_proxies<streambuf_proxy_omp> bufs;
            m_setup_fn();
#    pragma omp parallel default(shared)
            {
                std::size_t fails{0};
                std::size_t errs{0};
#    pragma omp for schedule(dynamic)
                for (std::int64_t i = 0; i < tc_size; ++i) {
                    auto& tc{m_testcases[static_cast<std::size_t>(i)]};
                    if (tc.result() == testcase::IS_UNDONE) {
                        m_pretest_fn();
                        tc();
                        switch (tc.result()) {
                            case testcase::HAS_FAILED: ++fails; break;
                            case testcase::HAD_ERROR: ++errs; break;
                            default: break;
                        }
                        m_posttest_fn();
                        tc.cout(bufs.cout.str());
                        tc.cerr(bufs.cerr.str());
                    }
                }
#    pragma omp critical
                {
                    m_stats.m_num_fails += fails;
                    m_stats.m_num_errs += errs;
                }
            }
            m_teardown_fn();
            m_state = IS_DONE;
            m_stats.m_elapsed_t += d.get();
        }
    }
    testsuite_parallel(enable e_, char const* name_) : testsuite(e_, name_) {}
};
}  // namespace test
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_REPORT_REPORTER_HPP
#    define TPP_REPORT_REPORTER_HPP
namespace tpp
{
namespace intern
{
namespace report
{
namespace fmt
{
static constexpr auto SPACE = ' ';
static constexpr auto LF    = '\n';
namespace ansi
{
static constexpr auto RED        = "\x1b[0;91m";
static constexpr auto GREEN      = "\x1b[0;92m";
static constexpr auto YELLOW     = "\x1b[0;93m";
static constexpr auto BLUE       = "\x1b[0;94m";
static constexpr auto CYAN       = "\x1b[0;96m";
static constexpr auto MAGENTA    = "\x1b[0;95m";
static constexpr auto WHITE_BOLD = "\x1b[1;97m";
static constexpr auto RST        = "\x1b[0m";
}  // namespace ansi
}  // namespace fmt
class reporter;
using reporter_ptr = std::shared_ptr<reporter>;
class reporter : public std::enable_shared_from_this<reporter>
{
public:
    reporter(reporter const&)     = delete;
    reporter(reporter&&) noexcept = delete;
    virtual ~reporter() noexcept  = default;
    auto
    operator=(reporter const&) -> reporter& = delete;
    auto
    operator=(reporter&&) noexcept -> reporter& = delete;
    void
    report(test::testsuite_ptr const& ts_) {
        report_testsuite(ts_);
        m_out_stream.flush();
    }
    virtual void
    begin_report() {
        m_abs_errs  = 0;
        m_abs_fails = 0;
        m_abs_tests = 0;
        m_abs_time  = .0;
    };
    virtual void
    end_report() = 0;
    inline auto
    faults() const -> std::size_t {
        return m_abs_errs + m_abs_fails;
    }
    auto
    with_color() -> reporter_ptr {
        m_colors.RED     = fmt::ansi::RED;
        m_colors.GREEN   = fmt::ansi::GREEN;
        m_colors.YELLOW  = fmt::ansi::YELLOW;
        m_colors.BLUE    = fmt::ansi::BLUE;
        m_colors.CYAN    = fmt::ansi::CYAN;
        m_colors.MAGENTA = fmt::ansi::MAGENTA;
        m_colors.W_BOLD  = fmt::ansi::WHITE_BOLD;
        m_colors.RESET   = fmt::ansi::RST;
        return shared_from_this();
    }
    auto
    with_captured_output() -> reporter_ptr {
        m_capture = true;
        return shared_from_this();
    }
    auto
    with_stripping() -> reporter_ptr {
        m_stripped = true;
        return shared_from_this();
    }

protected:
    struct enable
    {};
    struct color_palette
    {
        char const* RED{""};
        char const* GREEN{""};
        char const* YELLOW{""};
        char const* BLUE{""};
        char const* CYAN{""};
        char const* MAGENTA{""};
        char const* W_BOLD{""};
        char const* RESET{""};
                    operator char const *() const {
            return RESET;
        }
    };
    explicit reporter(std::ostream& stream_) : m_out_stream(stream_) {
        if (!m_out_stream) {
            throw std::runtime_error("could not open stream for report");
        }
    }
    explicit reporter(std::string const& fname_) : m_out_file(fname_), m_out_stream(m_out_file) {
        if (!m_out_stream) {
            throw std::runtime_error("could not open file for report");
        }
    }
    virtual void
    report_testsuite(test::testsuite_ptr const& ts_) {
        m_abs_errs += ts_->statistics().errors();
        m_abs_fails += ts_->statistics().failures();
        m_abs_tests += ts_->statistics().tests();
        m_abs_time += ts_->statistics().elapsed_time();
        std::for_each(ts_->testcases().begin(), ts_->testcases().end(),
                      [this](test::testcase const& tc_) { report_testcase(tc_); });
    }
    virtual void
    report_testcase(test::testcase const& tc_) = 0;
    template<typename T>
    auto
    operator<<(T&& t_) const -> std::ostream& {
        m_out_stream << std::forward<T>(t_);
        return m_out_stream;
    }
    inline void
    space(std::uint32_t depth_ = 1) const {
        if (!m_stripped) {
            for (auto i{0U}; i < depth_; ++i) {
                *this << fmt::SPACE;
            }
        }
    }
    inline void
    newline(std::uint32_t depth_ = 1) const {
        if (!m_stripped) {
            for (auto i{0U}; i < depth_; ++i) {
                *this << fmt::LF;
            }
        }
        space(m_indent_lvl);
    }
    inline void
    push_indent(std::uint32_t lvl_ = 2) {
        m_indent_lvl += lvl_;
    }
    inline void
    pop_indent(std::uint32_t lvl_ = 2) {
        m_indent_lvl -= lvl_;
    }
    inline auto
    capture() const -> bool {
        return m_capture;
    }
    inline auto
    abs_tests() const -> std::size_t {
        return m_abs_tests;
    }
    inline auto
    abs_fails() const -> std::size_t {
        return m_abs_fails;
    }
    inline auto
    abs_errs() const -> std::size_t {
        return m_abs_errs;
    }
    inline auto
    abs_time() const -> double {
        return m_abs_time;
    }
    auto
    color() const -> color_palette const& {
        return m_colors;
    }

private:
    std::ofstream m_out_file;
    std::ostream& m_out_stream;
    std::uint32_t m_indent_lvl{0};
    color_palette m_colors{};
    bool          m_capture{false};
    bool          m_stripped{false};
    std::size_t   m_abs_tests{0};
    std::size_t   m_abs_fails{0};
    std::size_t   m_abs_errs{0};
    double        m_abs_time{0};
};
}  // namespace report
}  // namespace intern
using reporter_ptr = intern::report::reporter_ptr;
}  // namespace tpp
#endif
#ifndef TPP_REPORT_XML_REPORTER_HPP
#    define TPP_REPORT_XML_REPORTER_HPP
namespace tpp
{
namespace intern
{
namespace report
{
class xml_reporter : public reporter
{
public:
    static auto
    create(std::ostream& stream_ = std::cout) -> reporter_ptr {
        return std::make_shared<xml_reporter>(enable{}, stream_);
    }
    static auto
    create(std::string const& fname_) -> reporter_ptr {
        return std::make_shared<xml_reporter>(enable{}, fname_);
    }
    xml_reporter(enable, std::ostream& stream_) : reporter(stream_) {}
    xml_reporter(enable, std::string const& fname_) : reporter(fname_) {}

private:
    void
    report_testsuite(test::testsuite_ptr const& ts_) override {
        std::time_t           stamp = std::chrono::system_clock::to_time_t(ts_->timestamp());
        std::array<char, 128> buff{};
        std::strftime(buff.data(), 127, "%FT%T", std::localtime(&stamp));
        push_indent();
        newline();
        *this << "<testsuite id=\"" << m_id++ << "\" name=\"" << ts_->name() << "\" errors=\""
              << ts_->statistics().errors() << "\" tests=\"" << ts_->statistics().tests() << "\" failures=\""
              << ts_->statistics().failures() << R"(" skipped="0" time=")" << ts_->statistics().elapsed_time()
              << "\" timestamp=\"" << buff.data() << "\">";
        reporter::report_testsuite(ts_);
        newline();
        *this << "</testsuite>";
        pop_indent();
    }
    void
    report_testcase(test::testcase const& tc_) override {
        push_indent();
        newline();
        *this << "<testcase name=\"" << tc_.name() << "\" classname=\"" << tc_.suite_name() << "\" time=\""
              << tc_.elapsed_time() << "\"";
        if (tc_.result() != test::testcase::HAS_PASSED) {
            auto const unsuccess = [&] {
                return tc_.result() == test::testcase::HAD_ERROR ? "error" : "failure";
            };
            *this << '>';
            push_indent();
            newline();
            *this << '<' << unsuccess() << " message=\"" << tc_.reason() << "\"></" << unsuccess() << '>';
            pop_indent();
            print_system_out(tc_);
            newline();
            *this << "</testcase>";
        } else if (capture()) {
            *this << '>';
            print_system_out(tc_);
            newline();
            *this << "</testcase>";
        } else {
            *this << "/>";
        }
        pop_indent();
    }
    void
    begin_report() override {
        reporter::begin_report();
        *this << R"(<?xml version="1.0" encoding="UTF-8" ?>)";
        newline();
        *this << "<testsuites>";
    }
    void
    end_report() override {
        newline();
        *this << "</testsuites>";
        newline();
    }
    void
    print_system_out(test::testcase const& tc_) {
        if (!capture()) {
            return;
        }
        push_indent();
        newline();
        *this << "<system-out>" << tc_.cout() << "</system-out>";
        newline();
        *this << "<system-err>" << tc_.cerr() << "</system-err>";
        pop_indent();
    }
    std::size_t mutable m_id{0};
};
}  // namespace report
}  // namespace intern
using xml_reporter = intern::report::xml_reporter;
}  // namespace tpp
#endif
#ifndef TPP_REPORT_CONSOLE_REPORTER_HPP
#    define TPP_REPORT_CONSOLE_REPORTER_HPP
namespace tpp
{
namespace intern
{
namespace report
{
class console_reporter : public reporter
{
public:
    static auto
    create(std::ostream& stream_ = std::cout) -> reporter_ptr {
        return std::make_shared<console_reporter>(enable{}, stream_);
    }
    static auto
    create(std::string const& fname_) -> reporter_ptr {
        return std::make_shared<console_reporter>(enable{}, fname_);
    }
    console_reporter(enable, std::ostream& stream_) : reporter(stream_) {}
    console_reporter(enable, std::string const& fname_) : reporter(fname_) {}

private:
    void
    report_testsuite(test::testsuite_ptr const& ts_) override {
        *this << "--- " << ts_->name() << " (" << ts_->statistics().elapsed_time() << "ms) ---" << fmt::LF;
        reporter::report_testsuite(ts_);
        *this << fmt::LF;
    }
    void
    report_testcase(test::testcase const& tc_) override {
        *this << fmt::SPACE << tc_.name() << " (" << tc_.elapsed_time() << "ms)" << fmt::LF << fmt::SPACE
              << fmt::SPACE;
        if (capture()) {
            *this << "stdout = \"" << escaped_string(tc_.cout()) << '"' << fmt::LF << fmt::SPACE
                  << fmt::SPACE;
            *this << "stderr = \"" << escaped_string(tc_.cerr()) << '"' << fmt::LF << fmt::SPACE
                  << fmt::SPACE;
        }
        switch (tc_.result()) {
            case test::testcase::HAD_ERROR: *this << color().RED << "ERROR! " << tc_.reason(); break;
            case test::testcase::HAS_FAILED: *this << color().BLUE << "FAILED! " << tc_.reason(); break;
            default: *this << color().GREEN << "PASSED!"; break;
        }
        *this << color() << fmt::LF;
    }
    void
    end_report() override {
        if (abs_errs() > 0) {
            *this << color().MAGENTA;
        } else if (abs_fails() > 0) {
            *this << color().YELLOW;
        } else {
            *this << color().CYAN;
        }
        *this << "=== Result ===" << fmt::LF << "passes: " << abs_tests() - faults() << '/' << abs_tests()
              << " failures: " << abs_fails() << '/' << abs_tests() << " errors: " << abs_errs() << '/'
              << abs_tests() << " (" << abs_time() << "ms)" << color() << fmt::LF;
    }
};
}  // namespace report
}  // namespace intern
using console_reporter = intern::report::console_reporter;
}  // namespace tpp
#endif
#ifndef TPP_REPORT_MARKDOWN_REPORTER_HPP
#    define TPP_REPORT_MARKDOWN_REPORTER_HPP
namespace tpp
{
namespace intern
{
namespace report
{
class markdown_reporter : public reporter
{
public:
    static auto
    create(std::ostream& stream_ = std::cout) -> reporter_ptr {
        return std::make_shared<markdown_reporter>(enable{}, stream_);
    }
    static auto
    create(std::string const& fname_) -> reporter_ptr {
        return std::make_shared<markdown_reporter>(enable{}, fname_);
    }
    markdown_reporter(enable, std::ostream& stream_) : reporter(stream_) {}
    markdown_reporter(enable, std::string const& fname_) : reporter(fname_) {}

private:
    void
    report_testsuite(test::testsuite_ptr const& ts_) override {
        *this << "## " << ts_->name() << fmt::LF << fmt::LF << "|Tests|Successes|Failures|Errors|Time|"
              << fmt::LF << "|-|-|-|-|-|" << fmt::LF << '|' << ts_->statistics().tests() << '|'
              << ts_->statistics().successes() << '|' << ts_->statistics().failures() << '|'
              << ts_->statistics().errors() << '|' << ts_->statistics().elapsed_time() << "ms|" << fmt::LF
              << fmt::LF << "### Tests" << fmt::LF << fmt::LF << "|Name|Time|Status|" << fmt::LF << "|-|-|-|"
              << fmt::LF;
        reporter::report_testsuite(ts_);
        *this << fmt::LF;
        std::for_each(ts_->testcases().cbegin(), ts_->testcases().cend(),
                      [this](test::testcase const& tc_) { testcase_details(tc_); });
    }
    void
    report_testcase(test::testcase const& tc_) override {
        auto const status = [&] {
            switch (tc_.result()) {
                case test::testcase::HAD_ERROR: return "ERROR";
                case test::testcase::HAS_FAILED: return "FAILED";
                default: return "PASSED";
            }
        };
        *this << '|' << tc_.name() << '|' << tc_.elapsed_time() << "ms|" << status() << '|' << fmt::LF;
    }
    void
    begin_report() override {
        reporter::begin_report();
        *this << "# Test Report" << fmt::LF << fmt::LF;
    }
    void
    end_report() override {
        *this << "## Summary" << fmt::LF << fmt::LF << "|Tests|Successes|Failures|Errors|Time|" << fmt::LF
              << "|-|-|-|-|-|" << fmt::LF << '|' << abs_tests() << '|' << (abs_tests() - faults()) << '|'
              << abs_fails() << '|' << abs_errs() << '|' << abs_time() << "ms|" << fmt::LF;
    }
    void
    testcase_details(test::testcase const& tc_) {
        bool const isFailed = tc_.result() != test::testcase::HAS_PASSED;
        if (isFailed || capture()) {
            *this << "#### " << tc_.name() << fmt::LF << fmt::LF;
        }
        if (isFailed) {
            *this << "##### Reason" << fmt::LF << fmt::LF << tc_.reason() << fmt::LF << fmt::LF;
        }
        if (capture()) {
            print_system_out("Out", tc_.cout());
            print_system_out("Err", tc_.cerr());
        }
    }
    void
    print_system_out(char const* chan_, std::string const& out_) {
        *this << "##### System-" << chan_ << fmt::LF << fmt::LF;
        if (!out_.empty()) {
            *this << "```" << fmt::LF << out_ << fmt::LF << "```" << fmt::LF << fmt::LF;
        }
    }
};
}  // namespace report
}  // namespace intern
using markdown_reporter = intern::report::markdown_reporter;
}  // namespace tpp
#endif
#ifndef TPP_REPORT_JSON_REPORTER_HPP
#    define TPP_REPORT_JSON_REPORTER_HPP
namespace tpp
{
namespace intern
{
namespace report
{
class json_reporter : public reporter
{
public:
    static auto
    create(std::ostream& stream_ = std::cout) -> reporter_ptr {
        return std::make_shared<json_reporter>(enable{}, stream_);
    }
    static auto
    create(std::string const& fname_) -> reporter_ptr {
        return std::make_shared<json_reporter>(enable{}, fname_);
    }
    json_reporter(enable, std::ostream& stream_) : reporter(stream_) {}
    json_reporter(enable, std::string const& fname_) : reporter(fname_) {}

private:
    void
    report_testsuite(test::testsuite_ptr const& ts_) override {
        m_first_test = true;
        conditional_prefix(&m_first_suite);
        json_property_string("name", ts_->name(), true, color().CYAN);
        json_property_value("time", ts_->statistics().elapsed_time(), true);
        json_property_value("count", ts_->statistics().tests(), true);
        json_property_value("passes", ts_->statistics().successes(), true);
        json_property_value("failures", ts_->statistics().failures(), true);
        json_property_value("errors", ts_->statistics().errors(), true);
        *this << "\"tests\":";
        space();
        *this << '[';
        reporter::report_testsuite(ts_);
        newline();
        *this << ']';
        pop_indent();
        newline();
        *this << '}';
        pop_indent();
    }
    void
    report_testcase(test::testcase const& tc_) override {
        auto const dres = decode_result(tc_.result());
        conditional_prefix(&m_first_test);
        json_property_string("name", tc_.name(), true, color().W_BOLD);
        json_property_string("result", std::get<0>(dres), true, std::get<1>(dres));
        json_property_string("reason", tc_.reason(), true, std::get<1>(dres));
        json_property_value("time", tc_.elapsed_time(), capture());
        if (capture()) {
            json_property_string("stdout", tc_.cout(), true);
            json_property_string("stderr", tc_.cerr(), false);
        }
        pop_indent();
        newline();
        *this << '}';
        pop_indent();
    }
    void
    begin_report() override {
        reporter::begin_report();
        *this << '{';
        push_indent();
        newline();
        *this << "\"testsuites\":";
        space();
        *this << '[';
    }
    void
    end_report() override {
        newline();
        *this << "],";
        newline();
        json_property_value("count", abs_tests(), true, color().W_BOLD);
        json_property_value("passes", abs_tests() - faults(), true, color().GREEN);
        json_property_value("failures", abs_fails(), true, color().BLUE);
        json_property_value("errors", abs_errs(), true, color().RED);
        json_property_value("time", abs_time(), false);
        pop_indent();
        newline();
        *this << '}';
        newline();
    }
    template<typename T>
    void
    json_property_string(char const* name_, T&& val_, bool has_next_, char const* col_ = nullptr) {
        *this << '"' << name_ << "\":";
        space();
        *this << (col_ ? col_ : "") << '"' << escaped_string(std::forward<T>(val_)) << '"'
              << (col_ ? color() : "");
        if (has_next_) {
            *this << ',';
            newline();
        }
    }
    template<typename T>
    void
    json_property_value(char const* name_, T&& val_, bool has_next_, char const* col_ = nullptr) {
        *this << '"' << name_ << "\":";
        space();
        *this << (col_ ? col_ : "") << std::forward<T>(val_) << (col_ ? color() : "");
        if (has_next_) {
            *this << ',';
            newline();
        }
    }
    void
    conditional_prefix(bool* cond_) {
        push_indent();
        if (!*cond_) {
            *this << ',';
        } else {
            *cond_ = false;
        }
        newline();
        *this << '{';
        push_indent();
        newline();
    }
    inline auto
    decode_result(test::testcase::results res_) -> std::tuple<char const*, char const*> {
        switch (res_) {
            case test::testcase::HAD_ERROR: return {"error", color().RED};
            case test::testcase::HAS_FAILED: return {"failure", color().BLUE};
            default: return {"success", color().GREEN};
        }
    }
    bool m_first_suite{true};
    bool m_first_test{true};
};
}  // namespace report
}  // namespace intern
using json_reporter = intern::report::json_reporter;
}  // namespace tpp
#endif
#ifndef TPP_REPORT_REPORTER_FACTORY_HPP
#    define TPP_REPORT_REPORTER_FACTORY_HPP
namespace tpp
{
namespace intern
{
namespace report
{
struct reporter_config
{
    bool          color{false};
    bool          capture_out{false};
    bool          strip{false};
    std::string   outfile;
    std::ostream* ostream{nullptr};
};
class reporter_factory
{
public:
    template<typename T>
    static auto
    make(reporter_config const& cfg_) -> reporter_ptr {
        static_assert(TPP_INTERN_IS(std::is_base_of, reporter, T),
                      "Cannot make a concrete reporter that is not derived from abstract reporter!");
        auto rep = cfg_.outfile.empty() ? (cfg_.ostream ? T::create(*cfg_.ostream) : T::create()) :
                                          T::create(cfg_.outfile);
        if (cfg_.capture_out) {
            rep->with_captured_output();
        }
        if (cfg_.color) {
            rep->with_color();
        }
        if (cfg_.strip) {
            rep->with_stripping();
        }
        return rep;
    }
};
}  // namespace report
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_CONFIG_HPP
#    define TPP_CONFIG_HPP
namespace tpp
{
namespace intern
{
struct config
{
    enum class filter_mode
    {
        INCLUDE,
        EXCLUDE,
        NONE
    };
    enum class report_format
    {
        XML,
        MD,
        JSON,
        CNS
    };
    auto
    reporter() const -> reporter_ptr {
        switch (report_fmt) {
            case report_format::XML: return report::reporter_factory::make<xml_reporter>(report_cfg);
            case report_format::MD: return report::reporter_factory::make<markdown_reporter>(report_cfg);
            case report_format::JSON: return report::reporter_factory::make<json_reporter>(report_cfg);
            default: return report::reporter_factory::make<console_reporter>(report_cfg);
        }
    }
    report_format           report_fmt{report_format::CNS};
    report::reporter_config report_cfg;
    std::vector<std::regex> f_patterns;
    filter_mode             f_mode{filter_mode::NONE};
};
}  // namespace intern
using config = intern::config;
}  // namespace tpp
#endif
#ifndef TPP_CMDLINE_PARSER_HPP
#    define TPP_CMDLINE_PARSER_HPP
namespace tpp
{
namespace intern
{
class cmdline_parser
{
public:
    struct help_called
    {};
    void
    parse(std::size_t argc_, char const** argv_) {
        auto const args{tokenize_args(argc_, argv_)};
        m_progname = argv_[0];
        for (auto i{1UL}; i < args.size(); ++i) {
            eval_arg(args[i], [&](std::string const& arg_) -> std::string const& {
                try {
                    return args.at(++i);
                } catch (std::out_of_range const&) {
                    throw std::runtime_error(arg_ + " requires an argument!");
                }
            });
        }
    }
    inline auto
    config() const -> config const& {
        return m_cfg;
    }

private:
    struct matched
    {};
    template<typename T>
    struct option
    {
        T m_flag;
        template<typename Arg, typename Fn>
        auto
        operator()(Arg&& arg_, Fn&& fn_) const -> decltype(*this)& {
            if (arg_ == m_flag) {
                fn_();
                throw matched{};
            }
            return *this;
        }
    };
    struct combined_option
    {
        template<typename Fn>
        auto
        operator()(std::string const& arg_, Fn&& fn_) const -> decltype(*this)& {
            if (arg_[0] == '-') {
                std::for_each(arg_.cbegin() + 1, arg_.cend(), [&](char c_) {
                    try {
                        fn_(c_);
                    } catch (matched) {
                    }
                });
                throw matched{};
            }
            return *this;
        }
    };
    template<typename Fn>
    void
    eval_arg(std::string const& arg_, Fn&& getval_fn_) {
        try {
            make_option(+"--help")(arg_, [&] { print_help(); });
            make_option(+"--xml")(arg_, [&] { m_cfg.report_fmt = config::report_format::XML; });
            make_option(+"--md")(arg_, [&] { m_cfg.report_fmt = config::report_format::MD; });
            make_option(+"--json")(arg_, [&] { m_cfg.report_fmt = config::report_format::JSON; });
            combined_option{}(arg_, [&](char c_) {
                make_option('c')(c_, [&] { m_cfg.report_cfg.color = true; });
                make_option('o')(c_, [&] { m_cfg.report_cfg.capture_out = true; });
                make_option('s')(c_, [&] { m_cfg.report_cfg.strip = true; });
                make_option('e')(c_, [&] {
                    set_filter_mode(config::filter_mode::EXCLUDE);
                    m_cfg.f_patterns.push_back(to_regex(getval_fn_(arg_)));
                });
                make_option('i')(c_, [&] {
                    set_filter_mode(config::filter_mode::INCLUDE);
                    m_cfg.f_patterns.push_back(to_regex(getval_fn_(arg_)));
                });
            });
        } catch (matched) {
            return;
        }
        m_cfg.report_cfg.outfile = arg_;
    }
    template<typename T>
    static auto
    make_option(T&& t_) -> option<T> {
        return option<T>{std::forward<T>(t_)};
    }
    void
    print_help() {
        std::cout
            << "Unit testing binary built with Test++ (" TPP_VERSION ").\n"
            << "GitHub: https://github.com/Jarthianur/TestPlusPlus\n\n"
            << "Usage: " << m_progname << " [OPTIONS] [filename]\n"
            << "Default is to report to standard-out in an informative text format (using console-reporter).\n\n"
               "OPTIONS:\n"
               "  --help: Print this message and exit.\n"
               "  --xml : Report in JUnit-like XML format.\n"
               "  --md  : Report in markdown format.\n"
               "  --json: Report in json format.\n"
               "  -c    : Use ANSI colors in report, if supported by reporter.\n"
               "  -s    : Strip unnecessary whitespaces from report.\n"
               "  -o    : Report captured output from tests, if supported by reporter.\n\n"
               "  Multiple filters are possible, but includes and excludes are mutually exclusive.\n"
               "  Patterns may contain * as wildcard.\n\n"
               "  -e <pattern> : Exclude testsuites with names matching pattern.\n"
               "  -i <pattern> : Include only testsuites with names matching pattern."
            << std::endl;
        throw help_called{};
    }
    static auto
    tokenize_args(std::size_t argc_, char const** argv_) -> std::vector<std::string> {
        if (argc_ == 0) {
            throw std::runtime_error("Too few arguments!");
        }
        std::vector<std::string> a;
        a.reserve(argc_);
        for (auto i{0UL}; i < argc_; ++i) {
            std::string arg(argv_[i]);
            if (!arg.empty()) {
                a.push_back(std::move(arg));
            }
        }
        return a;
    }
    void
    set_filter_mode(config::filter_mode m_) {
        if (m_cfg.f_mode == config::filter_mode::NONE) {
            m_cfg.f_mode = m_;
        } else if (m_cfg.f_mode != m_) {
            throw std::runtime_error("Inclusion and exclusion are mutually exclusive!");
        }
    }
    static auto
    to_regex(std::string const& str_) -> std::regex {
        try {
            return std::regex(std::regex_replace(str_, std::regex("\\*"), ".*"),
                              std::regex_constants::nosubs | std::regex_constants::basic);
        } catch (std::regex_error const&) {
            throw std::runtime_error(str_ + " is not a valid pattern!");
        }
    }
    struct config m_cfg;
    char const*   m_progname{nullptr};
};
}  // namespace intern
}  // namespace tpp
#endif
#ifndef TPP_RUNNER_HPP
#    define TPP_RUNNER_HPP
namespace tpp
{
namespace intern
{
class runner
{
    enum class retval : int
    {
        HELP   = -1,
        EXCEPT = -2
    };

public:
    void
    add_testsuite(test::testsuite_ptr const& ts_) {
        m_testsuites.push_back(ts_);
    }
    auto
    run(int argc_, char const** argv_) noexcept -> int {
        cmdline_parser cmd;
        try {
            if (argc_ < 0) {
                throw std::runtime_error("argument count cannot be less than zero!");
            }
            cmd.parse(static_cast<std::size_t>(argc_), argv_);
        } catch (cmdline_parser::help_called) {
            return to_int(retval::HELP);
        } catch (std::runtime_error const& e) {
            return err_exit(e.what());
        }
        return run(cmd.config());
    }
    auto
    run(config const& cfg_) noexcept -> int {
        bool const fm_inc{cfg_.f_mode != config::filter_mode::EXCLUDE};
        try {
            auto rep{cfg_.reporter()};
            rep->begin_report();
            std::for_each(m_testsuites.begin(), m_testsuites.end(), [&](test::testsuite_ptr& ts_) {
                bool const match{cfg_.f_patterns.empty() ||
                                 std::any_of(cfg_.f_patterns.cbegin(), cfg_.f_patterns.cend(),
                                             [&](std::regex const& re_) -> bool {
                                                 return std::regex_match(ts_->name(), re_);
                                             })};
                if (fm_inc == match) {
                    ts_->run();
                    rep->report(ts_);
                }
            });
            rep->end_report();
            return static_cast<int>(
                std::min(rep->faults(), static_cast<std::size_t>(std::numeric_limits<int>::max())));
        } catch (std::runtime_error const& e) {
            return err_exit(e.what());
        }
    }
    static auto
    instance() -> runner& {
        static runner r;
        return r;
    }

private:
    static inline auto
    to_int(retval v_) -> int {
        return static_cast<int>(v_);
    }
    static inline auto
    err_exit(char const* msg_) -> int {
        std::cerr << "A fatal error occurred!\n  what(): " << msg_ << std::endl;
        return to_int(retval::EXCEPT);
    }
    std::vector<test::testsuite_ptr> m_testsuites;
};
}  // namespace intern
using runner = intern::runner;
}  // namespace tpp
#endif
#ifndef TPP_API_HPP
#    define TPP_API_HPP
namespace tpp
{
namespace intern
{
template<typename T>
struct singleton final
{
    template<typename... Args>
    static auto
    instance(Args&&... args_) noexcept -> T const& {
        static T const inst(std::forward<Args>(args_)...);
        return inst;
    }
};
}  // namespace intern
}  // namespace tpp
#    define TPP_INTERN_CONCAT3(A, B, C) A##B##C
#    define TPP_INTERN_API_TEST_NAME(ID) TPP_INTERN_CONCAT3(tpp_intern_test_, ID, _)
#    define TPP_INTERN_API_TEST_INST(ID) TPP_INTERN_CONCAT3(tpp_intern_test_, ID, _inst_)
#    define TPP_INTERN_API_TEST_FN(ID) TPP_INTERN_CONCAT3(tpp_intern_test_fn_, ID, _)
#    define TPP_INTERN_API_SUITE_NS(ID) TPP_INTERN_CONCAT3(tpp_intern_ns_, ID, _)
#    define TPP_INTERN_API_SUITE_NAME(ID) TPP_INTERN_CONCAT3(tpp_intern_suite_, ID, _)
#    define TPP_INTERN_API_SUITE_WRAPPER(DESCR, BASE)                                                      \
        namespace TPP_INTERN_API_SUITE_NS(__LINE__) {                                                      \
            class test_module                                                                              \
            {                                                                                              \
                tpp::intern::test::testsuite_ptr m_ts_;                                                    \
                                                                                                           \
            public:                                                                                        \
                test_module(test_module const&)     = delete;                                              \
                test_module(test_module&&) noexcept = delete;                                              \
                virtual ~test_module() noexcept     = default;                                             \
                auto                                                                                       \
                operator=(test_module const&) -> test_module& = delete;                                    \
                auto                                                                                       \
                operator=(test_module&&) noexcept -> test_module& = delete;                                \
                                                                                                           \
            protected:                                                                                     \
                test_module() : m_ts_(tpp::intern::test::BASE::create(DESCR)) {                            \
                    tpp::runner::instance().add_testsuite(m_ts_);                                          \
                }                                                                                          \
                auto                                                                                       \
                tpp_intern_ts_() const -> tpp::intern::test::testsuite_ptr const& {                        \
                    return m_ts_;                                                                          \
                }                                                                                          \
            };                                                                                             \
            class TPP_INTERN_API_SUITE_NAME(__LINE__);                                                     \
            using tpp_intern_mod_type_         = TPP_INTERN_API_SUITE_NAME(__LINE__);                      \
            static auto const& tpp_intern_mod_ = tpp::intern::singleton<tpp_intern_mod_type_>::instance(); \
        }                                                                                                  \
        class TPP_INTERN_API_SUITE_NS(__LINE__)::TPP_INTERN_API_SUITE_NAME(__LINE__)                       \
            : public TPP_INTERN_API_SUITE_NS(__LINE__)::test_module
#    define TPP_INTERN_API_TEST_WRAPPER(DESCR)                                                          \
        class TPP_INTERN_API_TEST_NAME(__LINE__)                                                        \
        {                                                                                               \
        public:                                                                                         \
            explicit TPP_INTERN_API_TEST_NAME(__LINE__)(tpp_intern_mod_type_ * mod_) {                  \
                mod_->tpp_intern_ts_()->test(DESCR, [=] { mod_->TPP_INTERN_API_TEST_FN(__LINE__)(); }); \
            }                                                                                           \
        } TPP_INTERN_API_TEST_INST(__LINE__){this};                                                     \
        void TPP_INTERN_API_TEST_FN(__LINE__)()
#    define TPP_INTERN_API_FN_WRAPPER(FN)                                           \
        class tpp_intern_##FN##_                                                    \
        {                                                                           \
        public:                                                                     \
            explicit tpp_intern_##FN##_(tpp_intern_mod_type_* mod_) {               \
                mod_->tpp_intern_ts_()->FN([=] { mod_->tpp_intern_##FN##_fn_(); }); \
            }                                                                       \
        } tpp_intern_##FN##_inst_{this};                                            \
        void tpp_intern_##FN##_fn_()
#    define SUITE(DESCR) TPP_INTERN_API_SUITE_WRAPPER(DESCR, testsuite)
#    define SUITE_PAR(DESCR) TPP_INTERN_API_SUITE_WRAPPER(DESCR, testsuite_parallel)
#    define DESCRIBE(DESCR) SUITE(DESCR)
#    define DESCRIBE_PAR(DESCR) SUITE_PAR(DESCR)
#    define TEST(DESCR) TPP_INTERN_API_TEST_WRAPPER(DESCR)
#    define IT(DESCR) TPP_INTERN_API_TEST_WRAPPER("It " DESCR)
#    define BEFORE_EACH() TPP_INTERN_API_FN_WRAPPER(before_each)
#    define AFTER_EACH() TPP_INTERN_API_FN_WRAPPER(after_each)
#    define SETUP() TPP_INTERN_API_FN_WRAPPER(setup)
#    define TEARDOWN() TPP_INTERN_API_FN_WRAPPER(teardown)
#endif
#ifndef TPP_TPP_HPP
#    define TPP_TPP_HPP
#    define TPP_DEFAULT_MAIN                                  \
        auto main(int argc_, char const** argv_)->int {       \
            return tpp::runner::instance().run(argc_, argv_); \
        }
#endif
#endif
