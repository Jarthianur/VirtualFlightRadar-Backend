#include "Sensor.h"

namespace data
{
namespace object
{
Atmosphere::Atmosphere() : Object(0)
{}

Atmosphere::Atmosphere(std::uint32_t vPriority) : Object(vPriority)
{}

Atmosphere::~Atmosphere() noexcept
{}

const std::string& Atmosphere::getMdaStr() const
{
    return mMdaStr;
}

double Atmosphere::getPressure() const
{
    return mPressure;
}

Wind::Wind() : Object(0)
{}

Wind::Wind(std::uint32_t vPriority) : Object(vPriority)
{}

Wind::~Wind() noexcept
{}

const std::string& Wind::getMwvStr() const
{
    return mMwvStr;
}
}
}
