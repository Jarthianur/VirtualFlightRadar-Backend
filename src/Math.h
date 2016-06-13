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

class Math
{
public:
    Math();
    virtual ~Math();

    /**
     * convert degree to radian
     */
    static long double radian(long double);

    /**
     * convert radian to degree
     */
    static long double degree(long double);

    /**
     * convert long double to int, round to nearest number
     */
    static int ldToI(long double);


    static long double dmsToDeg(long double dms);

    /**
     * constants
     */
    static const long double kts2kmh;
    static const long double kmh2kts;
    static const long double ms2fpm;
    static const long double fpm2ms;
    static const long double feet2m;
    static const long double PI;
};

#endif /* MATH_H_ */
