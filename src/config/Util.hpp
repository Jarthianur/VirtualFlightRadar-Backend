#pragma once

#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

namespace config
{
namespace util
{
using Number    = boost::variant<std::int32_t, std::uint64_t, double>;
using OptNumber = boost::tuple<bool, Number>;

template<typename T>
inline OptNumber stringToNumber(const std::string& crStr)
{
    std::stringstream ss(crStr);
    T result;
    bool suc = ss >> result;
    return boost::make_tuple(suc, Number(result));
}

/**
 * @fn trimString
 * @brief Trim a string on both sides.
 * @param rStr The string
 * @return the trimmed string
 */
inline std::string& trimString(std::string& rStr)
{
    std::size_t f = rStr.find_first_not_of(' ');
    if(f != std::string::npos)
    {
        rStr = rStr.substr(f);
    }
    std::size_t l = rStr.find_last_not_of(' ');
    if(l != std::string::npos)
    {
        rStr = rStr.substr(0, l + 1);
    }
    return rStr;
}
}
}
