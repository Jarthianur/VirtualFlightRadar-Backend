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

#ifndef CONFIGREADER_H_
#define CONFIGREADER_H_

#include <fstream>
#include <unordered_map>
#include <string>
#include <regex>

class ConfigReader
{
public:
   ConfigReader(const char*);
   inline virtual ~ConfigReader() throw() {};

   void read();
   const std::string& getProperty(const std::string&, const std::string&) const;

private:
   const char* file;
   std::unordered_map<std::string, std::string> config;
   const std::regex conf_re;
};

#endif /* CONFIGREADER_H_ */
