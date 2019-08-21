/*
 Copyright_License {

 Copyright (C) 2017 Julian P. Becht
 Author: Julian P. Becht

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
#ifndef SCTF_RELEASE_SCTF_HPP
#define SCTF_RELEASE_SCTF_HPP
#include <algorithm>
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
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#ifndef SCTF_COMMON_TYPES_HPP
#define SCTF_COMMON_TYPES_HPP
namespace sctf {
namespace _ {
class abstract_reporter;
using test_function = std::function<void()>;
struct location final {
const char* file;
const int line;
};
template<typename T> struct singleton final {
static T& instance() {
static T instance;
return instance;
}
};
}// namespace _
using reporter_shared = std::shared_ptr<_::abstract_reporter>;
class testsuite;
using testsuite_shared = std::shared_ptr<testsuite>;
}// namespace sctf
#endif// SCTF_COMMON_TYPES_HPP
#ifndef SCTF_COMMON_TRAITS_HPP
#define SCTF_COMMON_TRAITS_HPP
#define ENABLE_IF(C) typename std::enable_if<C>::type* = nullptr
#define AND &&
#define OR ||
#define NOT !
#define IS(T, ...) (T<__VA_ARGS__>::value)
#define IS_TYPE(T, R) (IS(std::is_same, T, R))
#define IS_FLOAT(T) (IS(std::is_floating_point, T))
#define IS_STREAMABLE(T, S) (IS(sctf::_::is_streamable, S, T))
#define IS_ITERABLE(T) (IS(sctf::_::is_iterable, T))
#define IS_ORDINAL(T) (IS(sctf::_::is_ordinal, T))
#define IS_EQUAL_COMPARABLE(T, R) (IS(sctf::_::is_equal_comparable, T, R))
#define IS_UNEQUAL_COMPARABLE(T, R) (IS(sctf::_::is_unequal_comparable, T, R))
namespace sctf { namespace _ {
template<typename S, typename T> class is_streamable {
template<typename SS, typename TT> static auto test(int) -> decltype(std::declval<SS&>() << std::declval<TT>(), std::true_type());
template<typename, typename> static auto test(...) -> std::false_type;
public:
static const bool value = decltype(test<S, T>(0))::value;
};
template<typename T> class is_iterable {
template<typename TT> static auto test(int) -> decltype(std::begin(std::declval<TT&>()) != std::end(std::declval<TT&>()), void(), ++std::declval<decltype(std::begin(std::declval<TT&>()))&>(), void(*std::begin(std::declval<TT&>())), std::true_type());
template<typename> static auto test(...) -> std::false_type;
public:
static const bool value = decltype(test<T>(0))::value;
};
template<typename T> class is_ordinal {
template<typename TT> static auto test(int) -> decltype(std::declval<TT>() < std::declval<TT>(), std::declval<TT>() > std::declval<TT>(), std::true_type());
template<typename> static auto test(...) -> std::false_type;
public:
static const bool value = decltype(test<T>(0))::value;
};
template<typename S, typename T> class is_equal_comparable {
template<typename SS, typename TT> static auto test(int) -> decltype(std::declval<SS>() == std::declval<TT>(), std::true_type());
template<typename, typename> static auto test(...) -> std::false_type;
public:
static const bool value = decltype(test<S, T>(0))::value;
};
template<typename S, typename T> class is_unequal_comparable {
template<typename SS, typename TT> static auto test(int) -> decltype(std::declval<SS>() != std::declval<TT>(), std::true_type());
template<typename, typename> static auto test(...) -> std::false_type;
public:
static const bool value = decltype(test<S, T>(0))::value;
};
}}// namespace sctf::_
#endif// SCTF_COMMON_TRAITS_HPP
#ifndef SCTF_COMMON_DURATION_HPP
#define SCTF_COMMON_DURATION_HPP
namespace sctf { namespace _ {
struct duration final {
duration() : _start(std::chrono::steady_clock::now()) {}
~duration() noexcept {}
double get() { return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - _start).count(); }
protected:
const std::chrono::steady_clock::time_point _start;
};
}}// namespace sctf::_
#endif// SCTF_COMMON_DURATION_HPP
#ifndef SCTF_COMMON_STRINGIFY_HPP
#define SCTF_COMMON_STRINGIFY_HPP
namespace sctf { namespace _ {
template<typename T> static const char* name_for_type() {
static thread_local std::string name;
if(name.length() > 0) { return name.c_str(); }
#if defined(__GNUG__) || defined(__clang__)
const std::string sig(__PRETTY_FUNCTION__);
const auto b = sig.rfind("T = ") + 4;
name = sig.substr(b, sig.rfind(']') - b);
name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
#else
const std::string sig(typeid(T).name());
auto b = sig.find("struct ");
if(b != std::string::npos) {
name = sig.substr(b + 7);
return name.c_str();
}
b = sig.find("class ");
if(b != std::string::npos) {
name = sig.substr(b + 6);
} else {
name = std::move(sig);
}
#endif
return name.c_str();
}
template<typename T, ENABLE_IF(IS_STREAMABLE(T, std::ostringstream) AND NOT IS_FLOAT(T))> std::string to_string(const T& arg) {
std::ostringstream oss;
oss << arg;
return oss.str();
}
template<typename T, ENABLE_IF(IS_STREAMABLE(T, std::ostringstream) AND IS_FLOAT(T))> std::string to_string(const T& arg) {
std::ostringstream oss;
oss << std::setprecision(std::numeric_limits<T>::max_digits10) << arg;
return oss.str();
}
template<typename T, ENABLE_IF(NOT IS_STREAMABLE(T, std::ostringstream))> std::string to_string(const T&) { return name_for_type<T>(); }
inline std::string to_string(const std::nullptr_t&) { return "0"; }
inline std::string to_string(const bool& arg) { return arg ? "true" : "false"; }
}}// namespace sctf::_
#endif// SCTF_COMMON_STRINGIFY_HPP
#ifndef SCTF_COMMON_STREAMBUF_PROXY_OMP_HPP
#define SCTF_COMMON_STREAMBUF_PROXY_OMP_HPP
#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_max_threads() 1
#define omp_get_thread_num() 0
#endif
namespace sctf { namespace _ {
class streambuf_proxy_omp : public std::streambuf {
#define CURRENT_THREAD_BUFFER() (m_thd_buffers.at(static_cast<std::size_t>(omp_get_thread_num())))
public:
streambuf_proxy_omp(std::ostream& stream) : m_orig_buf(stream.rdbuf(this)), m_orig_stream(stream), m_thd_buffers(static_cast<std::size_t>(omp_get_max_threads())) {}
virtual ~streambuf_proxy_omp() noexcept override { m_orig_stream.rdbuf(m_orig_buf); }
std::string str() const { return CURRENT_THREAD_BUFFER().str(); }
void clear() { CURRENT_THREAD_BUFFER().str(""); }
protected:
virtual int_type overflow(int_type c) override { return CURRENT_THREAD_BUFFER().sputc(std::stringbuf::traits_type::to_char_type(c)); }
virtual std::streamsize xsputn(const char* s, std::streamsize n) override { return CURRENT_THREAD_BUFFER().sputn(s, n); }
std::streambuf* m_orig_buf;
std::ostream& m_orig_stream;
std::vector<std::stringbuf> m_thd_buffers;
};
}}// namespace sctf::_
#ifndef _OPENMP
#undef omp_get_max_threads
#undef omp_get_thread_num
#endif
#undef CURRENT_THREAD_BUFFER
#endif// SCTF_COMMON_STREAMBUF_PROXY_OMP_HPP
#ifndef SCTF_COMMON_STREAMBUF_PROXY_HPP
#define SCTF_COMMON_STREAMBUF_PROXY_HPP
namespace sctf { namespace _ {
class streambuf_proxy : public std::streambuf {
public:
streambuf_proxy(std::ostream& stream) : m_orig_buf(stream.rdbuf(this)), m_orig_stream(stream) {}
virtual ~streambuf_proxy() noexcept override { m_orig_stream.rdbuf(m_orig_buf); }
std::string str() const { return m_buffer.str(); }
void clear() { m_buffer.str(""); }
protected:
virtual int_type overflow(int_type c) override { return m_buffer.sputc(std::stringbuf::traits_type::to_char_type(c)); }
virtual std::streamsize xsputn(const char* s, std::streamsize n) override { return m_buffer.sputn(s, n); }
std::streambuf* m_orig_buf;
std::ostream& m_orig_stream;
std::stringbuf m_buffer;
};
}}// namespace sctf::_
#endif// SCTF_COMMON_STREAMBUF_PROXY_HPP
#ifndef SCTF_COMMON_ASSERTION_FAILURE_HPP
#define SCTF_COMMON_ASSERTION_FAILURE_HPP
namespace sctf { namespace _ {
class assertion_failure : public std::exception {
public:
assertion_failure(const std::string& msg, const location& loc) : std::exception(), m_msg(msg + " at " + loc.file + ":" + std::to_string(loc.line)) {}
virtual ~assertion_failure() noexcept override = default;
inline const char* what() const noexcept override { return m_msg.c_str(); }
protected:
const std::string m_msg;
};
}}// namespace sctf::_
#endif// SCTF_COMMON_ASSERTION_FAILURE_HPP
#ifndef SCTF_TESTSUITE_STATISTICS_HPP
#define SCTF_TESTSUITE_STATISTICS_HPP
namespace sctf {
class testsuite;
class testsuite_parallel;
namespace _ {
class statistics {
public:
statistics() = default;
~statistics() noexcept = default;
inline std::size_t tests() const { return m_num_of_tests; }
inline std::size_t successes() const { return m_num_of_tests - m_num_of_errs - m_num_of_fails; }
inline std::size_t failures() const { return m_num_of_fails; }
inline std::size_t errors() const { return m_num_of_errs; }
protected:
friend sctf::testsuite;
friend sctf::testsuite_parallel;
std::size_t m_num_of_tests = 0;
std::size_t m_num_of_fails = 0;
std::size_t m_num_of_errs = 0;
};
}// namespace _
}// namespace sctf
#endif// SCTF_TESTSUITE_STATISTICS_HPP
#ifndef SCTF_TESTSUITE_TESTCASE_HPP
#define SCTF_TESTSUITE_TESTCASE_HPP
namespace sctf { namespace _ {
class testcase {
public:
testcase(const testcase&) = delete;
testcase& operator=(const testcase&) = delete;
~testcase() noexcept = default;
testcase(const char* name, const char* context, test_function&& t_func) : m_name(name), m_context(context), m_test_func(std::move(t_func)) {}
testcase(testcase&& other) : m_name(other.m_name), m_context(other.m_context), m_state(other.m_state), m_duration(other.m_duration), m_err_msg(std::move(other.m_err_msg)), m_test_func(std::move(other.m_test_func)) {}
testcase& operator=(testcase&& other) {
m_name = other.m_name;
m_context = other.m_context;
m_state = other.m_state;
m_duration = other.m_duration;
m_err_msg = std::move(other.m_err_msg);
m_test_func = std::move(other.m_test_func);
return *this;
}
enum class result : std::int_fast8_t { NONE, PASSED, FAILED, ERROR };
void operator()() {
if(m_state != result::NONE) return;
struct duration dur;
try {
m_test_func();
pass();
} catch(const assertion_failure& e) { fail(e.what()); } catch(const std::exception& e) {
erroneous(e.what());
} catch(...) { erroneous(); }
m_duration = dur.get();
}
inline result state() const { return m_state; }
inline double duration() const { return m_duration; }
inline const std::string& err_msg() const { return m_err_msg; }
inline const char* name() const { return m_name; }
inline const char* context() const { return m_context; }
inline void set_cout(const std::string& str) { m_cout = str; }
inline void set_cerr(const std::string& str) { m_cerr = str; }
inline const std::string& cout() const { return m_cout; }
inline const std::string& cerr() const { return m_cerr; }
private:
inline void pass() { m_state = result::PASSED; }
inline void fail(const char* msg) {
m_state = result::FAILED;
m_err_msg = msg;
}
inline void erroneous(const char* error = "unknown error") {
m_state = result::ERROR;
m_err_msg = error;
}
const char* m_name;
const char* m_context;
result m_state = result::NONE;
double m_duration = 0.0;
std::string m_err_msg;
std::string m_cout;
std::string m_cerr;
test_function m_test_func;
};
}}// namespace sctf::_
#endif// SCTF_TESTSUITE_TESTCASE_HPP
#ifndef SCTF_TESTSUITE_TESTSUITE_HPP
#define SCTF_TESTSUITE_TESTSUITE_HPP
#define SCTF_EXEC_SILENT(F) \
if(F) { \
try { \
F(); \
} catch(...) {} \
}
namespace sctf {
class testsuite : public std::enable_shared_from_this<testsuite> {
public:
virtual ~testsuite() noexcept = default;
static testsuite_shared create(const char* name, const char* context) { return testsuite_shared(new testsuite(name, context)); }
virtual void run() {
if(m_state != execution_state::DONE) {
m_stats.m_num_of_tests = m_testcases.size();
_::streambuf_proxy buf_cout(std::cout);
_::streambuf_proxy buf_cerr(std::cerr);
SCTF_EXEC_SILENT(m_setup_func)
std::for_each(m_testcases.begin(), m_testcases.end(), [this, &buf_cerr, &buf_cout](_::testcase& tc) {
if(tc.state() == _::testcase::result::NONE) {
SCTF_EXEC_SILENT(m_pre_test_func)
tc();
switch(tc.state()) {
case _::testcase::result::FAILED: ++m_stats.m_num_of_fails; break;
case _::testcase::result::ERROR: ++m_stats.m_num_of_errs; break;
default: break;
}
m_time += tc.duration();
SCTF_EXEC_SILENT(m_post_test_func)
tc.set_cout(buf_cout.str());
tc.set_cerr(buf_cerr.str());
buf_cout.clear();
buf_cerr.clear();
}
});
m_state = execution_state::DONE;
}
}
template<typename T> testsuite_shared test(const char* name, _::test_function&& t_func) {
m_testcases.push_back(_::testcase(name, _::name_for_type<T>(), std::move(t_func)));
m_state = execution_state::PENDING;
return shared_from_this();
}
testsuite_shared test(const char* name, const char* context, _::test_function&& t_func) {
m_testcases.push_back(_::testcase(name, context, std::move(t_func)));
m_state = execution_state::PENDING;
return shared_from_this();
}
testsuite_shared test(const char* name, _::test_function&& t_func) {
m_testcases.push_back(_::testcase(name, m_context, std::move(t_func)));
m_state = execution_state::PENDING;
return shared_from_this();
}
testsuite_shared setup(_::test_function&& t_func) {
m_setup_func = std::move(t_func);
return shared_from_this();
}
testsuite_shared before(_::test_function&& t_func) {
m_pre_test_func = std::move(t_func);
return shared_from_this();
}
testsuite_shared after(_::test_function&& t_func) {
m_post_test_func = std::move(t_func);
return shared_from_this();
}
inline const char* name() const { return m_name; }
inline const std::chrono::system_clock::time_point& timestamp() const { return m_timestamp; }
inline const _::statistics& statistics() const { return m_stats; }
inline double time() const { return m_time; }
inline const std::vector<_::testcase>& testcases() const { return m_testcases; }
protected:
enum class execution_state : std::int_fast8_t { PENDING, DONE };
testsuite(const char* name, const char* context) : m_name(name), m_context(context), m_timestamp(std::chrono::system_clock::now()) {}
const char* m_name;
const char* m_context;
const std::chrono::system_clock::time_point m_timestamp;
double m_time = 0.0;
_::statistics m_stats;
std::vector<_::testcase> m_testcases;
execution_state m_state = execution_state::PENDING;
_::test_function m_setup_func;
_::test_function m_pre_test_func;
_::test_function m_post_test_func;
};
}// namespace sctf
#endif// SCTF_TESTSUITE_TESTSUITE_HPP
#ifndef SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
#define SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
namespace sctf {
class testsuite_parallel : public testsuite {
public:
~testsuite_parallel() noexcept override = default;
static testsuite_shared create(const char* name, const char* context) { return testsuite_shared(new testsuite_parallel(name, context)); }
void run() override {
if(m_state != execution_state::DONE) {
if(m_testcases.size() > std::numeric_limits<long>::max()) { throw std::overflow_error("Too many testcases! Size would overflow loop variant."); }
const long tc_size = static_cast<long>(m_testcases.size());
m_stats.m_num_of_tests = m_testcases.size();
_::streambuf_proxy_omp mt_buf_cout(std::cout);
_::streambuf_proxy_omp mt_buf_cerr(std::cerr);
SCTF_EXEC_SILENT(m_setup_func)
#pragma omp parallel
{
double tmp = 0.0;
std::size_t fails = 0;
std::size_t errs = 0;
// OpenMP 2 compatible - MSVC not supporting higher version
#pragma omp for schedule(dynamic)
for(long i = 0; i < tc_size; ++i) {
auto& tc = m_testcases[static_cast<std::size_t>(i)];
if(tc.state() == _::testcase::result::NONE) {
SCTF_EXEC_SILENT(m_pre_test_func)
tc();
switch(tc.state()) {
case _::testcase::result::FAILED: ++fails; break;
case _::testcase::result::ERROR: ++errs; break;
default: break;
}
tmp += tc.duration();
SCTF_EXEC_SILENT(m_post_test_func)
tc.set_cout(mt_buf_cout.str());
tc.set_cerr(mt_buf_cerr.str());
mt_buf_cout.clear();
mt_buf_cerr.clear();
}
}
#pragma omp critical
{
m_stats.m_num_of_fails += fails;
m_stats.m_num_of_errs += errs;
m_time = std::max(m_time, tmp);
}
}
m_state = execution_state::DONE;
}
}
private:
testsuite_parallel(const char* name, const char* context) : testsuite(name, context) {}
};
}// namespace sctf
#endif// SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
#ifndef SCTF_TESTSUITE_RUNNER_HPP
#define SCTF_TESTSUITE_RUNNER_HPP
namespace sctf {
class runner {
public:
runner() = default;
~runner() noexcept = default;
testsuite_shared register_ts(testsuite_shared ts) {
m_testsuites.push_back(ts);
return ts;
}
void run() noexcept {
std::for_each(m_testsuites.begin(), m_testsuites.end(), [](testsuite_shared& ts) { ts->run(); });
}
const std::vector<testsuite_shared>& testsuites() { return m_testsuites; }
static runner& default_instance() {
static runner r;
return r;
}
private:
std::vector<testsuite_shared> m_testsuites;
};
inline static testsuite_shared describeParallel(const char* name, const char* context = "main", runner& runner = runner::default_instance()) { return runner.register_ts(testsuite_parallel::create(name, context)); }
template<typename T> static testsuite_shared describeParallel(const char* name, runner& runner = runner::default_instance()) { return runner.register_ts(testsuite_parallel::create(name, _::name_for_type<T>())); }
inline static testsuite_shared describe(const char* name, const char* context = "main", runner& runner = runner::default_instance()) { return runner.register_ts(testsuite::create(name, context)); }
template<typename T> static testsuite_shared describe(const char* name, runner& runner = runner::default_instance()) { return runner.register_ts(testsuite::create(name, _::name_for_type<T>())); }
}// namespace sctf
#endif// SCTF_TESTSUITE_RUNNER_HPP
#ifndef SCTF_TESTSUITE_TESTMODULE_HPP
#define SCTF_TESTSUITE_TESTMODULE_HPP
namespace sctf { namespace _ {
class test_module {
protected:
test_module(const testsuite_shared& ts) : m_ts(ts) {}
virtual ~test_module() noexcept = default;
inline void test(const char* name, _::test_function&& fn) { m_ts->test(name, std::move(fn)); }
inline void setup(_::test_function&& fn) { m_ts->setup(std::move(fn)); }
inline void before(_::test_function&& fn) { m_ts->before(std::move(fn)); }
inline void after(_::test_function&& fn) { m_ts->after(std::move(fn)); }
testsuite_shared m_ts;
};
}}// namespace sctf::_
#define TEST_MODULE(NAME, FN, ...) \
class NAME : public sctf::_::test_module { \
public: \
NAME() : sctf::_::test_module(sctf::describe(#NAME, #NAME, ##__VA_ARGS__)) { FN; } \
~NAME() noexcept override = default; \
}; \
namespace sctf { namespace _ { \
static const auto& mods_##NAME = singleton<NAME>::instance(); \
} \
}
#define TEST_MODULE_PARALLEL(NAME, FN, ...) \
class NAME : public sctf::_::test_module { \
public: \
NAME() : sctf::_::test_module(sctf::describeParallel(#NAME, #NAME, ##__VA_ARGS__)) { FN; } \
~NAME() noexcept override = default; \
}; \
namespace sctf { namespace _ { \
static const auto& modp_##NAME = singleton<NAME>::instance(); \
} \
}
#endif// SCTF_TESTSUITE_TESTMODULE_HPP
#ifndef SCTF_REPORTER_ABSTRACT_REPORTER_HPP
#define SCTF_REPORTER_ABSTRACT_REPORTER_HPP
#ifdef _WIN32
#define SCTF_LF "\r\n"
#else
#define SCTF_LF "\n"
#endif
#define SCTF_SPACE "  "
#define SCTF_XSPACE "    "
namespace sctf { namespace _ {
class abstract_reporter {
public:
virtual ~abstract_reporter() noexcept = default;
std::size_t report(runner& runner = runner::default_instance()) {
m_abs_errs = 0;
m_abs_fails = 0;
m_abs_tests = 0;
m_abs_time = 0.0;
runner.run();
begin_report();
std::for_each(runner.testsuites().begin(), runner.testsuites().end(), [this](const testsuite_shared& ts) {
m_abs_errs += ts->statistics().errors();
m_abs_fails += ts->statistics().failures();
m_abs_tests += ts->statistics().tests();
m_abs_time += ts->time();
report_ts(ts);
});
end_report();
return m_abs_errs + m_abs_fails;
}
protected:
explicit abstract_reporter(std::ostream& stream) : mr_out_stream(stream) {}
explicit abstract_reporter(const char* fname) : m_out_file(fname), mr_out_stream(m_out_file) {
if(!mr_out_stream) { throw std::runtime_error("Could not open file."); }
}
inline virtual void report_ts(const testsuite_shared ts) {
std::for_each(ts->testcases().begin(), ts->testcases().end(), [this](const _::testcase& tc) { report_tc(tc); });
}
virtual void report_tc(const testcase& tc) = 0;
virtual void begin_report() = 0;
virtual void end_report() = 0;
template<typename T> std::ostream& operator<<(const T& _1) {
mr_out_stream << _1;
return mr_out_stream;
}
std::ofstream m_out_file;
std::ostream& mr_out_stream;
std::size_t m_abs_tests = 0;
std::size_t m_abs_fails = 0;
std::size_t m_abs_errs = 0;
double m_abs_time = 0;
};
}}// namespace sctf::_
#endif// SCTF_REPORTER_ABSTRACT_REPORTER_HPP
#ifndef SCTF_REPORTER_XML_REPORTER_HPP
#define SCTF_REPORTER_XML_REPORTER_HPP
namespace sctf {
class xml_reporter : public _::abstract_reporter {
public:
~xml_reporter() noexcept override = default;
explicit xml_reporter(std::ostream& stream) : abstract_reporter(stream) {}
explicit xml_reporter(const char* fname) : abstract_reporter(fname) {}
protected:
void report_ts(const testsuite_shared ts) override {
std::time_t stamp = std::chrono::system_clock::to_time_t(ts->timestamp());
char buff[128];
std::strftime(buff, 127, "%FT%T", std::localtime(&stamp));
*this << SCTF_SPACE << "<testsuite id=\"" << m_id++ << "\" name=\"" << ts->name() << "\" errors=\"" << ts->statistics().errors() << "\" tests=\"" << ts->statistics().tests() << "\" failures=\"" << ts->statistics().failures() << "\" skipped=\"0\" time=\"" << ts->time() << "\" timestamp=\"" << buff << "\">" << SCTF_LF;
abstract_reporter::report_ts(ts);
*this << SCTF_SPACE << "</testsuite>" << SCTF_LF;
}
void report_tc(const _::testcase& tc) override {
*this << SCTF_XSPACE << "<testcase name=\"" << tc.name() << "\" classname=\"" << tc.context() << "\" time=\"" << tc.duration() << "\"";
switch(tc.state()) {
case _::testcase::result::ERROR: *this << ">" << SCTF_LF << SCTF_XSPACE << SCTF_SPACE << "<error message=\"" << tc.err_msg() << "\"></error>" << SCTF_LF << SCTF_XSPACE << "</testcase>"; break;
case _::testcase::result::FAILED: *this << ">" << SCTF_LF << SCTF_XSPACE << SCTF_SPACE << "<failure message=\"" << tc.err_msg() << "\"></failure>" << SCTF_LF << SCTF_XSPACE << "</testcase>"; break;
case _::testcase::result::PASSED: *this << "/>"; break;
default: break;
}
*this << SCTF_LF;
}
void begin_report() override { *this << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << SCTF_LF << "<testsuites>" << SCTF_LF; }
void end_report() override { *this << "</testsuites>" << SCTF_LF; }
mutable std::size_t m_id = 0;
};
static reporter_shared createXmlReporter(std::ostream& stream = std::cout) { return std::make_shared<xml_reporter>(stream); }
static reporter_shared createXmlReporter(const char* file) { return std::make_shared<xml_reporter>(file); }
}// namespace sctf
#endif// SCTF_REPORTER_XML_REPORTER_HPP
#ifndef SCTF_REPORTER_PLAINTEXT_REPORTER_HPP
#define SCTF_REPORTER_PLAINTEXT_REPORTER_HPP
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_RESET "\x1b[0m"
namespace sctf {
class plaintext_reporter : public _::abstract_reporter {
public:
~plaintext_reporter() noexcept override = default;
explicit plaintext_reporter(std::ostream& stream, bool color = false, bool out = false) : abstract_reporter(stream), m_color(color), m_out(out) {}
explicit plaintext_reporter(const char* fname, bool color = false, bool out = false) : abstract_reporter(fname), m_color(color), m_out(out) {}
protected:
void report_ts(const testsuite_shared ts) override {
*this << "Run Testsuite [" << ts->name() << "]; time = " << ts->time() << "ms" << SCTF_LF;
abstract_reporter::report_ts(ts);
}
void report_tc(const _::testcase& tc) override {
*this << SCTF_SPACE << "Run Testcase [" << tc.name() << "](" << tc.context() << "); time = " << tc.duration() << "ms" << SCTF_LF << SCTF_XSPACE;
if(m_out) {
*this << "stdout = '" << tc.cout() << '\'' << SCTF_LF << SCTF_XSPACE;
*this << "stderr = '" << tc.cerr() << '\'' << SCTF_LF << SCTF_XSPACE;
}
switch(tc.state()) {
case _::testcase::result::ERROR: *this << (m_color ? ANSI_MAGENTA : "") << "ERROR! " << tc.err_msg(); break;
case _::testcase::result::FAILED: *this << (m_color ? ANSI_RED : "") << "FAILED! " << tc.err_msg(); break;
case _::testcase::result::PASSED: *this << (m_color ? ANSI_GREEN : "") << "PASSED!"; break;
default: break;
}
*this << (m_color ? ANSI_RESET : "") << SCTF_LF;
}
void begin_report() override {}
void end_report() override {
if(m_abs_fails >= (m_abs_tests + 1) / 2) {
*this << (m_color ? ANSI_YELLOW : "");
} else {
*this << (m_color ? ANSI_CYAN : "");
}
*this << "Result:: passed: " << m_abs_tests - m_abs_fails - m_abs_errs << "/" << m_abs_tests << " ; failed: " << m_abs_fails << "/" << m_abs_tests << " ; errors: " << m_abs_errs << "/" << m_abs_tests << " ; time = " << m_abs_time << "ms" << (m_color ? ANSI_RESET : "") << SCTF_LF;
}
bool m_color;
bool m_out;
};
static reporter_shared createPlainTextReporter(std::ostream& stream, bool color = false, bool out = false) { return std::make_shared<plaintext_reporter>(stream, color, out); }
static reporter_shared createPlainTextReporter(bool color = false, bool out = false) { return std::make_shared<plaintext_reporter>(std::cout, color, out); }
static reporter_shared createPlainTextReporter(const char* file, bool color = false, bool out = false) { return std::make_shared<plaintext_reporter>(file, color, out); }
}// namespace sctf
#undef ANSI_RED
#undef ANSI_BLUE
#undef ANSI_CYAN
#undef ANSI_GREEN
#undef ANSI_RESET
#undef ANSI_YELLOW
#undef ANSI_MAGENTA
#endif// SCTF_REPORTER_PLAINTEXT_REPORTER_HPP
#ifndef SCTF_REPORTER_HTML_REPORTER_HPP
#define SCTF_REPORTER_HTML_REPORTER_HPP
#define TD "<td>"
#define TD_ "</td>"
#define TR "<tr>"
#define TR_ "</tr>"
#define TH "<th>"
#define TH_ "</th>"
namespace sctf {
class html_reporter : public _::abstract_reporter {
public:
~html_reporter() noexcept override = default;
explicit html_reporter(std::ostream& stream) : abstract_reporter(stream) {}
explicit html_reporter(const char* fname) : abstract_reporter(fname) {}
protected:
void report_ts(const testsuite_shared ts) override {
*this << "<h3>" << ts->name() << "</h3>"
      << "<p>Tests: " << ts->statistics().tests() << " Failures: " << ts->statistics().failures() << " Errors: " << ts->statistics().errors() << " Time: " << ts->time() << "ms</p><table><thead>" << TR << TH << "Name" << TH_ << TH << "Context" << TH_ << TH << "Time" << TH_ << TH << "Status" << TH_ << TR_ << "</thead><tbody>";
abstract_reporter::report_ts(ts);
*this << "</tbody></table>";
}
void report_tc(const _::testcase& tc) override {
std::string status;
switch(tc.state()) {
case _::testcase::result::ERROR: status = "error"; break;
case _::testcase::result::FAILED: status = "failed"; break;
case _::testcase::result::PASSED: status = "passed"; break;
default: break;
}
*this << "<tr class=\"" << status << "\">" << TD << tc.name() << TD_ << TD << tc.context() << TD_ << TD << tc.duration() << "ms" << TD_ << TD << status << TD_ << TR_;
}
void begin_report() override {
*this << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"/>"
         "<style>table{border-collapse: collapse;min-width: 50%}"
         "tr,th,td{border: 1px solid black;padding: 2px}.failed{background: lightskyblue}"
         ".passed{background: lightgreen}.error{background: lightcoral}</style>"
         "</head><body><header><h1>Test Report</h1></header>";
}
void end_report() override { *this << "<footer><h3>Summary</h3><p>Tests: " << m_abs_tests << " Failures: " << m_abs_fails << " Errors: " << m_abs_errs << " Time: " << m_abs_time << "ms</p></footer></body></html>"; }
};
static reporter_shared createHtmlReporter(std::ostream& stream = std::cout) { return std::make_shared<html_reporter>(stream); }
static reporter_shared createHtmlReporter(const char* file) { return std::make_shared<html_reporter>(file); }
}// namespace sctf
#undef TD
#undef TD_
#undef TR
#undef TR_
#undef TH
#undef TH_
#endif// SCTF_REPORTER_HTML_REPORTER_HPP
#ifndef SCTF_COMPARATOR_COMPARATORS_HPP
#define SCTF_COMPARATOR_COMPARATORS_HPP
#if __cplusplus >= 201703L
#include <optional>
using std::nullopt;
using std::optional;
#elif __cplusplus >= 201402L
#include <experimental/optional>
using std::experimental::nullopt;
using std::experimental::optional;
#else
#include <cstring>
#endif
namespace sctf { namespace _ {
struct comparison final {
#if __cplusplus >= 201402L
constexpr comparison() : _failure(nullopt) {}
comparison(const char* comp_str, const std::string& value, const std::string& expect) : _failure("Expected '" + value + "' " + comp_str + " '" + expect + "'") {}
explicit operator bool() { return !_failure; }
const std::string& operator*() const { return *_failure; }
protected:
const optional<std::string> _failure;
#else
constexpr comparison() : _success(true) {}
comparison(const char* comp_str, const std::string& value, const std::string& expect) : _success(false) {
std::string msg;
msg.reserve(15 + std::strlen(comp_str) + value.length() + expect.length());
msg = "Expected '";
msg.append(value).append("' ").append(comp_str).append(" '").append(expect).append("'");
error() = msg;
}
explicit operator bool() { return _success; }
const std::string& operator*() const { return error(); }
protected:
const bool _success;
std::string& error() const {
static thread_local std::string err_msg;
return err_msg;
}
#endif
};
template<typename V, typename E = V> using comparator = comparison (*)(const V&, const E&);
#if __cplusplus >= 201402L
#define success comparison()
#else
constexpr comparison success = comparison();
#endif
#ifdef SCTF_EPSILON
static double epsilon = SCTF_EPSILON;
#elif defined(SCTF_EXTERN_EPSILON)
extern double epsilon;
#endif
}}// namespace sctf::_
#define PROVIDE_COMPARATOR(COMP, NAME) \
namespace sctf { \
template<typename V, typename E = V> static _::comparator<V, E> NAME() { return &_::COMP<V, E>; } \
}
#define COMPARATOR(NAME, COMPSTR, PRED) \
namespace sctf { namespace _ { \
constexpr const char* NAME##_comp_str = COMPSTR; \
template<typename V, typename E = V> static comparison NAME(const V& value, const E& expect) { return (PRED) ? success : comparison(NAME##_comp_str, to_string(value), to_string(expect)); } \
} \
}
#define SCTF_SET_EPSILON(E) \
namespace sctf { namespace _ { \
double epsilon = E; \
} \
}
#endif// SCTF_COMPARATOR_COMPARATORS_HPP
#ifndef SCTF_COMPARATOR_LESS_HPP
#define SCTF_COMPARATOR_LESS_HPP
COMPARATOR(less_than, "to be less than", value < expect)
PROVIDE_COMPARATOR(less_than, LESS)
PROVIDE_COMPARATOR(less_than, LT)
#endif// SCTF_COMPARATOR_LESS_HPP
#ifndef SCTF_COMPARATOR_CONTAINS_HPP
#define SCTF_COMPARATOR_CONTAINS_HPP
namespace sctf { namespace _ {
constexpr const char* in_range_comp_str = "to be in range of";
template<typename V, typename R, ENABLE_IF(IS_ITERABLE(R) AND NOT IS_TYPE(R, std::string))> static comparison in_range(const V& value, const R& range) { return std::find(range.begin(), range.end(), value) != range.end() ? success : comparison(in_range_comp_str, to_string(value), to_string(range)); }
template<typename V, typename R = V, ENABLE_IF(IS_TYPE(R, std::string))> comparison in_range(const V& value, const R& range) { return range.find(value) != std::string::npos ? success : comparison(in_range_comp_str, to_string(value), to_string(range)); }
}}// namespace sctf::_
PROVIDE_COMPARATOR(in_range, IN_RANGE)
PROVIDE_COMPARATOR(in_range, IN)
#endif// SCTF_COMPARATOR_CONTAINS_HPP
#ifndef SCTF_COMPARATOR_UNEQUALS_HPP
#define SCTF_COMPARATOR_UNEQUALS_HPP
namespace sctf { namespace _ {
constexpr const char* unequals_comp_str = "to be unequals";
template<typename V, typename E = V, ENABLE_IF(NOT IS_FLOAT(V) AND IS_UNEQUAL_COMPARABLE(V, E))> static comparison unequals(const V& value, const E& expect) { return value != expect ? success : comparison(unequals_comp_str, to_string(value), to_string(expect)); }
template<typename V, typename E = V, ENABLE_IF(NOT IS_FLOAT(V) AND NOT IS_UNEQUAL_COMPARABLE(V, E) AND IS_EQUAL_COMPARABLE(V, E))> static comparison unequals(const V& value, const E& expect) { return value == expect ? comparison(unequals_comp_str, to_string(value), to_string(expect)) : success; }
template<typename V, typename E = V, ENABLE_IF(IS_FLOAT(V) AND IS_FLOAT(E))> comparison unequals(const V& value, const E& expect) {
#if defined(SCTF_EXTERN_EPSILON) || defined(SCTF_EPSILON)
static V epsilon_ = static_cast<V>(epsilon);
#else
static V epsilon_ = std::numeric_limits<V>::epsilon();
#endif
return (std::abs(value - expect) <= std::max(std::abs(value), std::abs(expect)) * epsilon_) ? comparison(unequals_comp_str, to_string(value), to_string(expect)) : success;
}
}}// namespace sctf::_
PROVIDE_COMPARATOR(unequals, UNEQUALS)
PROVIDE_COMPARATOR(unequals, NE)
#endif// SCTF_COMPARATOR_UNEQUALS_HPP
#ifndef SCTF_COMPARATOR_EQUALS_HPP
#define SCTF_COMPARATOR_EQUALS_HPP
namespace sctf { namespace _ {
constexpr const char* equals_comp_str = "to be equals";
template<typename V, typename E = V, ENABLE_IF(NOT IS_FLOAT(V) AND IS_EQUAL_COMPARABLE(V, E))> static comparison equals(const V& value, const E& expect) { return value == expect ? success : comparison(equals_comp_str, to_string(value), to_string(expect)); }
template<typename V, typename E = V, ENABLE_IF(NOT IS_FLOAT(V) AND NOT IS_EQUAL_COMPARABLE(V, E) AND IS_UNEQUAL_COMPARABLE(V, E))> static comparison equals(const V& value, const E& expect) { return value != expect ? comparison(equals_comp_str, to_string(value), to_string(expect)) : success; }
template<typename V, typename E = V, ENABLE_IF(IS_FLOAT(V) AND IS_FLOAT(E))> comparison equals(const V& value, const E& expect) {
#if defined(SCTF_EXTERN_EPSILON) || defined(SCTF_EPSILON)
static V epsilon_ = static_cast<V>(epsilon);
#else
static V epsilon_ = std::numeric_limits<V>::epsilon();
#endif
return (std::abs(value - expect) <= std::max(std::abs(value), std::abs(expect)) * epsilon_) ? success : comparison(equals_comp_str, to_string(value), to_string(expect));
}
}}// namespace sctf::_
PROVIDE_COMPARATOR(equals, EQUALS)
PROVIDE_COMPARATOR(equals, EQ)
#endif// SCTF_COMPARATOR_EQUALS_HPP
#ifndef SCTF_COMPARATOR_GREATER_HPP
#define SCTF_COMPARATOR_GREATER_HPP
COMPARATOR(greater_than, "to be greater than", value > expect)
PROVIDE_COMPARATOR(greater_than, GREATER)
PROVIDE_COMPARATOR(greater_than, GT)
#endif// SCTF_COMPARATOR_GREATER_HPP
#ifndef SCTF_ASSERT_HPP
#define SCTF_ASSERT_HPP
// disable assert macro
#ifdef assert
#undef assert
#endif
// provide assertion macros and wrappers
#define assert(VALUE, COMP, EXPECT) sctf::_::_assertStatement(VALUE, EXPECT, COMP<decltype(VALUE), decltype(EXPECT)>(), sctf::_::location{__FILE__, __LINE__})
#define assertT(VALUE, COMP, EXPECT, TYPE) sctf::_::_assertStatement<TYPE, TYPE>(VALUE, EXPECT, COMP<TYPE, TYPE>(), sctf::_::location{__FILE__, __LINE__})
#define assertEquals(VALUE, EXPECT) assertT(VALUE, EQUALS, EXPECT, decltype(VALUE))
#define assertTrue(VALUE) sctf::_::_assertStatement<bool>(VALUE, true, sctf::EQUALS<bool>(), sctf::_::location{__FILE__, __LINE__})
#define assertFalse(VALUE) sctf::_::_assertStatement<bool>(VALUE, false, sctf::EQUALS<bool>(), sctf::_::location{__FILE__, __LINE__})
#define assertNotNull(VALUE) sctf::_::_assertStatement(static_cast<void* const>(VALUE), nullptr, sctf::UNEQUALS<void* const, std::nullptr_t>(), sctf::_::location{__FILE__, __LINE__})
#define assertZero(VALUE) sctf::_::_assertStatement(VALUE, static_cast<decltype(VALUE)>(0), sctf::EQUALS<decltype(VALUE)>(), sctf::_::location{__FILE__, __LINE__})
#define assertException(FUNC, TYPE) sctf::_::_assertException<TYPE>([&] { FUNC; }, sctf::_::location{__FILE__, __LINE__})
#define assertNoExcept(FUNC) sctf::_::_assertNoExcept([&] { FUNC; }, sctf::_::location{__FILE__, __LINE__})
#define assertPerformance(FUNC, MILLIS) sctf::_::_assertPerformance([&] { FUNC; }, MILLIS, sctf::_::location{__FILE__, __LINE__})
namespace sctf { namespace _ {
template<typename V, typename E = V> static void _assertStatement(const V& value, const E& expect, comparator<V, E> comp, const location& loc) {
comparison res = (*comp)(value, expect);
if(!res) { throw assertion_failure(*res, loc); }
}
template<typename T> static void _assertException(const test_function& func, const location& loc) {
try {
func();
} catch(const T&) { return; } catch(const std::exception& e) {
throw assertion_failure(std::string("Wrong exception thrown, caught '") + to_string(e) + "'", loc);
} catch(...) { throw assertion_failure("Wrong exception thrown", loc); }
throw assertion_failure(std::string("No exception thrown, expected '") + name_for_type<T>() + "'", loc);
}
static void _assertNoExcept(const test_function& func, const location& loc) {
try {
func();
} catch(const std::exception& e) { throw assertion_failure(std::string("Expected no exception, caught '") + to_string(e) + "'", loc); } catch(...) {
throw assertion_failure("Expected no exception", loc);
}
}
static void _assertPerformance(const test_function& func, double max_millis, const location& loc) {
try {
duration dur;
func();
double dur_ms = dur.get();
if(dur_ms > max_millis) { throw assertion_failure(std::string("runtime > ") + to_string(max_millis) + "ms", loc); }
} catch(const std::exception& e) { throw assertion_failure(e.what(), loc); } catch(...) {
throw assertion_failure("Unknown exception thrown", loc);
}
}
}}// namespace sctf::_
#endif// SCTF_ASSERT_HPP
#ifndef SCTF_SCTF_HPP
#define SCTF_SCTF_HPP
#define SCTF_DEFAULT_MAIN(REPORTER, ...) \
int main(int, char**) { \
__VA_ARGS__; \
return static_cast<int>(sctf::REPORTER->report()); \
}
#endif// SCTF_SCTF_HPP
#endif
