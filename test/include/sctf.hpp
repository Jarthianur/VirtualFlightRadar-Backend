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
class reporter;
using test_function = std::function<void()>;
struct code_location final {
char const* file;
int const line;
};
template<typename T> struct singleton final {
template<typename... Args> static T& instance(Args&&... args_) {
static T instance(std::forward<Args>(args_)...);
return instance;
}
};
}// namespace _
class testsuite;
using reporter_ptr = std::shared_ptr<_::reporter>;
using testsuite_ptr = std::shared_ptr<testsuite>;
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
class duration final {
public:
duration() : m_start(std::chrono::steady_clock::now()) {}
~duration() noexcept = default;
double get() { return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_start).count(); }
private:
std::chrono::steady_clock::time_point const m_start;
};
}}// namespace sctf::_
#endif// SCTF_COMMON_DURATION_HPP
#ifndef SCTF_COMMON_STRINGIFY_HPP
#define SCTF_COMMON_STRINGIFY_HPP
namespace sctf { namespace _ {
template<typename T> static char const* name_for_type() {
static thread_local std::string name;
if(name.length() > 0) { return name.c_str(); }
#if defined(__GNUG__) || defined(__clang__)
std::string const sig(__PRETTY_FUNCTION__);
auto const b = sig.rfind("T = ") + 4;
name = sig.substr(b, sig.rfind(']') - b);
name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
#else
std::string const sig(typeid(T).name());
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
template<typename T, ENABLE_IF(IS_STREAMABLE(T, std::ostringstream) AND NOT IS_FLOAT(T))> std::string to_string(T const& arg_) {
std::ostringstream oss;
oss << arg_;
return oss.str();
}
template<typename T, ENABLE_IF(IS_STREAMABLE(T, std::ostringstream) AND IS_FLOAT(T))> std::string to_string(T const& arg_) {
std::ostringstream oss;
oss << std::setprecision(std::numeric_limits<T>::max_digits10) << arg_;
return oss.str();
}
template<typename T, ENABLE_IF(NOT IS_STREAMABLE(T, std::ostringstream))> std::string to_string(T const&) { return name_for_type<T>(); }
inline std::string to_string(std::nullptr_t const&) { return "0"; }
inline std::string to_string(bool const& arg) { return arg ? "true" : "false"; }
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
streambuf_proxy_omp(std::ostream& stream_) : m_orig_buf(stream_.rdbuf(this)), m_orig_stream(stream_), m_thd_buffers(static_cast<std::size_t>(omp_get_max_threads())) {}
virtual ~streambuf_proxy_omp() noexcept override { m_orig_stream.rdbuf(m_orig_buf); }
std::string str() const { return CURRENT_THREAD_BUFFER().str(); }
void clear() { CURRENT_THREAD_BUFFER().str(""); }
protected:
virtual int_type overflow(int_type c_) override { return CURRENT_THREAD_BUFFER().sputc(std::stringbuf::traits_type::to_char_type(c_)); }
virtual std::streamsize xsputn(char const* s_, std::streamsize n_) override { return CURRENT_THREAD_BUFFER().sputn(s_, n_); }
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
}}// namespace sctf::_
#endif// SCTF_COMMON_STREAMBUF_PROXY_HPP
#ifndef SCTF_COMMON_ASSERTION_FAILURE_HPP
#define SCTF_COMMON_ASSERTION_FAILURE_HPP
namespace sctf { namespace _ {
class assertion_failure : public std::exception {
public:
assertion_failure(std::string const& msg_, code_location const& loc_) : m_msg(msg_ + " at " + loc_.file + ":" + std::to_string(loc_.line)) {}
virtual ~assertion_failure() noexcept override = default;
inline const char* what() const noexcept override { return m_msg.c_str(); }
protected:
std::string const m_msg;
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
testcase(testcase const&) = delete;
testcase& operator=(testcase const&) = delete;
~testcase() noexcept = default;
testcase(char const* name_, char const* ctx_, test_function&& fn_) : m_name(name_), m_context(ctx_), m_test_func(std::move(fn_)) {}
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
enum class result : std::int_fast8_t {
NONE,///< not yet executed
PASSED,
FAILED,
ERROR
};
void operator()() {
if(m_state != result::NONE) return;
class duration dur;
try {
m_test_func();
pass();
} catch(assertion_failure const& e) { fail(e.what()); } catch(std::exception const& e) {
erroneous(e.what());
} catch(...) { erroneous(); }
m_duration = dur.get();
}
inline result state() const { return m_state; }
inline double duration() const { return m_duration; }
inline std::string const& err_msg() const { return m_err_msg; }
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
inline void erroneous(char const* err_ = "unknown error") {
m_state = result::ERROR;
m_err_msg = err_;
}
char const* m_name;
char const* m_context;
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
static testsuite_ptr create(char const* name_, char const* ctx_) { return testsuite_ptr(new testsuite(name_, ctx_)); }
virtual void run() {
if(m_state != execution_state::DONE) {
m_stats.m_num_of_tests = m_testcases.size();
_::streambuf_proxy buf_cout(std::cout);
_::streambuf_proxy buf_cerr(std::cerr);
SCTF_EXEC_SILENT(m_setup_fn)
std::for_each(m_testcases.begin(), m_testcases.end(), [this, &buf_cerr, &buf_cout](_::testcase& tc_) {
if(tc_.state() == _::testcase::result::NONE) {
SCTF_EXEC_SILENT(m_pretest_fn)
tc_();
switch(tc_.state()) {
case _::testcase::result::FAILED: ++m_stats.m_num_of_fails; break;
case _::testcase::result::ERROR: ++m_stats.m_num_of_errs; break;
default: break;
}
m_time += tc_.duration();
SCTF_EXEC_SILENT(m_posttest_fn)
tc_.cout(buf_cout.str());
tc_.cerr(buf_cerr.str());
buf_cout.clear();
buf_cerr.clear();
}
});
SCTF_EXEC_SILENT(m_teardown_fn)
m_state = execution_state::DONE;
}
}
template<typename T> testsuite_ptr test(char const* name_, _::test_function&& fn_) {
m_testcases.push_back(_::testcase(name_, _::name_for_type<T>(), std::move(fn_)));
m_state = execution_state::PENDING;
return shared_from_this();
}
testsuite_ptr test(char const* name_, char const* ctx_, _::test_function&& fn_) {
m_testcases.push_back(_::testcase(name_, ctx_, std::move(fn_)));
m_state = execution_state::PENDING;
return shared_from_this();
}
testsuite_ptr test(char const* name_, _::test_function&& fn_) {
m_testcases.push_back(_::testcase(name_, m_context, std::move(fn_)));
m_state = execution_state::PENDING;
return shared_from_this();
}
testsuite_ptr setup(_::test_function&& fn_) {
m_setup_fn = std::move(fn_);
return shared_from_this();
}
testsuite_ptr teardown(_::test_function&& fn_) {
m_teardown_fn = std::move(fn_);
return shared_from_this();
}
testsuite_ptr before_each(_::test_function&& fn_) {
m_pretest_fn = std::move(fn_);
return shared_from_this();
}
testsuite_ptr after_each(_::test_function&& fn_) {
m_posttest_fn = std::move(fn_);
return shared_from_this();
}
inline char const* name() const { return m_name; }
inline std::chrono::system_clock::time_point const& timestamp() const { return m_timestamp; }
inline _::statistics const& statistics() const { return m_stats; }
inline double time() const { return m_time; }
inline std::vector<_::testcase> const& testcases() const { return m_testcases; }
protected:
enum class execution_state : std::int_fast8_t { PENDING, DONE };
testsuite(char const* name_, char const* ctx_) : m_name(name_), m_context(ctx_), m_timestamp(std::chrono::system_clock::now()) {}
char const* m_name;
char const* m_context;
std::chrono::system_clock::time_point const m_timestamp;
double m_time = 0.0;
_::statistics m_stats;
std::vector<_::testcase> m_testcases;
execution_state m_state = execution_state::PENDING;
_::test_function m_setup_fn;
_::test_function m_teardown_fn;
_::test_function m_pretest_fn;
_::test_function m_posttest_fn;
};
}// namespace sctf
#endif// SCTF_TESTSUITE_TESTSUITE_HPP
#ifndef SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
#define SCTF_TESTSUITE_TESTSUITE_PARALLEL_HPP
namespace sctf {
class testsuite_parallel : public testsuite {
public:
testsuite_parallel(testsuite_parallel const&) = delete;
testsuite_parallel& operator=(testsuite_parallel const&) = delete;
~testsuite_parallel() noexcept override = default;
static testsuite_ptr create(char const* name_, char const* ctx_) { return testsuite_ptr(new testsuite_parallel(name_, ctx_)); }
void run() override {
if(m_state != execution_state::DONE) {
if(m_testcases.size() > std::numeric_limits<long>::max()) { throw std::overflow_error("Too many testcases! Size would overflow loop variant."); }
const long tc_size = static_cast<long>(m_testcases.size());
m_stats.m_num_of_tests = m_testcases.size();
_::streambuf_proxy_omp mt_buf_cout(std::cout);
_::streambuf_proxy_omp mt_buf_cerr(std::cerr);
SCTF_EXEC_SILENT(m_setup_fn)
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
SCTF_EXEC_SILENT(m_pretest_fn)
tc();
switch(tc.state()) {
case _::testcase::result::FAILED: ++fails; break;
case _::testcase::result::ERROR: ++errs; break;
default: break;
}
tmp += tc.duration();
SCTF_EXEC_SILENT(m_posttest_fn)
tc.cout(mt_buf_cout.str());
tc.cerr(mt_buf_cerr.str());
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
SCTF_EXEC_SILENT(m_teardown_fn)
m_state = execution_state::DONE;
}
}
private:
testsuite_parallel(char const* name_, char const* ctx_) : testsuite(name_, ctx_) {}
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
testsuite_ptr add_testsuite(testsuite_ptr ts_) {
m_testsuites.push_back(ts_);
return ts_;
}
void run() noexcept {
std::for_each(m_testsuites.begin(), m_testsuites.end(), [](testsuite_ptr& ts_) { ts_->run(); });
}
std::vector<testsuite_ptr> const& testsuites() { return m_testsuites; }
static runner& default_instance() {
static runner r;
return r;
}
private:
std::vector<testsuite_ptr> m_testsuites;
};
inline static testsuite_ptr suite_par(char const* name_, char const* ctx_ = "main", runner& runner_ = runner::default_instance()) { return runner_.add_testsuite(testsuite_parallel::create(name_, ctx_)); }
template<typename T> static testsuite_ptr suite_par(char const* name_, runner& runner_ = runner::default_instance()) { return runner_.add_testsuite(testsuite_parallel::create(name_, _::name_for_type<T>())); }
inline static testsuite_ptr suite(char const* name_, char const* ctx_ = "main", runner& runner_ = runner::default_instance()) { return runner_.add_testsuite(testsuite::create(name_, ctx_)); }
template<typename T> static testsuite_ptr suite(char const* name_, runner& runner_ = runner::default_instance()) { return runner_.add_testsuite(testsuite::create(name_, _::name_for_type<T>())); }
}// namespace sctf
#endif// SCTF_TESTSUITE_RUNNER_HPP
#ifndef SCTF_TESTSUITE_TESTMODULE_HPP
#define SCTF_TESTSUITE_TESTMODULE_HPP
namespace sctf { namespace _ {
class test_module {
protected:
test_module(testsuite_ptr const& ts_) : m_ts(ts_) {}
virtual ~test_module() noexcept = default;
inline void test(char const* name_, _::test_function&& fn_) { m_ts->test(name_, std::move(fn_)); }
inline void setup(_::test_function&& fn_) { m_ts->setup(std::move(fn_)); }
inline void teardown(_::test_function&& fn_) { m_ts->teardown(std::move(fn_)); }
inline void before_each(_::test_function&& fn_) { m_ts->before_each(std::move(fn_)); }
inline void after_each(_::test_function&& fn_) { m_ts->after_each(std::move(fn_)); }
testsuite_ptr m_ts;
};
}}// namespace sctf::_
#define TEST_MODULE(NAME, FN, ...) \
class NAME : public sctf::_::test_module { \
public: \
NAME() : sctf::_::test_module(sctf::suite(#NAME, #NAME, ##__VA_ARGS__)) { FN; } \
~NAME() noexcept override = default; \
}; \
namespace sctf { namespace _ { \
static const auto& mods_##NAME = singleton<NAME>::instance(); \
} \
}
#define TEST_MODULE_PAR(NAME, FN, ...) \
class NAME : public sctf::_::test_module { \
public: \
NAME() : sctf::_::test_module(sctf::suite_par(#NAME, #NAME, ##__VA_ARGS__)) { FN; } \
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
class reporter {
public:
virtual ~reporter() noexcept = default;
std::size_t report(runner& runner_ = runner::default_instance()) {
m_abs_errs = 0;
m_abs_fails = 0;
m_abs_tests = 0;
m_abs_time = 0.0;
runner_.run();
begin_report();
std::for_each(runner_.testsuites().begin(), runner_.testsuites().end(), [this](testsuite_ptr const& ts_) {
m_abs_errs += ts_->statistics().errors();
m_abs_fails += ts_->statistics().failures();
m_abs_tests += ts_->statistics().tests();
m_abs_time += ts_->time();
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
inline virtual void report_testsuite(testsuite_ptr const ts_) {
std::for_each(ts_->testcases().begin(), ts_->testcases().end(), [this](const _::testcase& tc) { report_testcase(tc); });
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
}}// namespace sctf::_
#endif// SCTF_REPORTER_ABSTRACT_REPORTER_HPP
#ifndef SCTF_REPORTER_XML_REPORTER_HPP
#define SCTF_REPORTER_XML_REPORTER_HPP
namespace sctf {
class xml_reporter : public _::reporter {
public:
~xml_reporter() noexcept override = default;
explicit xml_reporter(std::ostream& stream_) : reporter(stream_) {}
explicit xml_reporter(char const* fname_) : reporter(fname_) {}
protected:
void report_testsuite(testsuite_ptr const ts_) override {
std::time_t stamp = std::chrono::system_clock::to_time_t(ts_->timestamp());
char buff[128];
std::strftime(buff, 127, "%FT%T", std::localtime(&stamp));
*this << SCTF_SPACE << "<testsuite id=\"" << m_id++ << "\" name=\"" << ts_->name() << "\" errors=\"" << ts_->statistics().errors() << "\" tests=\"" << ts_->statistics().tests() << "\" failures=\"" << ts_->statistics().failures() << "\" skipped=\"0\" time=\"" << ts_->time() << "\" timestamp=\"" << buff << "\">" << SCTF_LF;
reporter::report_testsuite(ts_);
*this << SCTF_SPACE << "</testsuite>" << SCTF_LF;
}
void report_testcase(_::testcase const& tc_) override {
*this << SCTF_XSPACE << "<testcase name=\"" << tc_.name() << "\" classname=\"" << tc_.context() << "\" time=\"" << tc_.duration() << "\"";
switch(tc_.state()) {
case _::testcase::result::ERROR: *this << ">" << SCTF_LF << SCTF_XSPACE << SCTF_SPACE << "<error message=\"" << tc_.err_msg() << "\"></error>" << SCTF_LF << SCTF_XSPACE << "</testcase>"; break;
case _::testcase::result::FAILED: *this << ">" << SCTF_LF << SCTF_XSPACE << SCTF_SPACE << "<failure message=\"" << tc_.err_msg() << "\"></failure>" << SCTF_LF << SCTF_XSPACE << "</testcase>"; break;
case _::testcase::result::PASSED: *this << "/>"; break;
default: break;
}
*this << SCTF_LF;
}
void begin_report() override { *this << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << SCTF_LF << "<testsuites>" << SCTF_LF; }
void end_report() override { *this << "</testsuites>" << SCTF_LF; }
std::size_t mutable m_id = 0;
};
static reporter_ptr create_xml_reporter(std::ostream& stream_ = std::cout) { return std::make_shared<xml_reporter>(stream_); }
static reporter_ptr create_xml_reporter(char const* fname_) { return std::make_shared<xml_reporter>(fname_); }
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
class console_reporter : public _::reporter {
public:
~console_reporter() noexcept override = default;
explicit console_reporter(std::ostream& stream_, bool color_ = false, bool capture_ = false) : reporter(stream_), m_color(color_), m_capture(capture_) {}
explicit console_reporter(char const* fname_, bool color_ = false, bool capture_ = false) : reporter(fname_), m_color(color_), m_capture(capture_) {}
protected:
void report_testsuite(testsuite_ptr const ts_) override {
*this << "Run Testsuite [" << ts_->name() << "]; time = " << ts_->time() << "ms" << SCTF_LF;
reporter::report_testsuite(ts_);
}
void report_testcase(_::testcase const& tc_) override {
*this << SCTF_SPACE << "Run Testcase [" << tc_.name() << "](" << tc_.context() << "); time = " << tc_.duration() << "ms" << SCTF_LF << SCTF_XSPACE;
if(m_capture) {
*this << "stdout = '" << tc_.cout() << '\'' << SCTF_LF << SCTF_XSPACE;
*this << "stderr = '" << tc_.cerr() << '\'' << SCTF_LF << SCTF_XSPACE;
}
switch(tc_.state()) {
case _::testcase::result::ERROR: *this << (m_color ? ANSI_MAGENTA : "") << "ERROR! " << tc_.err_msg(); break;
case _::testcase::result::FAILED: *this << (m_color ? ANSI_RED : "") << "FAILED! " << tc_.err_msg(); break;
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
bool m_capture;
};
static reporter_ptr create_console_reporter(std::ostream& stream_, bool color_ = false, bool capture_ = false) { return std::make_shared<console_reporter>(stream_, color_, capture_); }
static reporter_ptr create_console_reporter(bool color_ = false, bool capture_ = false) { return std::make_shared<console_reporter>(std::cout, color_, capture_); }
static reporter_ptr create_console_reporter(char const* fname_, bool color_ = false, bool capture_ = false) { return std::make_shared<console_reporter>(fname_, color_, capture_); }
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
#define SCTF_XLF (SCTF_LF SCTF_LF)
namespace sctf {
class markdown_reporter : public _::reporter {
public:
~markdown_reporter() noexcept override = default;
explicit markdown_reporter(std::ostream& stream_) : reporter(stream_) {}
explicit markdown_reporter(char const* fname_) : reporter(fname_) {}
protected:
void report_testsuite(testsuite_ptr const ts_) override {
*this << "## " << ts_->name() << SCTF_XLF << "|Tests|Successes|Failures|Errors|Time|" << SCTF_LF << "|-|-|-|-|-|" << SCTF_LF << "|" << ts_->statistics().tests() << "|" << ts_->statistics().successes() << "|" << ts_->statistics().failures() << "|" << ts_->statistics().errors() << "|" << ts_->time() << "ms|" << SCTF_XLF << "### Tests" << SCTF_XLF << "|Name|Context|Time|Status|" << SCTF_LF << "|-|-|-|-|" << SCTF_LF;
reporter::report_testsuite(ts_);
*this << SCTF_XLF;
}
void report_testcase(_::testcase const& tc_) override {
char const* status = "";
switch(tc_.state()) {
case _::testcase::result::ERROR: status = "ERROR"; break;
case _::testcase::result::FAILED: status = "FAILED"; break;
case _::testcase::result::PASSED: status = "PASSED"; break;
default: break;
}
*this << "|" << tc_.name() << "|" << tc_.context() << "|" << tc_.duration() << "ms|" << status << "|" << SCTF_LF;
}
void begin_report() override { *this << "# Test Report" << SCTF_XLF; }
void end_report() override { *this << "## Summary" << SCTF_XLF << "|Tests|Successes|Failures|Errors|Time|" << SCTF_LF << "|-|-|-|-|-|" << SCTF_LF << "|" << m_abs_tests << "|" << (m_abs_tests - m_abs_errs - m_abs_fails) << "|" << m_abs_fails << "|" << m_abs_errs << "|" << m_abs_time << "ms|" << SCTF_LF; }
};
static reporter_ptr create_markdown_reporter(std::ostream& stream_ = std::cout) { return std::make_shared<markdown_reporter>(stream_); }
static reporter_ptr create_markdown_reporter(char const* file_) { return std::make_shared<markdown_reporter>(file_); }
}// namespace sctf
#undef SCTF_XLF
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
constexpr comparison() : m_failure(nullopt) {}
comparison(char const* comp_str_, std::string const& val_, std::string const& expect_) : m_failure("Expected '" + val_ + "' " + comp_str_ + " '" + expect_ + "'") {}
explicit operator bool() { return !m_failure; }
std::string const& operator*() const { return *m_failure; }
private:
optional<std::string> const m_failure;
#else
constexpr comparison() : m_success(true) {}
comparison(char const* comp_str_, std::string const& val_, std::string const& expect_) : m_success(false) {
std::string msg;
msg.reserve(15 + std::strlen(comp_str_) + val_.length() + expect_.length());
msg = "Expected '";
msg.append(val_).append("' ").append(comp_str_).append(" '").append(expect_).append("'");
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
#if __cplusplus >= 201402L
#define SUCCESS comparison()
#else
constexpr comparison SUCCESS = comparison();
#endif
#ifdef SCTF_EPSILON
static double epsilon = SCTF_EPSILON;
#elif defined(SCTF_EXTERN_EPSILON)
extern double epsilon;
#endif
}}// namespace sctf::_
#define COMPARATOR(NAME, CMPSTR, PRED) \
namespace sctf { namespace _ { \
class NAME { \
static constexpr char const* m_cmp_str = "to be " CMPSTR; \
static constexpr char const* m_neg_cmp_str = "to be not " CMPSTR; \
bool m_neg = false; \
public: \
NAME& operator!() { \
m_neg = !m_neg; \
return *this; \
} \
template<typename V, typename E = V> comparison operator()(V const& actual_value, E const& expected_value) { return (PRED) != m_neg ? SUCCESS : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)); } \
}; \
} \
}
#define PROVIDE_COMPARATOR(COMP, NAME) \
namespace sctf { \
static _::COMP NAME() { return _::COMP(); } \
}
#define SCTF_SET_EPSILON(E) \
namespace sctf { namespace _ { \
double epsilon = E; \
} \
}
#endif// SCTF_COMPARATOR_COMPARATORS_HPP
#ifndef SCTF_COMPARATOR_LESS_HPP
#define SCTF_COMPARATOR_LESS_HPP
COMPARATOR(less_than, "less than", actual_value < expected_value)
PROVIDE_COMPARATOR(less_than, LESS_THAN)
PROVIDE_COMPARATOR(less_than, LESS)
PROVIDE_COMPARATOR(less_than, LT)
#endif// SCTF_COMPARATOR_LESS_HPP
#ifndef SCTF_COMPARATOR_INRANGE_HPP
#define SCTF_COMPARATOR_INRANGE_HPP
namespace sctf { namespace _ {
class in_range {
static constexpr char const* m_cmp_str = "to be in range of";
static constexpr char const* m_neg_cmp_str = "to be not in range of";
bool m_neg = false;
public:
in_range& operator!() {
m_neg = !m_neg;
return *this;
}
template<typename V, typename E = V, ENABLE_IF(IS_ITERABLE(E) AND NOT IS_TYPE(E, std::string))> comparison operator()(V const& actual_value, E const& expected_value) { return (std::find(expected_value.cbegin(), expected_value.cend(), actual_value) != expected_value.cend()) != m_neg ? SUCCESS : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)); }
template<typename V, typename E = V, ENABLE_IF(IS_TYPE(E, std::string))> comparison operator()(V const& actual_value, E const& expected_value) { return (expected_value.find(actual_value) != std::string::npos) != m_neg ? SUCCESS : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)); }
};
}}// namespace sctf::_
PROVIDE_COMPARATOR(in_range, IN_RANGE)
PROVIDE_COMPARATOR(in_range, IN)
#endif// SCTF_COMPARATOR_INRANGE_HPP
#ifndef SCTF_COMPARATOR_UNEQUALS_HPP
#define SCTF_COMPARATOR_UNEQUALS_HPP
namespace sctf { namespace _ {
class unequals {
static constexpr char const* m_cmp_str = "to be unequals";
static constexpr char const* m_neg_cmp_str = "to be not unequals";
bool m_neg = false;
public:
unequals& operator!() {
m_neg = !m_neg;
return *this;
}
template<typename V, typename E = V, ENABLE_IF(NOT IS_FLOAT(V) AND IS_UNEQUAL_COMPARABLE(V, E))> comparison operator()(V const& actual_value, E const& expected_value) { return (actual_value != expected_value) != m_neg ? SUCCESS : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)); }
template<typename V, typename E = V, ENABLE_IF(NOT IS_FLOAT(V) AND NOT IS_UNEQUAL_COMPARABLE(V, E) AND IS_EQUAL_COMPARABLE(V, E))> comparison operator()(V const& actual_value, E const& expected_value) { return (actual_value == expected_value) != m_neg ? comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)) : SUCCESS; }
template<typename V, typename E = V, ENABLE_IF(IS_FLOAT(V) AND IS_FLOAT(E))> comparison operator()(V const& actual_value, E const& expected_value) {
#if defined(SCTF_EXTERN_EPSILON) || defined(SCTF_EPSILON)
static V epsilon_ = static_cast<V>(epsilon);
#else
static V epsilon_ = std::numeric_limits<V>::epsilon();
#endif
return (std::abs(actual_value - expected_value) <= std::max(std::abs(actual_value), std::abs(expected_value)) * epsilon_) != m_neg ? comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)) : SUCCESS;
}
};
}}// namespace sctf::_
PROVIDE_COMPARATOR(unequals, UNEQUALS)
PROVIDE_COMPARATOR(unequals, NE)
#endif// SCTF_COMPARATOR_UNEQUALS_HPP
#ifndef SCTF_COMPARATOR_EQUALS_HPP
#define SCTF_COMPARATOR_EQUALS_HPP
namespace sctf { namespace _ {
class equals {
static constexpr char const* m_cmp_str = "to be equals";
static constexpr char const* m_neg_cmp_str = "to be not equals";
bool m_neg = false;
public:
equals& operator!() {
m_neg = !m_neg;
return *this;
}
template<typename V, typename E = V, ENABLE_IF(NOT IS_FLOAT(V) AND IS_EQUAL_COMPARABLE(V, E))> comparison operator()(V const& actual_value, E const& expected_value) { return (actual_value == expected_value) != m_neg ? SUCCESS : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)); }
template<typename V, typename E = V, ENABLE_IF(NOT IS_FLOAT(V) AND NOT IS_EQUAL_COMPARABLE(V, E) AND IS_UNEQUAL_COMPARABLE(V, E))> comparison operator()(V const& actual_value, E const& expected_value) { return (actual_value != expected_value) != m_neg ? comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value)) : SUCCESS; }
template<typename V, typename E = V, ENABLE_IF(IS_FLOAT(V) AND IS_FLOAT(E))> comparison operator()(V const& actual_value, E const& expected_value) {
#if defined(SCTF_EXTERN_EPSILON) || defined(SCTF_EPSILON)
static V epsilon_ = static_cast<V>(epsilon);
#else
static V epsilon_ = std::numeric_limits<V>::epsilon();
#endif
return (std::abs(actual_value - expected_value) <= std::max(std::abs(actual_value), std::abs(expected_value)) * epsilon_) != m_neg ? SUCCESS : comparison(m_neg ? m_neg_cmp_str : m_cmp_str, to_string(actual_value), to_string(expected_value));
}
};
}}// namespace sctf::_
PROVIDE_COMPARATOR(equals, EQUALS)
PROVIDE_COMPARATOR(equals, EQ)
#endif// SCTF_COMPARATOR_EQUALS_HPP
#ifndef SCTF_COMPARATOR_GREATER_HPP
#define SCTF_COMPARATOR_GREATER_HPP
COMPARATOR(greater_than, "greater than", actual_value > expected_value)
PROVIDE_COMPARATOR(greater_than, GREATER_THAN)
PROVIDE_COMPARATOR(greater_than, GREATER)
PROVIDE_COMPARATOR(greater_than, GT)
#endif// SCTF_COMPARATOR_GREATER_HPP
#ifndef SCTF_ASSERT_HPP
#define SCTF_ASSERT_HPP
#define ASSERT(VALUE, COMP, EXPECT) sctf::_::assert_statement(VALUE, EXPECT, COMP(), sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_NOT(VALUE, COMP, EXPECT) sctf::_::assert_statement(VALUE, EXPECT, !COMP(), sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_EQUALS(VALUE, EXPECT) sctf::_::assert_statement(VALUE, EXPECT, sctf::EQUALS(), sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_TRUE(VALUE) sctf::_::assert_statement(VALUE, true, sctf::EQUALS(), sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_FALSE(VALUE) sctf::_::assert_statement(VALUE, false, sctf::EQUALS(), sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_NULL(VALUE) sctf::_::assert_statement(static_cast<void const*>(VALUE), nullptr, sctf::EQUALS(), sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_NOT_NULL(VALUE) sctf::_::assert_statement(static_cast<void const*>(VALUE), nullptr, sctf::UNEQUALS(), sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_ZERO(VALUE) sctf::_::assert_statement(VALUE, static_cast<decltype(VALUE)>(0), sctf::EQUALS(), sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_THROWS(FUNC, TYPE) sctf::_::assert_throws<TYPE>([&] { FUNC; }, sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_NOTHROW(FUNC) sctf::_::assert_nothrow([&] { FUNC; }, sctf::_::code_location{__FILE__, __LINE__})
#define ASSERT_PERFORMANCE(FUNC, MILLIS) sctf::_::assert_performance([&] { FUNC; }, MILLIS, sctf::_::code_location{__FILE__, __LINE__})
namespace sctf { namespace _ {
template<typename C, typename V, typename E = V> static void assert_statement(V const& val_, E const& expect_, C&& cmp_, code_location const& loc_) {
comparison res = cmp_(val_, expect_);
if(!res) { throw assertion_failure(*res, loc_); }
}
template<typename T> static void assert_throws(test_function&& fn_, code_location const& loc_) {
try {
fn_();
} catch(T const&) { return; } catch(std::exception const& e) {
throw assertion_failure(std::string("Wrong exception thrown, caught '") + to_string(e) + "'", loc_);
} catch(...) { throw assertion_failure("Wrong exception thrown", loc_); }
throw assertion_failure(std::string("No exception thrown, expected '") + name_for_type<T>() + "'", loc_);
}
static void assert_nothrow(test_function&& fn_, code_location const& loc_) {
try {
fn_();
} catch(const std::exception& e) { throw assertion_failure(std::string("Expected no exception, caught '") + to_string(e) + "'", loc_); } catch(...) {
throw assertion_failure("Expected no exception", loc_);
}
}
static void assert_performance(test_function&& fn_, double max_ms_, code_location const& loc_) {
try {
duration dur;
fn_();
double dur_ms = dur.get();
if(dur_ms > max_ms_) { throw assertion_failure(std::string("runtime > ") + to_string(max_ms_) + "ms", loc_); }
} catch(std::exception const& e) { throw assertion_failure(e.what(), loc_); } catch(...) {
throw assertion_failure("Unknown exception thrown", loc_);
}
}
}}// namespace sctf::_
#endif// SCTF_ASSERT_HPP
#ifndef SCTF_SCTF_HPP
#define SCTF_SCTF_HPP
#define SCFT_VERSION "1.2-rc0"
#define SCTF_DEFAULT_MAIN(REPORTER, ...) \
int main(int, char**) { \
__VA_ARGS__; \
return static_cast<int>(sctf::REPORTER->report()); \
}
#endif// SCTF_SCTF_HPP
#endif
