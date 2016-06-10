/*
 * VFRB.cpp
 *
 *  Created on: 23.04.2016
 *      Author: lula
 */

#include "VFRB.h"
#include "ParserADSB.h"
#include "ParserOGN.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

std::mutex mutex;

std::condition_variable con_adsb_cond;
std::condition_variable con_ogn_cond;

int VFRB::base_altitude = 0;
long double VFRB::base_latitude = 0.0L;
long double VFRB::base_longitude = 0.0L;
float VFRB::base_geoid = 0.0;
int VFRB::global_out_port = 0;
int VFRB::global_ogn_port = 0;
int VFRB::global_adsb_port = 0;
std::string VFRB::global_ogn_host = "";
std::string VFRB::global_adsb_host = "";
std::string VFRB::global_login_str = "";
std::string VFRB::global_nmea_feed_host = "nA";
int VFRB::global_nmea_feed_port = 0;
int VFRB::filter_maxHeight = 0;
//int VFRB::filter_maxDist = 0;

VFRB::VFRB()
{
}

VFRB::~VFRB()
{
}

void VFRB::run()
{
    ConnectOutNMEA out_con(global_out_port);
    ConnectIn adsb_con(global_adsb_host.c_str(), global_adsb_port);
    ConnectInExt ogn_con(global_ogn_host.c_str(), global_ogn_port, std::ref(global_login_str));
    AircraftContainer ac_cont;
    NMEAFeedW add_nmea_str;
    ConnectIn con_nmea_feed(global_nmea_feed_host.c_str(), global_nmea_feed_port);//temporary hack
    bool nmea_feed_enabled = false;
    if (global_nmea_feed_host.compare("nA") != 0) {
        nmea_feed_enabled = true;
    }

    try{
        std::thread adsb_in_thread(handle_adsb_in, std::ref(adsb_con), std::ref(ac_cont));
        std::thread ogn_in_thread(handle_ogn_in, std::ref(ogn_con), std::ref(ac_cont));

        std::thread con_out_thread(handle_con_out, std::ref(out_con));
        std::thread con_adsb_thread(handle_con_adsb, std::ref(adsb_con));
        std::thread con_ogn_thread(handle_con_ogn, std::ref(ogn_con));

        std::thread nmea_feed_thread(handle_nmea_feed, std::ref(add_nmea_str), std::ref(con_nmea_feed), nmea_feed_enabled);

        if (nmea_feed_enabled) {
            std::cout << "nmea feed given on " << global_nmea_feed_host << ":" << global_nmea_feed_port << std::endl;
        } else std::cout << "no nmea feed given" << std::endl;

        ParserADSB adsb_parser(base_latitude, base_longitude, base_altitude, base_geoid);
        ParserOGN ogn_parser(base_latitude, base_longitude, base_altitude, base_geoid);

        std::string str;
        unsigned int i;

        while (1) {
            ac_cont.invalidateAircrafts();

            for (i = 0; i < ac_cont.getContSize(); ++i) {
                //this is actually a no-go, but works all write-occurences lock the container
                Aircraft& ac = ac_cont.getAircraft(i);
                if (ac.aircraft_type == -1) {
                    adsb_parser.process(ac, str);
                } else {
                    ogn_parser.process(ac, str);
                }
                out_con.sendMsgOut(str);
            }
            adsb_parser.gprmc(str);
            out_con.sendMsgOut(str);
            adsb_parser.gpgga(str);
            out_con.sendMsgOut(str);
            if (nmea_feed_enabled) {
                out_con.sendMsgOut(add_nmea_str.getNMEA());
            }
            std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
        }

        adsb_in_thread.join();
        ogn_in_thread.join();
        con_out_thread.join();
        con_adsb_thread.join();
        con_ogn_thread.join();
        nmea_feed_thread.join();

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        std::terminate();
    }
    return;
}

void VFRB::handle_nmea_feed(NMEAFeedW& nmea_str, ConnectIn& nmea_con, bool enabled)
{
    if (enabled) {
        if (nmea_con.setupConnectIn() == -1) return;
        if (nmea_con.connectIn() == -1) return;
        while(1) {
            if (nmea_con.readLineIn() <= 0) {
                while (nmea_con.connectIn() == -1) {
                    nmea_con.close();
                    nmea_con.setupConnectIn();
                    std::cout << "waiting for nmea feed"<<std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
                }
            } else {
                nmea_str.writeNMEA(std::ref(nmea_con.getResponse()));
            }
        }
    } else
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

void VFRB::handle_con_adsb(ConnectIn& adsb_con)
{
    if (adsb_con.setupConnectIn() == -1) return;

    while (adsb_con.connectIn() == -1) {
        std::cout << "waiting for adsb-server"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
    }
    con_adsb_cond.notify_one();

    while (1) {
        std::unique_lock<std::mutex> lock(mutex);
        con_adsb_cond.wait(lock);
        lock.unlock();
        while (adsb_con.connectIn() == -1) {
            adsb_con.close();
            adsb_con.setupConnectIn();
            std::cout << "waiting for adsb-server"<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
        }
        con_adsb_cond.notify_one();
    }
    return;
}

void VFRB::handle_con_ogn(ConnectInExt& ogn_con)
{
    if (ogn_con.setupConnectIn() == -1) return;

    while (ogn_con.connectIn() == -1) {
        std::cout << "waiting for ogn-server"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
    }
    con_ogn_cond.notify_one();

    while (1) {
        std::unique_lock<std::mutex> lock(mutex);
        con_ogn_cond.wait(lock);
        lock.unlock();
        while (ogn_con.connectIn() == -1) {
            ogn_con.close();
            ogn_con.setupConnectIn();
            std::cout << "waiting for ogn-server"<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
        }
        con_ogn_cond.notify_one();
    }
    return;
}

void VFRB::handle_adsb_in(ConnectIn& adsb_con, AircraftContainer& ac_cont)
{
    ParserADSB parser(base_latitude, base_longitude, base_altitude, base_geoid);
    std::unique_lock<std::mutex> lock(mutex);
    con_adsb_cond.wait(lock);
    lock.unlock();

    std::cout << "Scan for incoming adsb-msgs..." << std::endl;

    while (1) {
        if (adsb_con.readLineIn() <= 0) {
            con_adsb_cond.notify_one();
            std::unique_lock<std::mutex> lock(mutex);
            con_adsb_cond.wait(lock);
            std::cout << "Scan for incoming adsb-msgs..." << std::endl;
            lock.unlock();
        }
        //need msg3 only
        if (adsb_con.getResponse().at(4) == '3') {
            parser.unpack(adsb_con.getResponse(), ac_cont);
        }
    }
    return;
}

void VFRB::handle_ogn_in(ConnectInExt& ogn_con, AircraftContainer& ac_cont)
{
    ParserOGN parser(base_latitude, base_longitude, base_altitude, base_geoid);
    std::unique_lock<std::mutex> lock(mutex);
    con_ogn_cond.wait(lock);
    lock.unlock();

    std::cout << "Scan for incoming ogn-msgs..." << std::endl;

    while (1) {
        if (ogn_con.readLineIn() <= 0) {
            con_ogn_cond.notify_one();
            std::unique_lock<std::mutex> lock(mutex);
            con_ogn_cond.wait(lock);
            std::cout << "Scan for incoming ogn-msgs..." << std::endl;
            lock.unlock();
        }
        parser.unpack(ogn_con.getResponse(), ac_cont);
    }
    return;
}
