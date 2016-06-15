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

#include "Math.h"
#include <cmath>

namespace Math
{

long double radian(long double deg)
{
    return ((deg * PI) / 180.0L);
}

long double degree(long double rad)
{
    return (rad * 180.0L) / PI;
}

int ldToI(long double ld)
{
    return (ld >= 0) ? (int)(ld + 0.5L) : (int)(ld - 0.5L);
}

long double dmsToDeg(long double dms)
{
    long double absDms = std::abs(dms);
    long double d = std::floor(absDms);
    long double m = (absDms - d) * 100.0L / 60.0L;
    return d + m;
}
}
