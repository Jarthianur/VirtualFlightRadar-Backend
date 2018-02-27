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
     */
    Aircraft();

    /**
     * @fn Aircraft
     * @brief Constructor
     * @param vPriority The initial priority
     */
    Aircraft(std::uint32_t vPriority);

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
     * @fn operator==
     * @brief Compare two aircrafts by their Id.
     * @param crOther The Aircraft to compare
     * @return true, if Id's are equal, else false
     */
    bool operator==(const Aircraft& crOther) const;

    /**
     * @fn update
     * @brief Update this Aircraft.
     * @param crUpdate  Other Aircraft with new data
     * @param vPriority Priority of the update
     */
    Object& operator=(const Object& crOther) override;

    /**
     * @fn getId
     * @brief Get the Id.
     * @return the Id
     */
    const std::string& getId() const;

    /**
     * @fn getIdType
     * @brief Get the Id type.
     * @see IdType
     * @return the Id type
     */
    IdType getIdType() const;

    /**
     * @fn getTargetT
     * @brief Get the target type.
     * @see TargetType
     * @return the target type
     */
    TargetType getTargetType() const;

    /**
     * @fn getAircraftT
     * @brief Get the aircraft type.
     * @see AircraftType
     * @return the aircraft type
     */
    AircraftType getAircraftType() const;

    /**
     * @fn hasFullInfo
     * @brief Are position and movement informations given?
     * @return true, if full info is available, else false
     */
    bool hasFullInfo() const;

    /**
     * @fn getUpdateAge
     * @brief Get the current update age.
     * @return the update age by reference
     */
    std::uint64_t& getUpdateAge();

    /**
     * @fn getLatitude
     * @brief Get the last known latitude.
     * @return the latitude
     */
    double getLatitude() const;

    /**
     * @fn getLongitude
     * @brief Get the last known longitude.
     * @return the longitude
     */
    double getLongitude() const;

    /**
     * @fn getAltitude
     * @brief Get the last known altitude.
     * @return the altitude.
     */
    std::int32_t getAltitude() const;

    /**
     * @fn getGndSpeed
     * @brief Get the last known speed over ground.
     * @return the ground speed
     */
    double getGndSpeed() const;

    /**
     * @fn getHeading
     * @brief Get the last known heading.
     * @return the heading
     */
    double getHeading() const;

    /**
     * @fn getClimbRate
     * @brief Get the last known climb rate.
     * @return the climb rate
     */
    double getClimbRate() const;

    /**
     * @fn getUpdateAttempts
     * @brief Get the update attempt count.
     * @return the update attempt count by reference
     */
    std::uint64_t& getUpdateAttempts();

    /**
     * @fn setId
     * @brief Set the Id.
     * @param crId The new Id
     */
    void setId(const std::string& crId);

    /**
     * @fn setPosition
     * @brief Set the GPS position.
     * @param crPos The new position
     */
    void setPosition(const GpsPosition& crPos);

    /**
     * @fn setAircraftType
     * @brief Set the Aircraft type.
     * @see AircraftType
     * @param vType The new Aircraft type
     */
    void setAircraftType(AircraftType vType);

    /**
     * @fn setIdType
     * @brief Set the Id type.
     * @see IdType
     * @param vType The new Id type
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
    /// @var mId
    /// Aircraft identifier
    std::string mId;

    /// @var mIdType
    /// @see IdType
    IdType mIdType;

    /// @var mAircraftType
    /// @see AircraftType
    AircraftType mAircraftType;

    /// Target type; Got the last update from which device.
    TargetType mTargetType;

    /// @var mPosition
    /// Currently known position.
    GpsPosition mPosition;

    /// @var mMovement
    /// Currently known movement.
    Movement mMovement;

    /// @var mFullInfo
    /// Is full set of information available?
    bool mFullInfo = false;

    /// @var mUpdateAge
    /// Times processed without update.
    std::uint64_t mUpdateAge = 0;

    /// @var mUpdateAttempts
    /// Number of unsuccessfull update attempts.
    std::uint64_t mUpdateAttempts = 0;
};

}  // namespace object
}  // namespace data
