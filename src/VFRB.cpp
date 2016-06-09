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
std::string VFRB::global_user = "";
std::string VFRB::global_pass = "";
int VFRB::filter_maxHeight = 0;

VFRB::VFRB()
{
}

VFRB::~VFRB()
{
}

void VFRB::run()
{
    ConnectOutNMEA out_con(global_out_port);
    ConnectInADSB adsb_con(global_adsb_host.c_str(), global_adsb_port);
    ConnectInOGN ogn_con(global_ogn_host.c_str(), global_ogn_port, global_user.c_str(), global_pass.c_str());
    AircraftContainer ac_cont;

    try{
        std::thread adsb_in_thread(handle_adsb_in, std::ref(adsb_con), std::ref(ac_cont));
        std::thread ogn_in_thread(handle_ogn_in, std::ref(ogn_con), std::ref(ac_cont));

        std::thread con_out_thread(handle_con_out, std::ref(out_con));
        std::thread con_adsb_thread(handle_con_adsb, std::ref(adsb_con));
        std::thread con_ogn_thread(handle_con_ogn, std::ref(ogn_con));

        ParserADSB adsb_parser(base_latitude, base_longitude, base_altitude, base_geoid);
        ParserOGN ogn_parser(base_latitude, base_longitude, base_altitude, base_geoid);

        std::string str;
        unsigned int i;

        while (1) {
            ac_cont.invalidateAircrafts();

            for (i = 0; i < ac_cont.getContSize(); ++i) {
                Aircraft& ac = ac_cont.getAircraft(i);
                if (ac.aircraft_type == -1) {
                    adsb_parser.process(ac, str);
                } else {
                    ogn_parser.process(ac, str);
                }
                out_con.sendMsgOut(str);
            }
            adsb_parser.gprmc(str);
            int c = out_con.sendMsgOut(str);
            adsb_parser.gpgga(str);
            out_con.sendMsgOut(str);
            //std::cout << "clients: " << c << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(SYNC_TIME));
        }

        adsb_in_thread.join();
        ogn_in_thread.join();
        con_out_thread.join();
        con_adsb_thread.join();
        con_ogn_thread.join();

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return;
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

void VFRB::handle_con_adsb(ConnectInADSB& adsb_con)
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

void VFRB::handle_con_ogn(ConnectInOGN& ogn_con)
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

void VFRB::handle_adsb_in(ConnectInADSB& adsb_con, AircraftContainer& ac_cont)
{
    ParserADSB parser(base_latitude, base_longitude, base_altitude, base_geoid);
    std::unique_lock<std::mutex> lock(mutex);
    con_adsb_cond.wait(lock);
    lock.unlock();

    std::cout << "Scan for incoming adsb-msgs..." << std::endl;

    while (1) {
        if (adsb_con.readLineIn(adsb_con.getInSock()) <= 0) {
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

void VFRB::handle_ogn_in(ConnectInOGN& ogn_con, AircraftContainer& ac_cont)
{
    ParserOGN parser(base_latitude, base_longitude, base_altitude, base_geoid);
    std::unique_lock<std::mutex> lock(mutex);
    con_ogn_cond.wait(lock);
    lock.unlock();

    std::cout << "Scan for incoming ogn-msgs..." << std::endl;

    while (1) {
        if (ogn_con.readLineIn(ogn_con.getInSock()) <= 0) {
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
