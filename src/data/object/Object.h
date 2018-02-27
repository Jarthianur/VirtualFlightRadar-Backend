#pragma once

#include <cstdint>

namespace data
{
namespace object
{
class Object
{
public:
    Object();

    explicit Object(std::uint32_t vPriority);

    virtual ~Object() noexcept;

protected:
    /**
     * @fn setLastPriority
     * @brief Set the lastly updating priority.
     * @param vPriority
     */
    void setLastPriority(std::uint32_t vPriority);

    /**
     * @fn getLastPriority
     * @brief Get last written priority.
     * @return the last priority
     */
    std::uint32_t getLastPriority() const;

    /// @var mLastPriority
    /// Got last update with this priority.
    std::uint32_t mLastPriority;
};
}
}
