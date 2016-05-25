/*
 * VFRB.h
 *
 *  Created on: 23.04.2016
 *      Author: lula
 */

#ifndef VFRB_H_
#define VFRB_H_

#include "ConnectOutNMEA.h"
#include "ConnectInADSB.h"
#include "ConnectInOGN.h"
#include "AircraftContainer.h"

#define SYNC_TIME 1

class VFRB {
public:
    VFRB();
    virtual ~VFRB() throw();

    /**
     * runs the tool
     */
    static void run(long double, long double, int, float, int, int, int, const char*, const char*, const char*, const char*);

protected:
    /**
     * funtions for every single thread
     */
    static void handle_adsb_in(long double , long double, int, float, ConnectInADSB&, AircraftContainer&);
    static void handle_ogn_in(long double, long double, int, float, ConnectInOGN&, AircraftContainer&);
    static void handle_con_out(ConnectOutNMEA&);
    static void handle_con_adsb(ConnectInADSB&);
    static void handle_con_ogn(ConnectInOGN&);
};

#endif /* VFRB_H_ */
