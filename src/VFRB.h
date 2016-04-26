/*
 * VFRB.h
 *
 *  Created on: 23.04.2016
 *      Author: lula
 */

#ifndef VFRB_H_
#define VFRB_H_

#include <vector>
#include <mutex>
#include "ConnectOutNMEA.h"
#include "Aircraft.h"

#define INVALIDATE 4

class VFRB {
public:
    VFRB();
    virtual ~VFRB();

    static void run(long double,  long double,  int,  int,  int,  int, const char*, const char*, const char*, const char*);
    static int vecfind(std::string&);
    static void pushAircraft(Aircraft*);
    static Aircraft* getAircraft(int i);

protected:
    static std::vector<Aircraft*> vec;
    static std::mutex vec_lock;
    static void invalidateAircrafts();

    static void do_adsb(long double , long double, int, const char*, int);
    static void do_ogn(long double, long double, int, const char*, int, const char*, const char*);
    static void handle_connections(ConnectOutNMEA*);
};

#endif /* VFRB_H_ */
