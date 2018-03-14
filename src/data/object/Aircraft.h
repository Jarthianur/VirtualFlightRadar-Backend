/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
 A detailed list of copyright holders can be found in the file "AUTHORS".

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

#pragma once

#include <cstdint>
#include <string>

#include "Object.h"
#include "Position.h"

/// @def A_VALUE_NA
/// Indicate a double value is not available.
#define A_VALUE_NA -1024.0

/// @namespace data
namespace data
{
/// @namespace object
namespace object
{
/**
 * @struct Movement
 * @brief Hold information about an Aircrafts movement.
 */
struct Movement
{
    /// @var gndSpeed
    /// Speed over ground; m/s
    double gndSpeed = A_VALUE_NA;

    /// @var heading
    /// Heading; deg [0-359]
    double heading = A_VALUE_NA;

    /// @var climbRate
    /// Climb rate; m/s
    double climbRate = A_VALUE_NA;
};

/**
 * @class Aircraft
 * @brief Respresents an aircraft.
 * @extends Object
 */
class Aircraft : public Object
{
public:
    /**
     * @enum TargetType
     * @brief Device type from which the information is received.
     * @note FLARM is preferred over TRANSPONDER,
     *       in case an aircraft has both available.
     */
    enum class TargetType : std::uint32_t
    {
        FLARM,
        TRANSPONDER
    };

    /**
     * @enum AircraftType
     * @brief Aircraft types with their protocol codes.
     */
    enum class AircraftType : std::uint32_t
    {
        UNKNOWN               = 0,
        GLIDER                = 1,
        TOW_PLANE             = 2,
        HELICOPTER_ROTORCRAFT = 3,
        PARACHUTE             = 4,
        DROP_PLANE            = 5,
        HANG_GLIDER           = 6,
        PARA_GLIDER           = 7,
        POWERED_AIRCRAFT      = 8,  ///< Default for TRANSPONDER targets
        JET_AIRCRAFT          = 9,
        UFO                   = 10,
        BALLOON               = 11,
        AIRSHIP               = 12,
        UAV                   = 13,
        STATIC_OBJECT         = 15
    };

    /**
     * @enum IdType
     * @brief Id (address) types with their protocol codes.
     */
    enum class IdType : std::int32_t
    {
        UNRECOGNIZED = -1,
        RANDOM       = 0,
        ICAO         = 1,
        FLARM        = 2,
        OGN          = 3
    };

    /**
     * @fn Aircraft
     * @brief Constructor
     * @param vPriority The initial priority (default: 0)
     */
    explicit Aircraft(std::uint32_t vPriority = 0);

    /**
     * @fn Aircraft
     * @brief Constructor
     * @param rId  The id
     * @param rPos The position
     */
    Aircraft(std::string& rId, GpsPosition& rPos);

    /**
     * @fn Aircraft
     * @brief Constructor
     * @param rId           The Id
     * @param vIdType       The Id type
     * @param vAircraftType The aircraft type
     * @param rPos          The prosition
     * @param rMove         The aircrafts movement
     */
    Aircraft(std::string& rId, IdType vIdType, AircraftType vAircraftType,
             GpsPosition& rPos, Movement& rMove);

    /**
     * @fn ~Aircraft
     * @brief Destructor
     */
    virtual ~Aircraft() noexcept;

    /**
     * @fn assign
     * @brief Assign an other Aircrafts values to this.
     * @param crOther The other Aircraft
     */
    void assign(const Object& crOther) override;

    /**
     * @fn getId
     * @brief Get the Id.
     * @return mId
     */
    const std::string& getId() const;

    /**
     * @fn getIdType
     * @brief Get the Id type.
     * @see IdType
     * @return mIdType
     */
    IdType getIdType() const;

    /**
     * @fn getTargetType
     * @brief Get the target type.
     * @see TargetType
     * @return mTargetType
     */
    TargetType getTargetType() const;

