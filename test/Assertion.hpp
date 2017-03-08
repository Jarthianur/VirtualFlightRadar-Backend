#include <cstdint>
#include <functional>
#include <iostream>

namespace assertion
{

#define ASSERT_EQUAL(V, E, I...)  \
        ( \
         (C_ASSERT((V), (E), (__FUNCTION__), I)), \
         (void)0 \
        )
#define ASSERT(V, E, I...) (ASSERT_EQUAL((V), (E), I))

// ANSI colors
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
// ANSI reset colors
#define ANSI_RESET   "\x1b[0m"
// print
#define OUT std::cout
#define ERR std::cerr
#define LF std::endl

uint32_t NUM_OF_TESTS = 0;
uint32_t NUM_OF_FAILS = 0;

template<typename T>
inline void C_FAIL(const char* caller, const T& expected)
{
    NUM_OF_FAILS++;
    OUT<< ANSI_RED << "Test: [" << caller << "] failed! Expected: ("<< expected << ")" << ANSI_RESET
    << LF;
}

inline void C_PASS(const char* caller)
{
    OUT<< ANSI_GREEN << "Test: [" << caller << "] passed!" << ANSI_RESET
    << LF;
}

template<typename T, typename ... I>
inline void C_TEST(const char* caller, const T& t, const I ... input)
{
    NUM_OF_TESTS++;
    OUT<< ANSI_CYAN << "Run [" << caller << "] with ( " << ANSI_RESET;
    auto args = { input... };
    for (auto arg = args.begin(); arg != args.end(); arg++)
    {
        OUT<< *arg;
        if (arg != args.end()-1)
        {
            OUT << " , ";
        }
    }
    OUT<< ANSI_CYAN << " ); result = " << t << ANSI_RESET << LF;
}

template<typename T, typename ... I>
inline void C_ASSERT(const T& result, const T& expected, const char* caller,
                           const I ... input)
{
    C_TEST(caller, result, input...);
    if (result == expected)
    {
        C_PASS(caller);
    }
    else
    {
        C_FAIL(caller, expected);
    }
}

inline void PRINT_RESULT()
{
    OUT<<ANSI_CYAN << "Result:: passed: " << NUM_OF_TESTS - NUM_OF_FAILS << "/" << NUM_OF_TESTS << " ; failed: " << NUM_OF_FAILS
    << "/" << NUM_OF_TESTS << ANSI_RESET << LF;
}

}
