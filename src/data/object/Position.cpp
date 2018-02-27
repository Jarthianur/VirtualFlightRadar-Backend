#include "Position.h"

namespace data
{
namespace object
{
ExtGpsPosition::ExtGpsPosition() : Object(0)
{}

ExtGpsPosition::ExtGpsPosition(std::uint32_t vPriority) : Object(vPriority)
{}

ExtGpsPosition::~ExtGpsPosition() noexcept
{}

Object& ExtGpsPosition::
operator=(const boost::tuple<const Object&, std::uint32_t>& crOther)
{
    const ExtGpsPosition& crUpdate = static_cast<const ExtGpsPosition&>(crOther.get<0>());
    this->position                 = crUpdate.position;
    this->nrSats                   = crUpdate.nrSats;
    this->fixQa                    = crUpdate.fixQa;
    this->geoid                    = crUpdate.geoid;
    this->dilution                 = crUpdate.dilution;
    return *this;
}

}
}
