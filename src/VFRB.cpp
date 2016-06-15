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

#include "VFRB.h"
#include "AircraftProcessor.h"
#include "ParserADSB.h"
#include "ParserOGN.h"
#include "Configuration.h"
#include <chrono>
#include <thread>
#include <iostream>

bool VFRB::global_nmea_feed_enabled = false;
bool VFRB::global_ogn_enabled = false;
bool VFRB::global_adsb_enabled = false;

VFRB::VFRB()
{
}

VFRB::~VFRB()
{
}

void VFRB::run()
{
    ConnectOutNMEA out_con(Configuration::global_out_port);
    AircraftContainer ac_cont;
    NMEAFeedW nmea_feed_w;
    AircraftProcessor ac_proc(Configuration::base_latitude, Configuration::base_longitude, Configuration::base_altitude, Configuration::base_geoid);

    if (Configuration::global_ogn_host.compare("nA") != 0 || Configuration::global_ogn_host.length() == 0) {
        global_ogn_enabled = true;
    } else std::cout << "ogn not enabled" << std::endl;
    if (Configuration::global_adsb_host.compare("nA") != 0 || Configuration::global_adsb_host.length() == 0) {
        global_adsb_enabled = true;
    } else std::cout << "adsb not enabled" << std::endl;
    if (Configuration::global_nmea_feed_host.compare("nA") != 0 || Configuration::global_nmea_feed_host.length() == 0) {
        global_nmea_feed_enabled = true;
    } else std::cout << "nmea feed not enabled" << std::endl;

    try{
        std::thread adsb_in_thread(handle_adsb_in, std::ref(ac_cont));
        std::thread ogn_in_thread(handle_ogn_in, std::ref(ac_cont));
        std::thread con_out_thread(handle_con_out, std::ref(out_con));
        std::thread nmea_feed_thread(handle_nmea_feed, std::ref(nmea_feed_w));

        std::string str;
        unsigned int i;

        while (1) {
            ac_cont.invalidateAircrafts();
            for (i = 0; i < ac_cont.getContSize(); ++i) {
                ac_cont.processAircraft(i, std::ref(str));
                if (str.length() > 0) {
                    out_con.sendMsgOut(std::ref(str));
                }
            }
            ac_proc.gprmc(std::ref(str));
            out_con.sendMsgOut(std::ref(str));
            ac_proc.gpgga(std::ref(str));
            out_con.sendMsgOut(std::ref(str));
            if (global_nmea_feed_enabled) {
                nmea_feed_w.getNMEA(std::ref(str));
                out_con.sendMsgOut(std::ref(str));
            }
            std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
        }

        adsb_in_thread.join();
        ogn_in_thread.join();
        con_out_thread.join();
        nmea_feed_thread.join();

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        std::terminate();
    }
    return;
}

void VFRB::handle_nmea_feed(NMEAFeedW& nmea_str)
{
    if (global_nmea_feed_enabled) {
        ConnectIn nmea_con(Configuration::global_nmea_feed_host.c_str(), Configuration::global_nmea_feed_port);
        if (nmea_con.setupConnectIn() == -1) return;
        while (nmea_con.connectIn() == -1) {
            nmea_con.close();
            if (nmea_con.setupConnectIn() == -1) return;
            std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
        }
        while(1) {
            if (nmea_con.readLineIn() <= 0) {
                do {
                    nmea_con.close();
                    if (nmea_con.setupConnectIn() == -1) return;
                    std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
                } while (nmea_con.connectIn() == -1);
            } else {
                nmea_str.writeNMEA(std::ref(nmea_con.getResponse()));
            }
        }
    }
    return;
}

void VFRB::handle_con_out(ConnectOutNMEA& out_con)
{
    if (out_con.listenOut() == -1) return;
    while (1) {
        out_con.connectClient();
    }
    return;
}

void VFRB::handle_adsb_in(AircraftContainer& ac_cont)
{
    if (global_adsb_enabled) {
        ParserADSB parser;
        ConnectIn adsb_con(Configuration::global_adsb_host.c_str(), Configuration::global_adsb_port);

        if (adsb_con.setupConnectIn() == -1) return;
        while (adsb_con.connectIn() == -1) {
            adsb_con.close();
            if (adsb_con.setupConnectIn() == -1) return;
            std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
        }

        std::cout << "Scan for incoming adsb-msgs..." << std::endl;

        while (1) {
            if (adsb_con.readLineIn() <= 0) {
                do {
                    adsb_con.close();
                    if (adsb_con.setupConnectIn() == -1) return;
                    std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
                } while (adsb_con.connectIn() == -1);
            } else
                //need msg3 only
                if (adsb_con.getResponse().at(4) == '3') {
                    parser.unpack(std::ref(adsb_con.getResponse()), std::ref(ac_cont));
                }
        }
    }
    return;
}

void VFRB::handle_ogn_in(AircraftContainer& ac_cont)
{
    if (global_ogn_enabled) {
        ParserOGN parser;
        ConnectInExt ogn_con(Configuration::global_ogn_host.c_str(), Configuration::global_ogn_port, std::ref(Configuration::global_login_str));

        if (ogn_con.setupConnectIn() == -1) return;
        while (ogn_con.connectIn() == -1) {
            ogn_con.close();
            if (ogn_con.setupConnectIn() == -1) return;
            std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
        }

        std::cout << "Scan for incoming ogn-msgs..." << std::endl;

        while (1) {
            if (ogn_con.readLineIn() <= 0) {
                do {
                    ogn_con.close();
                    if (ogn_con.setupConnectIn() == -1) return;
                    std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME));
                } while (ogn_con.connectIn() == -1);
            } else /*if (ogn_con.getResponse().at(0) != '#')*/ {
                parser.unpack(std::ref(ogn_con.getResponse()), std::ref(ac_cont));
            }
        }
    }
    return;
}
