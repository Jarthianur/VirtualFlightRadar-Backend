/*
Copyright_License {

  Copyright (C) 2017 Virtual Flight Radar - Backend
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

#ifndef WINDFEED_H_
#define WINDFEED_H_

#include <string>
#include <mutex>

class WindFeed
{
public:
    WindFeed();
    virtual ~WindFeed();

    void getNMEA(std::string&);
    void writeNMEA(const std::string&);

private:
    std::mutex mutex;
    std::string nmea_str;
};

#endif /* WINDFEED_H_ */
