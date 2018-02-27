#include "Object.h"
namespace data
{
namespace object
{
Object::Object() : mLastPriority(0)
{}

Object::Object(std::uint32_t vPriority) : mLastPriority(vPriority)
{}

Object::~Object() noexcept
{}

void Object::setLastPriority(std::uint32_t vPriority)
{
    mLastPriority = vPriority;
}

std::uint32_t Object::getLastPriority() const
{
    return mLastPriority;
}
}
}
