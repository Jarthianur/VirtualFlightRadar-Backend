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
#include <regex>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#ifndef SCTF_CPP_META_HPP
#define SCTF_CPP_META_HPP
#if __cplusplus >= 201703L
#define SCTF_CPP_V17
#elif __cplusplus >= 201402L
#define SCTF_CPP_V14
#elif __cplusplus >= 201103L
#define SCTF_CPP_V11
#else
#error SCTF requires at least full C++11 compliance
#endif
#if defined(__GNUG__) || defined(__clang__)
#define SCTF_SYS_UNIX
#elif defined(_WIN32)
#define SCTF_SYS_WIN
#else
#error SCTF is only supported for Linux (gcc), OSX (clang), and Windows (msvc)
#endif
#endif// SCTF_CPP_META_HPP
#ifndef SCTF_TYPES_HPP
#define SCTF_TYPES_HPP
namespace sctf { namespace intern {
using void_function = std::function<void()>;
}}// namespace sctf::intern
#endif// SCTF_TYPES_HPP
#ifndef SCTF_LOC_HPP
#define SCTF_LOC_HPP
namespace sctf { namespace intern {
struct loc final {
char const* file;
int const line;
};
}}// namespace sctf::intern
#endif// SCTF_LOC_HPP
#ifndef SCTF_TRAITS_HPP
#define SCTF_TRAITS_HPP
#define SCTF_INTERN_ENABLE_IF(C) typename std::enable_if<C>::type* = nullptr
#define SCTF_INTERN_IS(T, ...) (T<__VA_ARGS__>::value)
#define SCTF_INTERN_IS_TYPE(T, R) (SCTF_INTERN_IS(std::is_same, T, R))
#define SCTF_INTERN_IS_FLOAT(T) (SCTF_INTERN_IS(std::is_floating_point, T))
#define SCTF_INTERN_HAS_STREAM_CAPABILITY(T, S) (SCTF_INTERN_IS(sctf::intern::stream_capability, S, T))
#define SCTF_INTERN_HAS_ITERATOR_CAPABILITY(T) (SCTF_INTERN_IS(sctf::intern::iterator_capability, T))
namespace sctf { namespace intern {
template<typename S, typename T> class stream_capability {
template<typename SS, typename TT> static auto test(int) -> decltype(std::declval<SS&>() << std::declval<TT>(), std::true_type());
template<typename, typename> static auto test(...) -> std::false_type;
public:
static const bool value = decltype(test<S, T>(0))::value;
};
template<typename T> class iterator_capability {
template<typename TT> static auto test(int) -> decltype(std::begin(std::declval<TT&>()) != std::end(std::declval<TT&>()), void(), ++std::declval<decltype(std::begin(std::declval<TT&>()))&>(), void(*std::begin(std::declval<TT&>())), std::true_type());
template<typename> static auto test(...) -> std::false_type;
public:
static const bool value = decltype(test<T>(0))::value;
};
}}// namespace sctf::intern
#endif// SCTF_TRAITS_HPP
#ifndef SCTF_DURATION_HPP
#define SCTF_DURATION_HPP
namespace sctf { namespace intern {
class duration final {
public:
duration() : m_start(std::chrono::steady_clock::now()) {}
~duration() noexcept = default;
double get() { return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_start).count(); }
private:
std::chrono::steady_clock::time_point const m_start;
};
}}// namespace sctf::intern
#endif// SCTF_DURATION_HPP
#ifndef SCTF_REGEX_HPP
#define SCTF_REGEX_HPP
namespace sctf {
struct regex final {
regex(char const* p_, std::regex_constants::syntax_option_type flags_) : pattern(p_), re(p_, flags_) {}
operator std::regex() const { return re; }
char const* const pattern;
std::regex const re;
};
inline regex operator"" _re(char const* lit_, std::size_t) { return regex(lit_, std::regex::nosubs | std::regex::ECMAScript); }
inline regex operator"" _re_i(char const* lit_, std::size_t) { return regex(lit_, std::regex::nosubs | std::regex::ECMAScript | std::regex::icase); }
}// namespace sctf
#endif// SCTF_REGEX_HPP
#ifndef SCTF_STRINGIFY_HPP
#define SCTF_STRINGIFY_HPP
namespace sctf { namespace intern {
template<typename T> static std::string const& name_for_type() {
static thread_local std::string name;
if(name.length() > 0) { return name; }
#ifdef SCTF_SYS_UNIX
std::string const sig(__PRETTY_FUNCTION__);
auto const b = sig.rfind("T = ") + 4;
name = sig.substr(b, sig.find_first_of(";]", b) - b);
name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
#else
std::string const sig(typeid(T).name());
auto b = sig.find("struct ");
if(b != std::string::npos) {
name = sig.substr(b + 7);
return name;
}
b = sig.find("class ");
if(b != std::string::npos) {
name = sig.substr(b + 6);
} else {
name = std::move(sig);
}
#endif
return name;
}
inline std::string escaped_char(char c_) {
char const* ret;
switch(c_) {
case '\r': ret = "\\r"; break;
case '\n': ret = "\\n"; break;
case '\t': ret = "\\t"; break;
case '\f': ret = "\\f"; break;
case '\v': ret = "\\v"; break;
case '\"': ret = "\\\""; break;
default: return std::string(1, c_);
}
return ret;
}
static std::string escaped_string(std::string const& str_) {
std::string s = str_;
std::size_t p = 0;
while((p = s.find_first_of("\r\n\t\f\v\"", p)) != std::string::npos) {
s.replace(p, 1, escaped_char(s[p]));
p += 2;
}
return s;
}
template<typename T, SCTF_INTERN_ENABLE_IF(SCTF_INTERN_HAS_STREAM_CAPABILITY(T, std::ostringstream) && !SCTF_INTERN_IS_FLOAT(T))> std::string to_string(T const& arg_) {
std::ostringstream oss;
oss << arg_;
return oss.str();
}
template<typename T, SCTF_INTERN_ENABLE_IF(SCTF_INTERN_IS_FLOAT(T))> std::string to_string(T const& arg_) {
std::ostringstream oss;
oss << std::setprecision(std::numeric_limits<T>::max_digits10) << arg_;
return oss.str();
}
template<typename T, SCTF_INTERN_ENABLE_IF(!SCTF_INTERN_HAS_STREAM_CAPABILITY(T, std::ostringstream))> std::string to_string(T const&) { return name_for_type<T>(); }
inline std::string to_string(std::string const& arg_) { return std::string("\"") + escaped_string(arg_) + "\""; }
inline std::string to_string(char const* const& arg_) { return std::string("\"") + escaped_string(arg_) + "\""; }
inline std::string to_string(char const& arg_) { return std::string("'") + escaped_char(arg_) + "'"; }
inline std::string to_string(std::nullptr_t const&) { return "0"; }
inline std::string to_string(bool const& arg_) { return arg_ ? "true" : "false"; }
inline std::string to_string(regex const& arg_) { return to_string(arg_.pattern); }
}}// namespace sctf::intern
#endif// SCTF_STRINGIFY_HPP
#ifndef SCTF_ASSERTION_FAILURE_HPP
#define SCTF_ASSERTION_FAILURE_HPP
namespace sctf { namespace intern {
class assertion_failure : public std::exception {
public:
assertion_failure(std::string const& msg_, loc const& loc_) : m_msg(msg_ + " at " + loc_.file + ":" + std::to_string(loc_.line)) {}
virtual ~assertion_failure() noexcept override = default;
inline char const* what() const noexcept override { return m_msg.c_str(); }
protected:
std::string const m_msg;
};
}}// namespace sctf::intern
#endif// SCTF_ASSERTION_FAILURE_HPP
#ifndef SCTF_TESTCASE_HPP
#define SCTF_TESTCASE_HPP
namespace sctf { namespace intern {
class testcase {
public:
testcase(testcase const&) = delete;
testcase& operator=(testcase const&) = delete;
~testcase() noexcept = default;
testcase(char const* name_, char const* ctx_, void_function&& fn_) : m_name(name_), m_context(ctx_), m_test_func(std::move(fn_)) {}
testcase(testcase&& other_) : m_name(other_.m_name), m_context(other_.m_context), m_state(other_.m_state), m_duration(other_.m_duration), m_err_msg(std::move(other_.m_err_msg)), m_test_func(std::move(other_.m_test_func)) {}
testcase& operator=(testcase&& other_) {
m_name = other_.m_name;
m_context = other_.m_context;
m_state = other_.m_state;
m_duration = other_.m_duration;
m_err_msg = std::move(other_.m_err_msg);
m_test_func = std::move(other_.m_test_func);
return *this;
}
enum class result : std::int_fast8_t { NONE, PASSED, FAILED, ERROR };
void operator()() {
if(m_state != result::NONE) return;
class duration dur;
try {
m_test_func();
pass();
} catch(assertion_failure const& e) { fail(e.what()); } catch(std::exception const& e) {
error(e.what());
} catch(...) { error(); }
m_duration = dur.get();
}
inline result state() const { return m_state; }
inline double duration() const { return m_duration; }
inline std::string const& reason() const { return m_err_msg; }
inline char const* name() const { return m_name; }
inline char const* context() const { return m_context; }
inline void cout(std::string const& str_) { m_cout = str_; }
inline void cerr(std::string const& str_) { m_cerr = str_; }
inline std::string const& cout() const { return m_cout; }
inline std::string const& cerr() const { return m_cerr; }
private:
inline void pass() { m_state = result::PASSED; }
inline void fail(char const* msg_) {
m_state = result::FAILED;
m_err_msg = msg_;
}
inline void error(char const* msg_ = "unknown error") {
m_state = result::ERROR;
m_err_msg = msg_;
}
char const* m_name;
char const* m_context;
result m_state = result::NONE;
double m_duration = 0.0;
std::string m_err_msg;
std::string m_cout;
std::string m_cerr;
void_function m_test_func;
};
}}// namespace sctf::intern
#endif// SCTF_TESTSUITE_TESTCASE_HPP
#ifndef SCTF_TESTSUITE_STREAMBUF_PROXY_OMP_HPP
#define SCTF_TESTSUITE_STREAMBUF_PROXY_OMP_HPP
#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_max_threads() 1
#define omp_get_thread_num() 0
#endif
namespace sctf { namespace intern {
class streambuf_proxy_omp : public std::streambuf {
#define SCTF_INTERN_CURRENT_THREAD_BUFFER() (m_thd_buffers.at(static_cast<std::size_t>(omp_get_thread_num())))
public:
streambuf_proxy_omp(std::ostream& stream_) : m_orig_buf(stream_.rdbuf(this)), m_orig_stream(stream_), m_thd_buffers(static_cast<std::size_t>(omp_get_max_threads())) {}
virtual ~streambuf_proxy_omp() noexcept override { m_orig_stream.rdbuf(m_orig_buf); }
std::string str() const { return SCTF_INTERN_CURRENT_THREAD_BUFFER().str(); }
void clear() { SCTF_INTERN_CURRENT_THREAD_BUFFER().str(""); }
protected:
virtual int_type overflow(int_type c_) override { return SCTF_INTERN_CURRENT_THREAD_BUFFER().sputc(std::stringbuf::traits_type::to_char_type(c_)); }
virtual std::streamsize xsputn(char const* s_, std::streamsize n_) override { return SCTF_INTERN_CURRENT_THREAD_BUFFER().sputn(s_, n_); }
std::streambuf* m_orig_buf;
std::ostream& m_orig_stream;
std::vector<std::stringbuf> m_thd_buffers;
};
}}// namespace sctf::intern
#ifndef _OPENMP
#undef omp_get_max_threads
#undef omp_get_thread_num
#endif
#undef SCTF_INTERN_CURRENT_THREAD_BUFFER
#endif// SCTF_TESTSUITE_STREAMBUF_PROXY_OMP_HPP
#ifndef SCTF_TESTSUITE_STREAMBUF_PROXY_HPP
#define SCTF_TESTSUITE_STREAMBUF_PROXY_HPP
namespace sctf { namespace intern {
class streambuf_proxy : public std::streambuf {
public:
streambuf_proxy(std::ostream& stream_) : m_orig_buf(stream_.rdbuf(this)), m_orig_stream(stream_) {}
virtual ~streambuf_proxy() noexcept override { m_orig_stream.rdbuf(m_orig_buf); }
std::string str() const { return m_buffer.str(); }
void clear() { m_buffer.str(""); }
protected:
virtual int_type overflow(int_type c_) override { return m_buffer.sputc(std::stringbuf::traits_type::to_char_type(c_)); }
virtual std::streamsize xsputn(char const* s_, std::streamsize n_) override { return m_buffer.sputn(s_, n_); }
std::streambuf* m_orig_buf;
std::ostream& m_orig_stream;
std::stringbuf m_buffer;
};
}}// namespace sctf::intern
#endif// SCTF_TESTSUITE_STREAMBUF_PROXY_HPP
#ifndef SCTF_TESTSUITE_STATISTICS_HPP
#define SCTF_TESTSUITE_STATISTICS_HPP
namespace sctf { namespace intern {
class statistic {
public:
inline std::size_t tests() const { return m_num_of_tests; }
inline std::size_t successes() const { return m_num_of_tests - m_num_of_errs - m_num_of_fails; }
inline std::size_t failures() const { return m_num_of_fails; }
inline std::size_t errors() const { return m_num_of_errs; }
protected:
friend class testsuite;
friend class testsuite_parallel;
std::size_t m_num_of_tests = 0;
std::size_t m_num_of_fails = 0;
std::size_t m_num_of_errs = 0;
};
}}// namespace sctf::intern
#endif// SCTF_TESTSUITE_STATISTICS_HPP
#ifndef SCTF_TESTSUITE_TESTSUITE_HPP
#define SCTF_TESTSUITE_TESTSUITE_HPP
namespace sctf { namespace intern {
class testsuite;
using testsuite_ptr = std::shared_ptr<testsuite>;
class testsuite {
public:
testsuite(testsuite const&) = delete;
testsuite& operator=(testsuite const&) = delete;
virtual ~testsuite() noexcept = default;
static testsuite_ptr create(char const* name_) { return testsuite_ptr(new testsuite(name_)); }
virtual void run() {
if(m_state != execution_state::DONE) {
m_setup_fn();
m_stats.m_num_of_tests = m_testcases.size();
streambuf_proxy buf_cout(std::cout);
streambuf_proxy buf_cerr(std::cerr);
std::for_each(m_testcases.begin(), m_testcases.end(), [this, &buf_cerr, &buf_cout](testcase& tc_) {
if(tc_.state() == testcase::result::NONE) {
m_pretest_fn();
tc_();
switch(tc_.state()) {
case testcase::result::FAILED: ++m_stats.m_num_of_fails; break;
case testcase::result::ERROR: ++m_stats.m_num_of_errs; break;
default: break;
}
m_exec_dur += tc_.duration();
m_posttest_fn();
tc_.cout(buf_cout.str());
tc_.cerr(buf_cerr.str());
buf_cout.clear();
buf_cerr.clear();
}
});
m_teardown_fn();
m_state = execution_state::DONE;
}
}
void test(char const* name_, void_function&& fn_) {
m_testcases.push_back(testcase(name_, m_name, std::move(fn_)));
m_state = execution_state::PENDING;
}
void setup(void_function&& fn_) { m_setup_fn.fn = std::move(fn_); }
void teardown(void_function&& fn_) { m_teardown_fn.fn = std::move(fn_); }
void before_each(void_function&& fn_) { m_pretest_fn.fn = std::move(fn_); }
void after_each(void_function&& fn_) { m_posttest_fn.fn = std::move(fn_); }
inline char const* name() const { return m_name; }
inline std::chrono::system_clock::time_point const& timestamp() const { return m_create_time; }
inline statistic const& statistics() const { return m_stats; }
inline double execution_duration() const { return m_exec_dur; }
inline std::vector<testcase> const& testcases() const { return m_testcases; }
protected:
explicit testsuite(char const* name_) : m_name(name_), m_create_time(std::chrono::system_clock::now()) {}
struct silent_functor final {
silent_functor& operator()() {
if(fn) {
try {
fn();
} catch(...) {}
}
return *this;
}
void_function fn;
};
enum class execution_state : std::int_fast8_t { PENDING, DONE };
char const* m_name;
std::chrono::system_clock::time_point const m_create_time;
double m_exec_dur = 0.0;
statistic m_stats;
std::vector<testcase> m_testcases;
execution_state m_state = execution_state::PENDING;
silent_functor m_setup_fn;
silent_functor m_teardown_fn;
silent_functor m_pretest_fn;
silent_functor m_posttest_fn;
};
}}// namespace sctf::intern
#endif// SCTF_TESTSUITE_TESTSUITE_HPP
#ifndef SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
#define SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
namespace sctf { namespace intern {
class testsuite_parallel : public testsuite {
public:
testsuite_parallel(testsuite_parallel const&) = delete;
testsuite_parallel& operator=(testsuite_parallel const&) = delete;
virtual ~testsuite_parallel() noexcept override = default;
static testsuite_ptr create(char const* name_) { return testsuite_ptr(new testsuite_parallel(name_)); }
void run() override {
if(m_state != execution_state::DONE) {
if(m_testcases.size() > static_cast<std::size_t>(std::numeric_limits<long>::max())) { throw std::overflow_error("Too many testcases! Size would overflow loop variant."); }
m_setup_fn();
const long tc_size = static_cast<long>(m_testcases.size());
m_stats.m_num_of_tests = m_testcases.size();
streambuf_proxy_omp mt_buf_cout(std::cout);
streambuf_proxy_omp mt_buf_cerr(std::cerr);
#pragma omp parallel
{// BEGIN parallel section
double tmp = 0.0;
std::size_t fails = 0;
std::size_t errs = 0;
// OpenMP 2 compatible - MSVC not supporting higher version
#pragma omp for schedule(dynamic)
for(long i = 0; i < tc_size; ++i) {
auto& tc = m_testcases[static_cast<std::size_t>(i)];
if(tc.state() == testcase::result::NONE) {
m_pretest_fn();
tc();
switch(tc.state()) {
case testcase::result::FAILED: ++fails; break;
case testcase::result::ERROR: ++errs; break;
default: break;
}
tmp += tc.duration();
m_posttest_fn();
tc.cout(mt_buf_cout.str());
tc.cerr(mt_buf_cerr.str());
mt_buf_cout.clear();
mt_buf_cerr.clear();
}
}
#pragma omp critical
{// BEGIN critical section
m_stats.m_num_of_fails += fails;
m_stats.m_num_of_errs += errs;
m_exec_dur = std::max(m_exec_dur, tmp);
}// END critical section
}// END parallel section
m_teardown_fn();
m_state = execution_state::DONE;
}
}
private:
explicit testsuite_parallel(char const* name_) : testsuite(name_) {}
};
}}// namespace sctf::intern
#endif// SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
#ifndef SCTF_RUNNER_HPP
#define SCTF_RUNNER_HPP
namespace sctf { namespace intern {
class runner {
public:
void add_testsuite(testsuite_ptr ts_) { m_testsuites.push_back(ts_); }
void run() noexcept {
std::for_each(m_testsuites.begin(), m_testsuites.end(), [](testsuite_ptr& ts_) { ts_->run(); });
}
std::vector<testsuite_ptr> const& testsuites() { return m_testsuites; }
static runner& instance() {
static runner r;
return r;
}
private:
std::vector<testsuite_ptr> m_testsuites;
};
}}// namespace sctf::intern
#endif// SCTF_RUNNER_HPP
#ifndef SCTF_REPORTER_REPORTER_HPP
#define SCTF_REPORTER_REPORTER_HPP
namespace sctf {
namespace intern {
class reporter {
public:
virtual ~reporter() noexcept = default;
std::size_t report() {
m_abs_errs = 0;
m_abs_fails = 0;
m_abs_tests = 0;
m_abs_time = 0.0;
runner& runner = runner::instance();
runner.run();
begin_report();
std::for_each(runner.testsuites().begin(), runner.testsuites().end(), [this](testsuite_ptr const& ts_) {
m_abs_errs += ts_->statistics().errors();
m_abs_fails += ts_->statistics().failures();
m_abs_tests += ts_->statistics().tests();
m_abs_time += ts_->execution_duration();
report_testsuite(ts_);
});
end_report();
return m_abs_errs + m_abs_fails;
}
protected:
explicit reporter(std::ostream& stream_) : mr_out_stream(stream_) {}
explicit reporter(char const* fname_) : m_out_file(fname_), mr_out_stream(m_out_file) {
if(!mr_out_stream) { throw std::runtime_error("Could not open file."); }
}
virtual void report_testsuite(testsuite_ptr const ts_) {
std::for_each(ts_->testcases().begin(), ts_->testcases().end(), [this](const testcase& tc) { report_testcase(tc); });
}
virtual void report_testcase(testcase const& tc_) = 0;
virtual void begin_report() = 0;
virtual void end_report() = 0;
template<typename T> std::ostream& operator<<(T const& t_) {
mr_out_stream << t_;
return mr_out_stream;
}
std::ofstream m_out_file;
std::ostream& mr_out_stream;
std::size_t m_abs_tests = 0;
std::size_t m_abs_fails = 0;
std::size_t m_abs_errs = 0;
double m_abs_time = 0;
};
namespace fmt {
static constexpr char const* const SPACE = "  ";
static constexpr char const* const XSPACE = "    ";
static constexpr char const* const LF = "\n";
static constexpr char const* const XLF = "\n\n";
}// namespace fmt
}// namespace intern
using reporter_ptr = std::shared_ptr<intern::reporter>;
}// namespace sctf
#endif// SCTF_REPORTER_REPORTER_HPP
#ifndef SCTF_REPORTER_XML_REPORTER_HPP
#define SCTF_REPORTER_XML_REPORTER_HPP
namespace sctf {
class xml_reporter : public intern::reporter {
public:
~xml_reporter() noexcept override = default;
static reporter_ptr create(std::ostream& stream_ = std::cout, bool capture_ = false) { return reporter_ptr(new xml_reporter(stream_, capture_)); }
static reporter_ptr create(char const* fname_, bool capture_ = false) { return reporter_ptr(new xml_reporter(fname_, capture_)); }
private:
xml_reporter(std::ostream& stream_, bool capture_) : reporter(stream_), m_capture(capture_) {}
xml_reporter(char const* fname_, bool capture_) : reporter(fname_), m_capture(capture_) {}
void report_testsuite(intern::testsuite_ptr const ts_) override {
std::time_t stamp = std::chrono::system_clock::to_time_t(ts_->timestamp());
char buff[128];
std::strftime(buff, 127, "%FT%T", std::localtime(&stamp));
*this << intern::fmt::SPACE << "<testsuite id=\"" << m_id++ << "\" name=\"" << ts_->name() << "\" errors=\"" << ts_->statistics().errors() << "\" tests=\"" << ts_->statistics().tests() << "\" failures=\"" << ts_->statistics().failures() << "\" skipped=\"0\" time=\"" << ts_->execution_duration() << "\" timestamp=\"" << buff << "\">" << intern::fmt::LF;
reporter::report_testsuite(ts_);
*this << intern::fmt::SPACE << "</testsuite>" << intern::fmt::LF;
}
void report_testcase(intern::testcase const& tc_) override {
*this << intern::fmt::XSPACE << "<testcase name=\"" << tc_.name() << "\" classname=\"" << tc_.context() << "\" time=\"" << tc_.duration() << "\"";
switch(tc_.state()) {
case intern::testcase::result::ERROR:
*this << ">" << intern::fmt::LF << intern::fmt::XSPACE << intern::fmt::SPACE << "<error message=\"" << tc_.reason() << "\"></error>" << intern::fmt::LF;
if(m_capture) { print_system_out(tc_); }
*this << intern::fmt::XSPACE << "</testcase>";
break;
case intern::testcase::result::FAILED:
*this << ">" << intern::fmt::LF << intern::fmt::XSPACE << intern::fmt::SPACE << "<failure message=\"" << tc_.reason() << "\"></failure>" << intern::fmt::LF;
if(m_capture) { print_system_out(tc_); }
*this << intern::fmt::XSPACE << "</testcase>";
break;
default:
if(m_capture) {
*this << ">" << intern::fmt::LF;
print_system_out(tc_);
*this << intern::fmt::XSPACE << "</testcase>";
} else {
*this << "/>";
}
break;
}
*this << intern::fmt::LF;
}
void begin_report() override { *this << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << intern::fmt::LF << "<testsuites>" << intern::fmt::LF; }
void end_report() override { *this << "</testsuites>" << intern::fmt::LF; }
void print_system_out(intern::testcase const& tc_) {
*this << intern::fmt::XSPACE << intern::fmt::SPACE << "<system-out>" << tc_.cout() << "</system-out>" << intern::fmt::LF;
*this << intern::fmt::XSPACE << intern::fmt::SPACE << "<system-err>" << tc_.cerr() << "</system-err>" << intern::fmt::LF;
}
std::size_t mutable m_id = 0;
bool m_capture;
};
}// namespace sctf
#endif// SCTF_REPORTER_XML_REPORTER_HPP
#ifndef SCTF_REPORTER_CONSOLE_REPORTER_HPP
#define SCTF_REPORTER_CONSOLE_REPORTER_HPP
namespace sctf {
namespace intern { namespace fmt {
static constexpr char const* const ANSI_RED = "\x1b[31m";
static constexpr char const* const ANSI_GREEN = "\x1b[32m";
static constexpr char const* const ANSI_YELLOW = "\x1b[33m";
static constexpr char const* const ANSI_BLUE = "\x1b[34m";
static constexpr char const* const ANSI_MAGENTA = "\x1b[35m";
static constexpr char const* const ANSI_CYAN = "\x1b[36m";
static constexpr char const* const ANSI_RESET = "\x1b[0m";
}}// namespace intern::fmt
class console_reporter : public intern::reporter {
public:
~console_reporter() noexcept override = default;
static reporter_ptr create(std::ostream& stream_ = std::cout, bool color_ = false, bool capture_ = false) { return reporter_ptr(new console_reporter(stream_, color_, capture_)); }
static reporter_ptr create(char const* fname_, bool color_ = false, bool capture_ = false) { return reporter_ptr(new console_reporter(fname_, color_, capture_)); }
private:
console_reporter(std::ostream& stream_, bool color_, bool capture_) : reporter(stream_), m_color(color_), m_capture(capture_) {}
console_reporter(char const* fname_, bool color_, bool capture_) : reporter(fname_), m_color(color_), m_capture(capture_) {}
void report_testsuite(intern::testsuite_ptr const ts_) override {
*this << "Run Testsuite [" << ts_->name() << "]; time = " << ts_->execution_duration() << "ms" << intern::fmt::LF;
reporter::report_testsuite(ts_);
}
void report_testcase(intern::testcase const& tc_) override {
*this << intern::fmt::SPACE << "Run Testcase [" << tc_.name() << "](" << tc_.context() << "); time = " << tc_.duration() << "ms" << intern::fmt::LF << intern::fmt::XSPACE;
if(m_capture) {
*this << "stdout = \"" << tc_.cout() << "\"" << intern::fmt::LF << intern::fmt::XSPACE;
*this << "stderr = \"" << tc_.cerr() << "\"" << intern::fmt::LF << intern::fmt::XSPACE;
}
switch(tc_.state()) {
case intern::testcase::result::ERROR: *this << (m_color ? intern::fmt::ANSI_MAGENTA : "") << "ERROR! " << tc_.reason(); break;
case intern::testcase::result::FAILED: *this << (m_color ? intern::fmt::ANSI_RED : "") << "FAILED! " << tc_.reason(); break;
case intern::testcase::result::PASSED: *this << (m_color ? intern::fmt::ANSI_GREEN : "") << "PASSED!"; break;
default: break;
}
*this << (m_color ? intern::fmt::ANSI_RESET : "") << intern::fmt::LF;
}
void begin_report() override {}
void end_report() override {
if(m_abs_fails >= (m_abs_tests + 1) / 2) {
*this << (m_color ? intern::fmt::ANSI_YELLOW : "");
} else {
*this << (m_color ? intern::fmt::ANSI_CYAN : "");
}
*this << "Result:: passed: " << m_abs_tests - m_abs_fails - m_abs_errs << "/" << m_abs_tests << " ; failed: " << m_abs_fails << "/" << m_abs_tests << " ; errors: " << m_abs_errs << "/" << m_abs_tests << " ; time = " << m_abs_time << "ms" << (m_color ? intern::fmt::ANSI_RESET : "") << intern::fmt::LF;
}
bool m_color;
bool m_capture;
};
}// namespace sctf
#endif// SCTF_REPORTER_CONSOLE_REPORTER_HPP
#ifndef SCTF_REPORTER_MARKDOWN_REPORTER_HPP
#define SCTF_REPORTER_MARKDOWN_REPORTER_HPP
namespace sctf {
class markdown_reporter : public intern::reporter {
public:
~markdown_reporter() noexcept override = default;
static reporter_ptr create(std::ostream& stream_ = std::cout, bool capture_ = false) { return reporter_ptr(new markdown_reporter(stream_, capture_)); }
static reporter_ptr create(char const* fname_, bool capture_ = false) { return reporter_ptr(new markdown_reporter(fname_, capture_)); }
private:
markdown_reporter(std::ostream& stream_, bool capture_) : reporter(stream_), m_capture(capture_) {}
markdown_reporter(char const* fname_, bool capture_) : reporter(fname_), m_capture(capture_) {}
void report_testsuite(intern::testsuite_ptr const ts_) override {
*this << "## " << ts_->name() << intern::fmt::XLF << "|Tests|Successes|Failures|Errors|Time|" << intern::fmt::LF << "|-|-|-|-|-|" << intern::fmt::LF << "|" << ts_->statistics().tests() << "|" << ts_->statistics().successes() << "|" << ts_->statistics().failures() << "|" << ts_->statistics().errors() << "|" << ts_->execution_duration() << "ms|" << intern::fmt::XLF << "### Tests" << intern::fmt::XLF << "|Name|Context|Time|Status|" << (m_capture ? "System-Out|System-Err|" : "") << intern::fmt::LF << "|-|-|-|-|" << (m_capture ? "-|-|" : "") << intern::fmt::LF;
reporter::report_testsuite(ts_);
*this << intern::fmt::XLF;
}
void report_testcase(intern::testcase const& tc_) override {
char const* status = "";
switch(tc_.state()) {
case intern::testcase::result::ERROR: status = "ERROR"; break;
case intern::testcase::result::FAILED: status = "FAILED"; break;
case intern::testcase::result::PASSED: status = "PASSED"; break;
default: break;
}
*this << "|" << tc_.name() << "|" << tc_.context() << "|" << tc_.duration() << "ms|" << status << "|";
if(m_capture) {
print_system_out(tc_.cout());
print_system_out(tc_.cerr());
}
*this << intern::fmt::LF;
}
void begin_report() override { *this << "# Test Report" << intern::fmt::XLF; }
void end_report() override { *this << "## Summary" << intern::fmt::XLF << "|Tests|Successes|Failures|Errors|Time|" << intern::fmt::LF << "|-|-|-|-|-|" << intern::fmt::LF << "|" << m_abs_tests << "|" << (m_abs_tests - m_abs_errs - m_abs_fails) << "|" << m_abs_fails << "|" << m_abs_errs << "|" << m_abs_time << "ms|" << intern::fmt::LF; }
void print_system_out(std::string const& out_) {
std::string line;
std::istringstream io_;
io_.str(out_);
bool first = true;
while(std::getline(io_, line)) {
*this << (first ? "" : "<br>") << "`" << line << "`";
first = false;
}
*this << "|";
}
bool m_capture;
};
}// namespace sctf
#endif// SCTF_REPORTER_MARKDOWN_REPORTER_HPP
#ifndef SCTF_COMPARATOR_COMPARATOR_HPP
#define SCTF_COMPARATOR_COMPARATOR_HPP
#ifdef SCTF_CPP_V17
#include <optional>
#else
#include <cstring>
#endif
namespace sctf { namespace intern {
struct comparison final {
#ifdef SCTF_CPP_V17
comparison() : m_failure(std::nullopt) {}
comparison(char const* comp_str_, std::string const& val_, std::string const& expect_) : m_failure("Expected " + val_ + " " + comp_str_ + " " + expect_) {}
explicit operator bool() { return !m_failure; }
std::string const& operator*() const { return *m_failure; }
private:
std::optional<std::string> const m_failure;
#else
comparison() : m_success(true) {}
comparison(char const* comp_str_, std::string const& val_, std::string const& expect_) : m_success(false) {
std::string msg;
msg.reserve(15 + std::strlen(comp_str_) + val_.length() + expect_.length());
msg = "Expected ";
msg.append(val_).append(" ").append(comp_str_).append(" ").append(expect_);
error() = msg;
}
explicit operator bool() { return m_success; }
const std::string& operator*() const { return error(); }
private:
bool const m_success;
std::string& error() const {
static thread_local std::string err_msg;
return err_msg;
}
#endif
};
}}// namespace sctf::intern
#define SCTF_COMPARATOR(NAME, CMPSTR, PRED) \
namespace sctf { namespace intern { \
class NAME { \
static constexpr char const* m_cmp_str = "to be " CMPSTR; \
static constexpr char const* m_neg_cmp_str = "to be not " CMPSTR; \
bool m_neg = false; \
public: \
NAME& operator!() { \
m_neg = !m_neg; \
return *this; \
} \
template<typename V, typename E = V> comparison operator()(V const& actual_value, E const& expected_value) { return (PRED) != m_neg ? comparison() : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)); } \
}; \
} \
}
#define SCTF_PROVIDE_COMPARATOR(COMP, NAME) \
namespace sctf { \
template<typename... Args> static intern::COMP NAME(Args&&... args) { return intern::COMP(std::forward<Args>(args)...); } \
}
#endif// SCTF_COMPARATOR_COMPARATOR_HPP
#ifndef SCTF_COMPARATOR_ORDERING_HPP
#define SCTF_COMPARATOR_ORDERING_HPP
SCTF_COMPARATOR(greater_than, "greater than", actual_value > expected_value)
SCTF_PROVIDE_COMPARATOR(greater_than, GREATER_THAN)
SCTF_PROVIDE_COMPARATOR(greater_than, GREATER)
SCTF_PROVIDE_COMPARATOR(greater_than, GT)
SCTF_COMPARATOR(less_than, "less than", actual_value < expected_value)
SCTF_PROVIDE_COMPARATOR(less_than, LESS_THAN)
SCTF_PROVIDE_COMPARATOR(less_than, LESS)
SCTF_PROVIDE_COMPARATOR(less_than, LT)
#endif// SCTF_COMPARATOR_ORDERING_HPP
#ifndef SCTF_COMPARATOR_EQUALITY_HPP
#define SCTF_COMPARATOR_EQUALITY_HPP
SCTF_COMPARATOR(equals, "equals", actual_value == expected_value)
SCTF_PROVIDE_COMPARATOR(equals, EQUALS)
SCTF_PROVIDE_COMPARATOR(equals, EQ)
#define SCTF_EPSILON(E) double sctf::epsilon = E;
namespace sctf {
extern double epsilon;
namespace intern {
class f_equals {
static constexpr char const* m_cmp_str = "to be equals";
static constexpr char const* m_neg_cmp_str = "to be not equals";
bool m_neg = false;
double m_eps = epsilon;
public:
f_equals() = default;
f_equals(double eps_) : m_eps(eps_) {}
f_equals& operator!() {
m_neg = !m_neg;
return *this;
}
template<typename V, typename E = V> comparison operator()(V const& actual_value, E const& expected_value) {
static_assert(SCTF_INTERN_IS_FLOAT(V) && SCTF_INTERN_IS_FLOAT(E), "The floating point comparator must not be used with other types than float, or double!");
V epsilon_ = static_cast<V>(m_eps);
return (std::abs(actual_value - expected_value) <= std::max(std::abs(actual_value), std::abs(expected_value)) * epsilon_) != m_neg ? comparison() : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value));
}
};
}// namespace intern
}// namespace sctf
SCTF_PROVIDE_COMPARATOR(f_equals, F_EQUALS)
SCTF_PROVIDE_COMPARATOR(f_equals, FEQ)
#endif// SCTF_COMPARATOR_EQUALITY_HPP
#ifndef SCTF_COMPARATOR_RANGE_HPP
#define SCTF_COMPARATOR_RANGE_HPP
namespace sctf { namespace intern {
class in_range {
static constexpr char const* m_cmp_str = "to be in range of";
static constexpr char const* m_neg_cmp_str = "to be not in range of";
bool m_neg = false;
public:
in_range& operator!() {
m_neg = !m_neg;
return *this;
}
template<typename V, typename E = V, SCTF_INTERN_ENABLE_IF(SCTF_INTERN_HAS_ITERATOR_CAPABILITY(E) && !SCTF_INTERN_IS_TYPE(E, std::string))> comparison operator()(V const& actual_value, E const& expected_value) { return (std::find(expected_value.cbegin(), expected_value.cend(), actual_value) != expected_value.cend()) != m_neg ? comparison() : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)); }
template<typename V, typename E = V, SCTF_INTERN_ENABLE_IF(SCTF_INTERN_IS_TYPE(E, std::string))> comparison operator()(V const& actual_value, E const& expected_value) { return (expected_value.find(actual_value) != std::string::npos) != m_neg ? comparison() : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)); }
};
}}// namespace sctf::intern
SCTF_PROVIDE_COMPARATOR(in_range, IN_RANGE)
SCTF_PROVIDE_COMPARATOR(in_range, IN)
#endif// SCTF_COMPARATOR_RANGE_HPP
#ifndef SCTF_COMPARATOR_REGEX_HPP
#define SCTF_COMPARATOR_REGEX_HPP
SCTF_COMPARATOR(match, "matching", std::regex_match(actual_value, std::regex(expected_value)))
SCTF_PROVIDE_COMPARATOR(match, MATCH)
SCTF_COMPARATOR(like, "like", std::regex_search(actual_value, std::regex(expected_value)))
SCTF_PROVIDE_COMPARATOR(like, LIKE)
#endif// SCTF_COMPARATOR_REGEX_HPP
#ifndef SCTF_ASSERT_HPP
#define SCTF_ASSERT_HPP
#define ASSERT(VAL, CMP, EXP) sctf::intern::assert_statement(VAL, EXP, sctf::CMP, sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_NOT(VAL, CMP, EXP) sctf::intern::assert_statement(VAL, EXP, !sctf::CMP, sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_EQ(VAL, EXP) sctf::intern::assert_statement(VAL, EXP, sctf::EQUALS(), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_TRUE(VAL) sctf::intern::assert_statement(VAL, true, sctf::EQUALS(), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_FALSE(VAL) sctf::intern::assert_statement(VAL, false, sctf::EQUALS(), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_NULL(PTR) sctf::intern::assert_statement(static_cast<void const*>(PTR), nullptr, sctf::EQUALS(), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_NOT_NULL(PTR) sctf::intern::assert_statement(static_cast<void const*>(PTR), nullptr, !sctf::EQUALS(), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_ZERO(VAL) sctf::intern::assert_statement(VAL, static_cast<decltype(VAL)>(0), sctf::EQUALS(), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_THROWS(FN, TRW) sctf::intern::assert_throws<TRW>([&] { FN; }, #TRW, sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_NOTHROW(FN) sctf::intern::assert_nothrow([&] { FN; }, sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_RUNTIME(FN, MAX) sctf::intern::assert_runtime([&] { FN; }, MAX, sctf::intern::loc{__FILE__, __LINE__})
namespace sctf { namespace intern {
template<typename C, typename V, typename E = V> static void assert_statement(V const& val_, E const& exp_, C&& cmp_, loc const& loc_) {
comparison res = cmp_(val_, exp_);
if(!res) { throw assertion_failure(*res, loc_); }
}
template<typename T> static void assert_throws(void_function&& fn_, char const* tname_, loc const& loc_) {
try {
fn_();
} catch(T const&) { return; } catch(std::exception const& e) {
throw assertion_failure("Wrong exception thrown, caught " + to_string(e), loc_);
} catch(...) { throw assertion_failure("Wrong exception thrown", loc_); }
throw assertion_failure(std::string("No exception thrown, expected ") + tname_, loc_);
}
static void assert_nothrow(void_function&& fn_, loc const& loc_) {
try {
fn_();
} catch(std::exception const& e) { throw assertion_failure("Expected no exception, caught " + to_string(e), loc_); } catch(...) {
throw assertion_failure("Expected no exception", loc_);
}
}
static void assert_runtime(void_function&& fn_, double max_ms_, loc const& loc_) {
try {
duration dur;
fn_();
double dur_ms = dur.get();
if(dur_ms > max_ms_) { throw assertion_failure("runtime > " + to_string(max_ms_) + "ms", loc_); }
} catch(std::exception const& e) { throw assertion_failure(e.what(), loc_); } catch(...) {
throw assertion_failure("Unknown exception thrown", loc_);
}
}
}}// namespace sctf::intern
#endif// SCTF_ASSERT_HPP
#ifndef SCTF_API_HPP
#define SCTF_API_HPP
namespace sctf { namespace intern {
template<typename T> struct singleton final {
template<typename... Args> static T& instance(Args&&... args_) {
static T inst(std::forward<Args>(args_)...);
return inst;
}
};
}}// namespace sctf::intern
#define SCTF_INTERN_CONCAT3(A, B, C) A##B##C
#define SCTF_INTERN_API_TEST_NAME(ID) SCTF_INTERN_CONCAT3(sctf_intern_test_, ID, _)
#define SCTF_INTERN_API_TEST_INST(ID) SCTF_INTERN_CONCAT3(sctf_intern_test_, ID, _inst_)
#define SCTF_INTERN_API_TEST_FN(ID) SCTF_INTERN_CONCAT3(sctf_intern_test_fn_, ID, _)
#define SCTF_INTERN_API_SUITE_NS(ID) SCTF_INTERN_CONCAT3(sctf_intern_ns_, ID, _)
#define SCTF_INTERN_API_SUITE_NAME(ID) SCTF_INTERN_CONCAT3(sctf_intern_suite_, ID, _)
#define SCTF_INTERN_API_SUITE_WRAPPER(DESCR, BASE) \
namespace SCTF_INTERN_API_SUITE_NS(__LINE__) { \
class test_module { \
protected: \
virtual ~test_module() noexcept = default; \
test_module() : sctf_intern_m_ts_(sctf::intern::BASE::create(DESCR)) { sctf::intern::runner::instance().add_testsuite(sctf_intern_m_ts_); } \
sctf::intern::testsuite_ptr sctf_intern_m_ts_; \
}; \
class SCTF_INTERN_API_SUITE_NAME(__LINE__); \
static const auto& sctf_intern_mod_ = sctf::intern::singleton<SCTF_INTERN_API_SUITE_NAME(__LINE__)>::instance(); \
using sctf_intern_mod_type_ = SCTF_INTERN_API_SUITE_NAME(__LINE__); \
} \
class SCTF_INTERN_API_SUITE_NS(__LINE__)::SCTF_INTERN_API_SUITE_NAME(__LINE__) : public SCTF_INTERN_API_SUITE_NS(__LINE__)::test_module
#define SCTF_INTERN_API_TEST_WRAPPER(DESCR) \
class SCTF_INTERN_API_TEST_NAME(__LINE__) { \
public: \
SCTF_INTERN_API_TEST_NAME(__LINE__)(sctf_intern_mod_type_ * mod_) { mod_->sctf_intern_m_ts_->test(DESCR, std::bind(&sctf_intern_mod_type_::SCTF_INTERN_API_TEST_FN(__LINE__), mod_)); } \
} SCTF_INTERN_API_TEST_INST(__LINE__){this}; \
void SCTF_INTERN_API_TEST_FN(__LINE__)()
#define SCTF_INTERN_API_FN_WRAPPER(FN) \
class sctf_intern_##FN##_ { \
public: \
sctf_intern_##FN##_(sctf_intern_mod_type_* mod_) { mod_->sctf_intern_m_ts_->FN(std::bind(&sctf_intern_mod_type_::sctf_intern_##FN##_fn_, mod_)); } \
} sctf_intern_##FN##_inst_{this}; \
void sctf_intern_##FN##_fn_()
#define SUITE(DESCR) SCTF_INTERN_API_SUITE_WRAPPER(DESCR, testsuite)
#define SUITE_PAR(DESCR) SCTF_INTERN_API_SUITE_WRAPPER(DESCR, testsuite_parallel)
#define DESCRIBE(DESCR) SUITE(DESCR)
#define DESCRIBE_PAR(DESCR) SUITE_PAR(DESCR)
#define TEST(DESCR) SCTF_INTERN_API_TEST_WRAPPER(DESCR)
#define IT(DESCR) SCTF_INTERN_API_TEST_WRAPPER("It " DESCR)
#define BEFORE_EACH() SCTF_INTERN_API_FN_WRAPPER(before_each)
#define AFTER_EACH() SCTF_INTERN_API_FN_WRAPPER(after_each)
#define SETUP() SCTF_INTERN_API_FN_WRAPPER(setup)
#define TEARDOWN() SCTF_INTERN_API_FN_WRAPPER(teardown)
#endif// SCTF_API_HPP
#ifndef SCTF_SCTF_HPP
#define SCTF_SCTF_HPP
#define SCFT_VERSION "2.0-rc1"
#define SCTF_DEFAULT_MAIN(R) \
int main(int, char**) { return static_cast<int>(sctf::R->report()); }
#endif// SCTF_SCTF_HPP
#endif
