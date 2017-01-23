/*
Copyright_License {

  Copyright (C) 2017 VirtualFlightRadar-Backend
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

#ifndef MATH_H_
#define MATH_H_

#include <cmath>
#include <cstdint>

namespace Math
{
/**
 * convert degree to radian
 */
double radian(double);

/**
 * convert radian to degree
 */
double degree(double);

/**
 * convert to int, round to nearest number
 */
int32_t dToI(double);

double dmsToDeg(double dms);

/**
 * calculate height difference from QNE to Pressure in meters
 * with ICAO height formula
 */
double calcIcaoHeight(double, double = 15.0);

/**
 * constants
 */
const double kts2kmh = 1.852;
const double kmh2kts = 0.539957;
const double kts2ms = 0.514444;
const double ms2kts = 1.94384;
const double kmh2ms = 0.277778;
const double ms2kmh = 3.6;
const double ms2fpm = 196.85;
const double fpm2ms = 0.00508;
const double feet2m = 0.3048;
const double m2feet = 3.28084;
const double PI = std::acos(-1.0);

}

#endif /* MATH_H_ */
