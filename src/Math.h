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
long double radian(long double);

/**
 * convert radian to degree
 */
long double degree(long double);

/**
 * convert long double to int, round to nearest number
 */
int ldToI(long double);


long double dmsToDeg(long double dms);

/**
 * constants
 */
const long double kts2kmh = 1.852L;
const long double kmh2kts = 0.539957L;
const long double ms2fpm = 196.85L;
const long double fpm2ms = 0.00508L;
const long double feet2m = 0.3048L;
const long double PI = std::acos(-1.0L);

}

#endif /* MATH_H_ */