    /**
     * @fn getAircraftType
     * @brief Get the aircraft type.
     * @see AircraftType
     * @return mAircraftType
     */
    AircraftType getAircraftType() const;

    /**
     * @fn hasFullInfo
     * @brief Are all position and movement informations given?
     * @return true, if full info is available, else false
     */
    bool hasFullInfo() const;

    /**
     * @fn getUpdateAge
     * @brief Get the current update age.
     * @return mUpdateAge by reference
     */
    std::uint32_t& getUpdateAge();

    /**
     * @fn getLatitude
     * @brief Get the latitude.
     * @return mPosition.latitude
     */
    double getLatitude() const;

    /**
     * @fn getLongitude
     * @brief Get the longitude.
     * @return mPosition.longitude
     */
    double getLongitude() const;

    /**
     * @fn getAltitude
     * @brief Get the altitude.
     * @return mPosition.altitude.
     */
    std::int32_t getAltitude() const;

    /**
     * @fn getGndSpeed
     * @brief Get the speed over ground.
     * @return mMovement.gndSpeed
     */
    double getGndSpeed() const;

    /**
     * @fn getHeading
     * @brief Get the heading.
     * @return mMovement.heading
     */
    double getHeading() const;

    /**
     * @fn getClimbRate
     * @brief Get the climb rate.
     * @return mMovement.climbRate
     */
    double getClimbRate() const;

    /**
     * @fn setId
     * @brief Set the Id.
     * @param crId The Id
     */
    void setId(const std::string& crId);

    /**
     * @fn setPosition
     * @brief Set the new position.
     * @param crPos The position
     */
    void setPosition(const GpsPosition& crPos);

    /**
     * @fn setAircraftType
     * @brief Set the Aircraft type.
     * @see AircraftType
     * @param vType The aircraft type
     */
    void setAircraftType(AircraftType vType);

    /**
     * @fn setIdType
     * @brief Set the Id type.
     * @see IdType
     * @param vType The Id type
     */
    void setIdType(IdType vType);

    /**
     * @fn setClimbRate
     * @brief Set the climb rate.
     * @param vRate The new climb rate (default: A_VALUE_NA)
     */
    void setClimbRate(double vRate = A_VALUE_NA);

    /**
     * @fn setHeading
     * @brief Set the heading.
     * @param vHeading The new heading (default: A_VALUE_NA)
     */
    void setHeading(double vHeading = A_VALUE_NA);

    /**
     * @fn setGndSpeed
     * @brief Set the ground speed.
     * @param vGndSpd The new ground speed (default: A_VALUE_NA)
     */
    void setGndSpeed(double vGndSpd = A_VALUE_NA);

    /**
     * @fn setTargetType
     * @brief Set the target type.
     * @see TargetType
     * @param vType The new target type
     */
    void setTargetType(TargetType vType);

    /**
     * @fn setFullInfo
     * @brief Set full information to be available.
     * @param vInfo Is available, or not (default: true)
     */
    void setFullInfo(bool vInfo = true);

private:
    /**
     * @fn canUpdate
     * @brief Check whether this Aircraft can update the other one.
     * @param crOther   The other Aircraft
     * @param vAttempts The update attempt count
     * @return true if yes, else false
     */
    bool canUpdate(const Object& crOther, std::uint32_t vAttempts) const override;

    /// @var mId
    /// Aircraft identifier
    std::string mId;

    /// @var mIdType
    /// @see IdType
    IdType mIdType;

    /// @var mAircraftType
    /// @see AircraftType
    AircraftType mAircraftType;

    /// @var mTargetType
    /// @see TargetType
    TargetType mTargetType;

    /// @var mPosition
    /// Currently known position.
    GpsPosition mPosition{0.0, 0.0, 0};

    /// @var mMovement
    /// Currently known movement.
    Movement mMovement;

    /// @var mFullInfo
    /// Is full set of information available?
    bool mFullInfo = false;

    /// @var mUpdateAge
    /// Times processed without update.
    std::uint32_t mUpdateAge = 0;
};

}  // namespace object
}  // namespace data
