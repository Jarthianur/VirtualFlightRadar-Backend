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

#ifndef WINDFEED_H_
#define WINDFEED_H_

#include <boost/thread/mutex.hpp>
#include <string>

#include "../config/Configuration.h"


#define ICAO_STD_A 1013.25
#define ICAO_STD_T 15.0

class ClimateData
{
public:
    ClimateData();
    virtual ~ClimateData();

    std::string extractWV();
    void insertWV(const std::string& wv);

    bool isValid();
    double getPress();
    double getTemp();
    void setPress(double p = Configuration::base_pressure);
    void setTemp(double t = Configuration::base_temp);

private:
    boost::mutex mutex;
    std::string wv_;
    // hpa
    double pressure;
    // celsius
    double temperature;
    bool wv_valid = false;
};

#endif /* WINDFEED_H_ */
