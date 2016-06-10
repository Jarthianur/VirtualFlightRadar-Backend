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
#include "NMEAFeedW.h"

#define SYNC_TIME 1

class VFRB {
public:
    VFRB();
    virtual ~VFRB() throw();

    /**
     * runs the tool
     */
    static void run();
    /**
     * configuration
     */
    static int base_altitude;
    static long double base_latitude;
    static long double base_longitude;
    static float base_geoid;
    static int global_out_port;
    static int global_ogn_port;
    static int global_adsb_port;
    static std::string global_ogn_host;
    static std::string global_adsb_host;
    static std::string global_user;
    static std::string global_pass;
    static std::string global_nmea_feed_host;
    static int global_nmea_feed_port;
    static int filter_maxHeight;
    //static int filter_maxDist;

protected:
    /**
     * funtions for every single thread
     */
    static void handle_adsb_in(ConnectInADSB&, AircraftContainer&);
    static void handle_ogn_in(ConnectInOGN&, AircraftContainer&);
    static void handle_con_out(ConnectOutNMEA&);
    static void handle_con_adsb(ConnectInADSB&);
    static void handle_con_ogn(ConnectInOGN&);
    static void handle_nmea_feed(NMEAFeedW&, ConnectInADSB&, bool);
};

#endif /* VFRB_H_ */
