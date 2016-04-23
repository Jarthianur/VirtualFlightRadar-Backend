/*
 * Aircraft.h
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <string>

class Aircraft
{
public:
    Aircraft();
    virtual ~Aircraft();

    std::string id;
    long double latitude;
    long double longitude;
    int altitude;
    float track;
    int ground_speed;
    int address_type;
    //illegal ac_type == -1 used for check extended data
    int aircraft_type;
    int climb_rate;
    //0 = valid; +x(cycles) = invalid
    int valid;

private:

};

#endif /* AIRCRAFT_H_ */
