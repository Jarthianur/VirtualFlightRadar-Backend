/*
Copyright_License {

  Copyright (C) 2016 Virtual Flight Radar - Backend
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
int dToI(double);
int fToI(float);


double dmsToDeg(double dms);

/**
 * constants
 */
const float kts2kmh = 1.852;
const float ms2fpm = 196.85;
const float feet2m = 0.3048;
const float ms2kmh = 3.6;
const double kmh2ms = 0.277778;
const double fpm2ms = 0.00508;
const double kmh2kts = 0.539957;
const double PI = std::acos(-1.0);

}

#endif /* MATH_H_ */
