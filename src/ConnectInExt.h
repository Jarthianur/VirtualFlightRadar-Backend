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

#ifndef CONNECTINEXT_H_
#define CONNECTINEXT_H_

#include "ConnectIn.h"

class ConnectInExt: public ConnectIn
{
public:
   ConnectInExt(const char*, const int, std::string&, unsigned int = 0);
   virtual ~ConnectInExt() throw();

   int setupConnectIn();
   int connectIn();

private:
   /**
    * login stuff
    */
   std::string login_str;

   /**
    * options
    */
   int yes = 1;
   int no = 0;

};

#endif /* CONNECTINEXT_H_ */
