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

#include "Math.h"

namespace Math
{

double radian(double deg)
{
    return ((deg * PI) / 180.0);
}

double degree(double rad)
{
    return (rad * 180.0) / PI;
}

int dToI(double d)
{
    return (d >= 0.0) ? (int)(d + 0.5) : (int)(d - 0.5);
}

double dmsToDeg(double dms)
{
    double absDms = std::abs(dms);
    double d = std::floor(absDms);
    double m = (absDms - d) * 100.0 / 60.0;
    return d + m;
}
}

double Math::calcIcaoHeight(double press, double temp)
{
    return (273.15 + temp) * (1 - std::pow((press / 1013.25), 0.190295)) / 0.0065;
}
