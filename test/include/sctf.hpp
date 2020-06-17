/*
    Copyright (C) 2017 Jarthianur

    This file is part of simple-cpp-test-framework.

    simple-cpp-test-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    simple-cpp-test-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with simple-cpp-test-framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef SCTF_RELEASE_SCTF_HPP
#define SCTF_RELEASE_SCTF_HPP
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
#endif
#ifndef SCTF_LOC_HPP
#define SCTF_LOC_HPP
namespace sctf { namespace intern {
struct loc final {
char const* file;
int const line;
};
}}
#endif
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
}}
#endif
#ifndef SCTF_DURATION_HPP
#define SCTF_DURATION_HPP
namespace sctf { namespace intern {
class duration final {
public:
duration() : m_start(std::chrono::steady_clock::now()) {}
auto get() -> double { return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_start).count(); }
private:
std::chrono::steady_clock::time_point const m_start;
};
}}
#endif
#ifndef SCTF_REGEX_HPP
#define SCTF_REGEX_HPP
namespace sctf {
struct regex final {
regex(char const* p_, std::regex_constants::syntax_option_type flags_) : pattern(p_), re(p_, flags_) {}
operator std::regex() const { return re; }
char const* const pattern;
std::regex const re;
};
inline auto operator"" _re(char const* lit_, std::size_t) -> regex { return regex(lit_, std::regex::nosubs | std::regex::ECMAScript); }
inline auto operator"" _re_i(char const* lit_, std::size_t) -> regex { return regex(lit_, std::regex::nosubs | std::regex::ECMAScript | std::regex::icase); }
}
#endif
#ifndef SCTF_STRINGIFY_HPP
#define SCTF_STRINGIFY_HPP
namespace sctf { namespace intern {
template<typename T> static auto name_for_type() -> std::string const& {
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
inline auto escaped_char(char c_) -> std::string {
switch(c_) {
case '\r': return "\\r";
case '\n': return "\\n";
case '\t': return "\\t";
case '\f': return "\\f";
case '\v': return "\\v";
case '\"': return "\\\"";
default: break;
}
return std::string(1, c_);
}
static auto escaped_string(std::string const& str_) -> std::string {
std::string s = str_;
std::size_t p = 0;
while((p = s.find_first_of("\r\n\t\f\v\"", p)) != std::string::npos) {
s.replace(p, 1, escaped_char(s[p]));
p += 2;
}
return s;
}
template<typename T, SCTF_INTERN_ENABLE_IF(SCTF_INTERN_HAS_STREAM_CAPABILITY(T, std::ostringstream) && !SCTF_INTERN_IS_FLOAT(T))> auto to_string(T const& arg_) -> std::string {
std::ostringstream oss;
oss << arg_;
return oss.str();
}
template<typename T, SCTF_INTERN_ENABLE_IF(SCTF_INTERN_IS_FLOAT(T))> auto to_string(T const& arg_) -> std::string {
std::ostringstream oss;
oss << std::setprecision(std::numeric_limits<T>::max_digits10) << arg_;
return oss.str();
}
template<typename T, SCTF_INTERN_ENABLE_IF(!SCTF_INTERN_HAS_STREAM_CAPABILITY(T, std::ostringstream))> auto to_string(T const&) -> std::string { return name_for_type<T>(); }
inline auto to_string(std::string const& arg_) -> std::string { return std::string("\"") + escaped_string(arg_) + "\""; }
inline auto to_string(char const* const& arg_) -> std::string { return std::string("\"") + escaped_string(arg_) + "\""; }
inline auto to_string(char const& arg_) -> std::string { return std::string("'") + escaped_char(arg_) + "'"; }
inline auto to_string(std::nullptr_t const&) -> std::string { return "0"; }
inline auto to_string(bool const& arg_) -> std::string { return arg_ ? "true" : "false"; }
inline auto to_string(regex const& arg_) -> std::string { return to_string(arg_.pattern); }
}}
#endif
#ifndef SCTF_ASSERTION_FAILURE_HPP
#define SCTF_ASSERTION_FAILURE_HPP
namespace sctf { namespace intern {
class assertion_failure : public std::exception {
public:
assertion_failure(std::string const& msg_, loc const& loc_) : m_msg(msg_ + " at " + loc_.file + ":" + std::to_string(loc_.line)) {}
inline auto what() const noexcept -> char const* override { return m_msg.c_str(); }
private:
std::string const m_msg;
};
}}
#endif
#ifndef SCTF_TESTCASE_HPP
#define SCTF_TESTCASE_HPP
namespace sctf { namespace intern {
using test_function = std::function<void()>;
struct test_context final {
char const* tc_name;
char const* ts_name;
};
class testcase {
public:
testcase(testcase const&) = delete;
auto operator=(testcase const&) -> testcase& = delete;
~testcase() noexcept = default;
testcase(test_context&& ctx_, test_function&& fn_) : m_name(ctx_.tc_name), m_suite_name(ctx_.ts_name), m_test_func(std::move(fn_)) {}
testcase(testcase&& other_) noexcept : m_name(other_.m_name), m_suite_name(other_.m_suite_name), m_state(other_.m_state), m_duration(other_.m_duration), m_err_msg(std::move(other_.m_err_msg)), m_test_func(std::move(other_.m_test_func)) {}
auto operator=(testcase&& other_) noexcept -> testcase& {
m_name = other_.m_name;
m_suite_name = other_.m_suite_name;
m_state = other_.m_state;
m_duration = other_.m_duration;
m_err_msg = std::move(other_.m_err_msg);
m_test_func = std::move(other_.m_test_func);
return *this;
}
enum class result : std::int_fast8_t { NONE, PASSED, FAILED, ERROR };
void operator()() {
if(m_state != result::NONE) { return; }
class duration dur;
try {
m_test_func();
pass();
} catch(assertion_failure const& e) { fail(e.what()); } catch(std::exception const& e) {
error(e.what());
} catch(...) { error(); }
m_duration = dur.get();
}
inline auto state() const -> result { return m_state; }
inline auto duration() const -> double { return m_duration; }
inline auto reason() const -> std::string const& { return m_err_msg; }
inline auto name() const -> char const* { return m_name; }
inline auto suite_name() const -> char const* { return m_suite_name; }
inline void cout(std::string const& str_) { m_cout = str_; }
inline void cerr(std::string const& str_) { m_cerr = str_; }
inline auto cout() const -> std::string const& { return m_cout; }
inline auto cerr() const -> std::string const& { return m_cerr; }
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
char const* m_suite_name;
result m_state = result::NONE;
double m_duration = 0.0;
std::string m_err_msg;
std::string m_cout;
std::string m_cerr;
test_function m_test_func;
};
}}
#endif
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
streambuf_proxy_omp(streambuf_proxy_omp const&) = delete;
streambuf_proxy_omp(streambuf_proxy_omp&&) noexcept = delete;
auto operator=(streambuf_proxy_omp const&) -> streambuf_proxy_omp& = delete;
auto operator=(streambuf_proxy_omp&&) noexcept -> streambuf_proxy_omp& = delete;
explicit streambuf_proxy_omp(std::ostream& stream_) : m_orig_buf(stream_.rdbuf(this)), m_orig_stream(stream_), m_thd_buffers(static_cast<std::size_t>(omp_get_max_threads())) {}
~streambuf_proxy_omp() noexcept override { m_orig_stream.rdbuf(m_orig_buf); }
auto str() const -> std::string { return SCTF_INTERN_CURRENT_THREAD_BUFFER().str(); }
void clear() { SCTF_INTERN_CURRENT_THREAD_BUFFER().str(""); }
private:
auto overflow(int_type c_) -> int_type override { return SCTF_INTERN_CURRENT_THREAD_BUFFER().sputc(std::stringbuf::traits_type::to_char_type(c_)); }
auto xsputn(char const* s_, std::streamsize n_) -> std::streamsize override { return SCTF_INTERN_CURRENT_THREAD_BUFFER().sputn(s_, n_); }
std::streambuf* m_orig_buf;
std::ostream& m_orig_stream;
std::vector<std::stringbuf> m_thd_buffers;
};
}}
#ifndef _OPENMP
#undef omp_get_max_threads
#undef omp_get_thread_num
#endif
#undef SCTF_INTERN_CURRENT_THREAD_BUFFER
#endif
#ifndef SCTF_TESTSUITE_STREAMBUF_PROXY_HPP
#define SCTF_TESTSUITE_STREAMBUF_PROXY_HPP
namespace sctf { namespace intern {
class streambuf_proxy : public std::streambuf {
public:
streambuf_proxy(streambuf_proxy const&) = delete;
streambuf_proxy(streambuf_proxy&&) noexcept = delete;
auto operator=(streambuf_proxy const&) -> streambuf_proxy& = delete;
auto operator=(streambuf_proxy&&) noexcept -> streambuf_proxy& = delete;
explicit streambuf_proxy(std::ostream& stream_) : m_orig_buf(stream_.rdbuf(this)), m_orig_stream(stream_) {}
~streambuf_proxy() noexcept override { m_orig_stream.rdbuf(m_orig_buf); }
auto str() const -> std::string { return m_buffer.str(); }
void clear() { m_buffer.str(""); }
private:
auto overflow(int_type c_) -> int_type override { return m_buffer.sputc(std::stringbuf::traits_type::to_char_type(c_)); }
auto xsputn(char const* s_, std::streamsize n_) -> std::streamsize override { return m_buffer.sputn(s_, n_); }
std::streambuf* m_orig_buf;
std::ostream& m_orig_stream;
std::stringbuf m_buffer;
};
}}
#endif
#ifndef SCTF_TESTSUITE_STATISTICS_HPP
#define SCTF_TESTSUITE_STATISTICS_HPP
namespace sctf { namespace intern {
class statistic {
public:
inline auto tests() const -> std::size_t { return m_num_tests; }
inline auto successes() const -> std::size_t { return m_num_tests - m_num_errs - m_num_fails; }
inline auto failures() const -> std::size_t { return m_num_fails; }
inline auto errors() const -> std::size_t { return m_num_errs; }
private:
friend class testsuite;
friend class testsuite_parallel;
std::size_t m_num_tests = 0;
std::size_t m_num_fails = 0;
std::size_t m_num_errs = 0;
};
}}
#endif
#ifndef SCTF_TESTSUITE_TESTSUITE_HPP
#define SCTF_TESTSUITE_TESTSUITE_HPP
namespace sctf { namespace intern {
class testsuite;
using testsuite_ptr = std::shared_ptr<testsuite>;
class testsuite {
protected:
struct enable {};
public:
using hook_function = std::function<void()>;
testsuite(testsuite const&) = delete;
testsuite(testsuite&&) noexcept = delete;
auto operator=(testsuite const&) -> testsuite& = delete;
auto operator=(testsuite&&) noexcept -> testsuite& = delete;
virtual ~testsuite() noexcept = default;
static auto create(char const* name_) -> testsuite_ptr { return std::make_shared<testsuite>(enable{}, name_); }
virtual void run() {
if(m_state != execution_state::DONE) {
m_stats.m_num_tests = m_testcases.size();
streambuf_proxy buf_cout(std::cout);
streambuf_proxy buf_cerr(std::cerr);
m_setup_fn();
std::for_each(m_testcases.begin(), m_testcases.end(), [this, &buf_cerr, &buf_cout](testcase& tc_) {
if(tc_.state() == testcase::result::NONE) {
m_pretest_fn();
tc_();
switch(tc_.state()) {
case testcase::result::FAILED: ++m_stats.m_num_fails; break;
case testcase::result::ERROR: ++m_stats.m_num_errs; break;
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
void test(char const* name_, hook_function&& fn_) {
m_testcases.emplace_back(test_context{name_, m_name}, std::move(fn_));
m_state = execution_state::PENDING;
}
void setup(hook_function&& fn_) { m_setup_fn.fn = std::move(fn_); }
void teardown(hook_function&& fn_) { m_teardown_fn.fn = std::move(fn_); }
void before_each(hook_function&& fn_) { m_pretest_fn.fn = std::move(fn_); }
void after_each(hook_function&& fn_) { m_posttest_fn.fn = std::move(fn_); }
inline auto name() const -> char const* { return m_name; }
inline auto timestamp() const -> std::chrono::system_clock::time_point const& { return m_create_time; }
inline auto statistics() const -> statistic const& { return m_stats; }
inline auto execution_duration() const -> double { return m_exec_dur; }
inline auto testcases() const -> std::vector<testcase> const& { return m_testcases; }
explicit testsuite(enable, char const* name_) : m_name(name_), m_create_time(std::chrono::system_clock::now()) {}
protected:
struct silent_functor final {
auto operator()() -> silent_functor& {
if(fn) {
try {
fn();
} catch(...) {}
}
return *this;
}
hook_function fn;
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
}}
#endif
#ifndef SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
#define SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
namespace sctf { namespace intern {
class testsuite_parallel : public testsuite {
public:
testsuite_parallel(testsuite_parallel const&) = delete;
testsuite_parallel(testsuite_parallel&&) noexcept = delete;
auto operator=(testsuite_parallel const&) -> testsuite_parallel& = delete;
auto operator=(testsuite_parallel&&) noexcept -> testsuite_parallel& = delete;
~testsuite_parallel() noexcept override = default;
static auto create(char const* name_) -> testsuite_ptr { return std::make_shared<testsuite_parallel>(enable{}, name_); }
void run() override {
if(m_state != execution_state::DONE) {
if(m_testcases.size() > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max())) { throw std::overflow_error("Too many testcases! Size would overflow loop variant."); }
auto const tc_size = static_cast<std::int64_t>(m_testcases.size());
m_stats.m_num_tests = m_testcases.size();
streambuf_proxy_omp mt_buf_cout(std::cout);
streambuf_proxy_omp mt_buf_cerr(std::cerr);
m_setup_fn();
#pragma omp parallel default(shared)
{
double tmp = 0.0;
std::size_t fails = 0;
std::size_t errs = 0;
#pragma omp for schedule(dynamic)
for(std::int64_t i = 0; i < tc_size; ++i) {
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
{
m_stats.m_num_fails += fails;
m_stats.m_num_errs += errs;
m_exec_dur = std::max(m_exec_dur, tmp);
}
}
m_teardown_fn();
m_state = execution_state::DONE;
}
}
explicit testsuite_parallel(enable e_, char const* name_) : testsuite(e_, name_) {}
};
}}
#endif
#ifndef SCTF_RUNNER_HPP
#define SCTF_RUNNER_HPP
namespace sctf { namespace intern {
class runner {
public:
void add_testsuite(testsuite_ptr const& ts_) { m_testsuites.push_back(ts_); }
void run() noexcept {
std::for_each(m_testsuites.begin(), m_testsuites.end(), [](testsuite_ptr& ts_) { ts_->run(); });
}
auto testsuites() -> std::vector<testsuite_ptr> const& { return m_testsuites; }
static auto instance() -> runner& {
static runner r;
return r;
}
private:
std::vector<testsuite_ptr> m_testsuites;
};
}}
#endif
#ifndef SCTF_REPORTER_REPORTER_HPP
#define SCTF_REPORTER_REPORTER_HPP
namespace sctf {
namespace intern {
class reporter : public std::enable_shared_from_this<reporter> {
public:
reporter(reporter const&) = delete;
reporter(reporter&&) noexcept = delete;
auto operator=(reporter const&) -> reporter& = delete;
auto operator=(reporter&&) noexcept -> reporter& = delete;
virtual ~reporter() noexcept = default;
auto report() -> std::size_t {
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
struct enable {};
explicit reporter(std::ostream& stream_) : m_out_stream(stream_) {}
explicit reporter(char const* fname_) : m_out_file(fname_), m_out_stream(m_out_file) {
if(!m_out_stream) { throw std::runtime_error("Could not open file."); }
}
virtual void report_testsuite(testsuite_ptr const& ts_) {
std::for_each(ts_->testcases().begin(), ts_->testcases().end(), [this](const testcase& tc) { report_testcase(tc); });
}
virtual void report_testcase(testcase const& tc_) = 0;
virtual void begin_report() = 0;
virtual void end_report() = 0;
template<typename T> auto operator<<(T const& t_) -> std::ostream& {
m_out_stream << t_;
return m_out_stream;
}
std::ofstream m_out_file;
std::ostream& m_out_stream;
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
}
}
using reporter_ptr = std::shared_ptr<intern::reporter>;
}
#endif
#ifndef SCTF_REPORTER_XML_REPORTER_HPP
#define SCTF_REPORTER_XML_REPORTER_HPP
namespace sctf {
class xml_reporter : public intern::reporter {
public:
xml_reporter(xml_reporter const&) = delete;
xml_reporter(xml_reporter&&) noexcept = delete;
auto operator=(xml_reporter const&) -> xml_reporter& = delete;
auto operator=(xml_reporter&&) noexcept -> xml_reporter& = delete;
~xml_reporter() noexcept override = default;
static auto create(std::ostream& stream_ = std::cout) -> std::shared_ptr<xml_reporter> { return std::make_shared<xml_reporter>(enable{}, stream_); }
static auto create(char const* fname_) -> std::shared_ptr<xml_reporter> { return std::make_shared<xml_reporter>(enable{}, fname_); }
auto with_captured_output() -> std::shared_ptr<xml_reporter> {
m_capture = true;
return std::static_pointer_cast<xml_reporter>(shared_from_this());
}
explicit xml_reporter(enable, std::ostream& stream_) : reporter(stream_) {}
explicit xml_reporter(enable, char const* fname_) : reporter(fname_) {}
private:
void report_testsuite(intern::testsuite_ptr const& ts_) override {
std::time_t stamp = std::chrono::system_clock::to_time_t(ts_->timestamp());
std::array<char, 128> buff{};
std::strftime(buff.data(), 127, "%FT%T", std::localtime(&stamp));
*this << intern::fmt::SPACE << "<testsuite id=\"" << m_id++ << "\" name=\"" << ts_->name() << "\" errors=\"" << ts_->statistics().errors() << "\" tests=\"" << ts_->statistics().tests() << "\" failures=\"" << ts_->statistics().failures() << R"(" skipped="0" time=")" << ts_->execution_duration() << "\" timestamp=\"" << buff.data() << "\">" << intern::fmt::LF;
reporter::report_testsuite(ts_);
*this << intern::fmt::SPACE << "</testsuite>" << intern::fmt::LF;
}
void report_testcase(intern::testcase const& tc_) override {
*this << intern::fmt::XSPACE << "<testcase name=\"" << tc_.name() << "\" classname=\"" << tc_.suite_name() << "\" time=\"" << tc_.duration() << "\"";
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
void begin_report() override { *this << R"(<?xml version="1.0" encoding="UTF-8" ?>)" << intern::fmt::LF << "<testsuites>" << intern::fmt::LF; }
void end_report() override { *this << "</testsuites>" << intern::fmt::LF; }
void print_system_out(intern::testcase const& tc_) {
*this << intern::fmt::XSPACE << intern::fmt::SPACE << "<system-out>" << tc_.cout() << "</system-out>" << intern::fmt::LF;
*this << intern::fmt::XSPACE << intern::fmt::SPACE << "<system-err>" << tc_.cerr() << "</system-err>" << intern::fmt::LF;
}
std::size_t mutable m_id = 0;
bool m_capture = false;
};
}
#endif
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
}}
class console_reporter : public intern::reporter {
public:
console_reporter(console_reporter const&) = delete;
console_reporter(console_reporter&&) noexcept = delete;
auto operator=(console_reporter const&) -> console_reporter& = delete;
auto operator=(console_reporter&&) noexcept -> console_reporter& = delete;
~console_reporter() noexcept override = default;
static auto create(std::ostream& stream_ = std::cout) -> std::shared_ptr<console_reporter> { return std::make_shared<console_reporter>(enable{}, stream_); }
static auto create(char const* fname_) -> std::shared_ptr<console_reporter> { return std::make_shared<console_reporter>(enable{}, fname_); }
auto with_color() -> std::shared_ptr<console_reporter> {
m_color = true;
return std::static_pointer_cast<console_reporter>(shared_from_this());
}
auto with_captured_output() -> std::shared_ptr<console_reporter> {
m_capture = true;
return std::static_pointer_cast<console_reporter>(shared_from_this());
}
explicit console_reporter(enable, std::ostream& stream_) : reporter(stream_) {}
explicit console_reporter(enable, char const* fname_) : reporter(fname_) {}
private:
void report_testsuite(intern::testsuite_ptr const& ts_) override {
*this << "Run Testsuite [" << ts_->name() << "]; time = " << ts_->execution_duration() << "ms" << intern::fmt::LF;
reporter::report_testsuite(ts_);
}
void report_testcase(intern::testcase const& tc_) override {
*this << intern::fmt::SPACE << "Run Testcase [" << tc_.name() << "](" << tc_.suite_name() << "); time = " << tc_.duration() << "ms" << intern::fmt::LF << intern::fmt::XSPACE;
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
bool m_color = false;
bool m_capture = false;
};
}
#endif
#ifndef SCTF_REPORTER_MARKDOWN_REPORTER_HPP
#define SCTF_REPORTER_MARKDOWN_REPORTER_HPP
namespace sctf {
class markdown_reporter : public intern::reporter {
public:
markdown_reporter(markdown_reporter const&) = delete;
markdown_reporter(markdown_reporter&&) noexcept = delete;
auto operator=(markdown_reporter const&) -> markdown_reporter& = delete;
auto operator=(markdown_reporter&&) noexcept -> markdown_reporter& = delete;
~markdown_reporter() noexcept override = default;
static auto create(std::ostream& stream_ = std::cout) -> std::shared_ptr<markdown_reporter> { return std::make_shared<markdown_reporter>(enable{}, stream_); }
static auto create(char const* fname_) -> std::shared_ptr<markdown_reporter> { return std::make_shared<markdown_reporter>(enable{}, fname_); }
auto with_captured_output() -> std::shared_ptr<markdown_reporter> {
m_capture = true;
return std::static_pointer_cast<markdown_reporter>(shared_from_this());
}
explicit markdown_reporter(enable, std::ostream& stream_) : reporter(stream_) {}
explicit markdown_reporter(enable, char const* fname_) : reporter(fname_) {}
private:
void report_testsuite(intern::testsuite_ptr const& ts_) override {
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
*this << "|" << tc_.name() << "|" << tc_.suite_name() << "|" << tc_.duration() << "ms|" << status << "|";
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
bool m_capture = false;
};
}
#endif
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
comparison(char const* comp_str_, std::tuple<std::string&&, std::string&&>&& val_) : m_failure("Expected " + std::get<0>(val_) + " " + comp_str_ + " " + std::get<1>(val_)) {}
explicit operator bool() { return !m_failure; }
auto operator*() const -> std::string const& { return *m_failure; }
private:
std::optional<std::string> const m_failure;
#else
comparison() = default;
comparison(char const* comp_str_, std::tuple<std::string&&, std::string&&>&& val_) : m_success(false) {
std::string msg;
msg.reserve(15 + std::strlen(comp_str_) + std::get<0>(val_).length() + std::get<1>(val_).length());
msg = "Expected ";
msg.append(std::get<0>(val_)).append(" ").append(comp_str_).append(" ").append(std::get<1>(val_));
error() = msg;
}
explicit operator bool() const { return m_success; }
auto operator*() const -> std::string const& { return error(); }
private:
bool const m_success = true;
static auto error() -> std::string& {
static thread_local std::string err_msg;
return err_msg;
}
#endif
};
}}
#define SCTF_COMPARATOR(NAME, CMPSTR, PRED) \
namespace sctf { namespace intern { \
class NAME { \
static constexpr char const* m_cmp_str = "to be " CMPSTR; \
static constexpr char const* m_neg_cmp_str = "to be not " CMPSTR; \
bool m_neg = false; \
public: \
auto operator!() -> NAME& { \
m_neg = !m_neg; \
return *this; \
} \
template<typename V, typename E = V> auto operator()(V const& actual_value, E const& expected_value) const -> comparison { return (PRED) != m_neg ? comparison() : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, std::forward_as_tuple(to_string(actual_value), to_string(expected_value))); } \
}; \
} \
}
#define SCTF_PROVIDE_COMPARATOR(COMP, NAME) \
namespace sctf { \
template<typename... Args> static auto NAME(Args&&... args) -> intern::COMP { return intern::COMP(std::forward<Args>(args)...); } \
}
#endif
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
#endif
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
explicit f_equals(double eps_) : m_eps(eps_) {}
auto operator!() -> f_equals& {
m_neg = !m_neg;
return *this;
}
template<typename V, typename E = V> auto operator()(V const& actual_value, E const& expected_value) const -> comparison {
static_assert(SCTF_INTERN_IS_FLOAT(V) && SCTF_INTERN_IS_FLOAT(E), "The floating point comparator must not be used with other types than float, or double!");
typename std::decay<V>::type epsilon_ = static_cast<V>(m_eps);
return (std::abs(actual_value - expected_value) <= std::max(std::abs(actual_value), std::abs(expected_value)) * epsilon_) != m_neg ? comparison() : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, std::forward_as_tuple(to_string(actual_value), to_string(expected_value)));
}
};
}
}
SCTF_PROVIDE_COMPARATOR(f_equals, F_EQUALS)
SCTF_PROVIDE_COMPARATOR(f_equals, FEQ)
#endif
#ifndef SCTF_COMPARATOR_RANGE_HPP
#define SCTF_COMPARATOR_RANGE_HPP
namespace sctf { namespace intern {
class in_range {
static constexpr char const* m_cmp_str = "to be in range of";
static constexpr char const* m_neg_cmp_str = "to be not in range of";
bool m_neg = false;
public:
auto operator!() -> in_range& {
m_neg = !m_neg;
return *this;
}
template<typename V, typename E = V, SCTF_INTERN_ENABLE_IF(SCTF_INTERN_HAS_ITERATOR_CAPABILITY(E) && !SCTF_INTERN_IS_TYPE(E, std::string))> auto operator()(V const& actual_value, E const& expected_value) const -> comparison { return (std::find(expected_value.cbegin(), expected_value.cend(), actual_value) != expected_value.cend()) != m_neg ? comparison() : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, std::forward_as_tuple(to_string(actual_value), to_string(expected_value))); }
template<typename V, typename E = V, SCTF_INTERN_ENABLE_IF(SCTF_INTERN_IS_TYPE(E, std::string))> auto operator()(V const& actual_value, E const& expected_value) const -> comparison { return (expected_value.find(actual_value) != std::string::npos) != m_neg ? comparison() : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, std::forward_as_tuple(to_string(actual_value), to_string(expected_value))); }
};
}}
SCTF_PROVIDE_COMPARATOR(in_range, IN_RANGE)
SCTF_PROVIDE_COMPARATOR(in_range, IN)
#endif
#ifndef SCTF_COMPARATOR_REGEX_HPP
#define SCTF_COMPARATOR_REGEX_HPP
SCTF_COMPARATOR(match, "matching", std::regex_match(actual_value, std::regex(expected_value)))
SCTF_PROVIDE_COMPARATOR(match, MATCH)
SCTF_COMPARATOR(like, "like", std::regex_search(actual_value, std::regex(expected_value)))
SCTF_PROVIDE_COMPARATOR(like, LIKE)
#endif
#ifndef SCTF_ASSERT_HPP
#define SCTF_ASSERT_HPP
#define ASSERT(VAL, CMP, EXP) sctf::intern::assert_statement(std::forward_as_tuple(CMP, VAL, EXP), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_NOT(VAL, CMP, EXP) sctf::intern::assert_statement(std::forward_as_tuple(!CMP, VAL, EXP), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_EQ(VAL, EXP) sctf::intern::assert_statement(std::forward_as_tuple(sctf::EQUALS(), VAL, EXP), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_TRUE(VAL) sctf::intern::assert_statement(std::forward_as_tuple(sctf::EQUALS(), VAL, true), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_FALSE(VAL) sctf::intern::assert_statement(std::forward_as_tuple(sctf::EQUALS(), VAL, false), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_NULL(PTR) sctf::intern::assert_statement(std::forward_as_tuple(sctf::EQUALS(), static_cast<void const*>(PTR), nullptr), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_NOT_NULL(PTR) sctf::intern::assert_statement(std::forward_as_tuple(!sctf::EQUALS(), static_cast<void const*>(PTR), nullptr), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_ZERO(VAL) sctf::intern::assert_statement(std::forward_as_tuple(sctf::EQUALS(), VAL, static_cast<decltype(VAL)>(0)), sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_THROWS(FN, TRW) sctf::intern::assert_throws<TRW>([&] { FN; }, #TRW, sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_NOTHROW(FN) sctf::intern::assert_nothrow([&] { FN; }, sctf::intern::loc{__FILE__, __LINE__})
#define ASSERT_RUNTIME(FN, MAX) sctf::intern::assert_runtime([&] { FN; }, MAX, sctf::intern::loc{__FILE__, __LINE__})
namespace sctf { namespace intern {
template<typename S> void assert_statement(S&& stmt_, loc const& loc_) {
comparison res = std::get<0>(stmt_)(std::get<1>(stmt_), std::get<2>(stmt_));
if(!res) { throw assertion_failure(*res, loc_); }
}
template<typename T, typename F> void assert_throws(F&& fn_, char const* tname_, loc const& loc_) {
try {
fn_();
} catch(T const&) { return; } catch(std::exception const& e) {
throw assertion_failure("Wrong exception thrown, caught " + to_string(e), loc_);
} catch(...) { throw assertion_failure("Wrong exception thrown", loc_); }
throw assertion_failure(std::string("No exception thrown, expected ") + tname_, loc_);
}
template<typename F> void assert_nothrow(F&& fn_, loc const& loc_) {
try {
fn_();
} catch(std::exception const& e) { throw assertion_failure("Expected no exception, caught " + to_string(e), loc_); } catch(...) {
throw assertion_failure("Expected no exception", loc_);
}
}
template<typename F> void assert_runtime(F&& fn_, double max_ms_, loc const& loc_) {
try {
duration dur;
fn_();
double dur_ms = dur.get();
if(dur_ms > max_ms_) { throw assertion_failure("runtime > " + to_string(max_ms_) + "ms", loc_); }
} catch(std::exception const& e) { throw assertion_failure(e.what(), loc_); } catch(...) {
throw assertion_failure("Unknown exception thrown", loc_);
}
}
}}
#endif
#ifndef SCTF_API_HPP
#define SCTF_API_HPP
namespace sctf { namespace intern {
template<typename T> struct singleton final {
template<typename... Args> static auto instance(Args&&... args_) noexcept -> T const& {
static T const inst(std::forward<Args>(args_)...);
return inst;
}
};
}}
#define SCTF_INTERN_CONCAT3(A, B, C) A##B##C
#define SCTF_INTERN_API_TEST_NAME(ID) SCTF_INTERN_CONCAT3(sctf_intern_test_, ID, _)
#define SCTF_INTERN_API_TEST_INST(ID) SCTF_INTERN_CONCAT3(sctf_intern_test_, ID, _inst_)
#define SCTF_INTERN_API_TEST_FN(ID) SCTF_INTERN_CONCAT3(sctf_intern_test_fn_, ID, _)
#define SCTF_INTERN_API_SUITE_NS(ID) SCTF_INTERN_CONCAT3(sctf_intern_ns_, ID, _)
#define SCTF_INTERN_API_SUITE_NAME(ID) SCTF_INTERN_CONCAT3(sctf_intern_suite_, ID, _)
#define SCTF_INTERN_API_SUITE_WRAPPER(DESCR, BASE) \
namespace SCTF_INTERN_API_SUITE_NS(__LINE__) { \
class test_module { \
sctf::intern::testsuite_ptr m_ts_; \
public: \
test_module(test_module const&) = delete; \
test_module(test_module&&) noexcept = delete; \
auto operator=(test_module const&) -> test_module& = delete; \
auto operator=(test_module&&) noexcept -> test_module& = delete; \
virtual ~test_module() noexcept = default; \
protected: \
test_module() : m_ts_(sctf::intern::BASE::create(DESCR)) { sctf::intern::runner::instance().add_testsuite(m_ts_); } \
auto sctf_intern_ts_() const -> sctf::intern::testsuite_ptr const& { return m_ts_; } \
}; \
class SCTF_INTERN_API_SUITE_NAME(__LINE__); \
static auto const& sctf_intern_mod_ = sctf::intern::singleton<SCTF_INTERN_API_SUITE_NAME(__LINE__)>::instance(); \
using sctf_intern_mod_type_ = SCTF_INTERN_API_SUITE_NAME(__LINE__); \
} \
class SCTF_INTERN_API_SUITE_NS(__LINE__)::SCTF_INTERN_API_SUITE_NAME(__LINE__) : public SCTF_INTERN_API_SUITE_NS(__LINE__)::test_module
#define SCTF_INTERN_API_TEST_WRAPPER(DESCR) \
class SCTF_INTERN_API_TEST_NAME(__LINE__) { \
public: \
explicit SCTF_INTERN_API_TEST_NAME(__LINE__)(sctf_intern_mod_type_ * mod_) { \
mod_->sctf_intern_ts_()->test(DESCR, [=] { mod_->SCTF_INTERN_API_TEST_FN(__LINE__)(); }); \
} \
} SCTF_INTERN_API_TEST_INST(__LINE__){this}; \
void SCTF_INTERN_API_TEST_FN(__LINE__)()
#define SCTF_INTERN_API_FN_WRAPPER(FN) \
class sctf_intern_##FN##_ { \
public: \
explicit sctf_intern_##FN##_(sctf_intern_mod_type_* mod_) { \
mod_->sctf_intern_ts_()->FN([=] { mod_->sctf_intern_##FN##_fn_(); }); \
} \
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
#endif
#ifndef SCTF_SCTF_HPP
#define SCTF_SCTF_HPP
#define SCFT_VERSION "2.0-rc8"
#define SCTF_DEFAULT_MAIN(R) \
auto main(int, char**)->int { return static_cast<int>(sctf::R->report()); }
#endif
#endif
