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
#include "ConnectInADSB.h"
#include "ConnectInOGN.h"
#include <iostream>
#include <cstring>

std::vector<Aircraft*> VFRB::vec;
std::mutex VFRB::vec_lock;

VFRB::VFRB()
{
}

VFRB::~VFRB()
{
}

void VFRB::run(long double latitude, long double longitude,
        int altitude, int out_port,  int ogn_port,  int adsb_port,
        const char* ogn_host, const char* adsb_host, const char* user, const char* pass)
{
    ConnectOutNMEA out_con(out_port);

    try{
        std::thread adsb_thread(do_adsb, latitude, longitude, altitude, adsb_host, adsb_port);
        std::thread ogn_thread(do_ogn, latitude, longitude, altitude, ogn_host, ogn_port, user, pass);
        std::thread conn_thread(handle_connections, &out_con);

        ParserADSB adsb_parser(latitude, longitude, altitude);
        ParserOGN ogn_parser(latitude, longitude, altitude);
        std::string str;
        while (1) {
            VFRB::vec_lock.lock();
            invalidateAircrafts();
            VFRB::vec_lock.unlock();

            for (Aircraft* ac : vec) {
                if (ac->aircraft_type == -1) {
                    adsb_parser.process(ac, str);
                    //std::cout << "from adsb:" << std::endl;
                } else {
                    ogn_parser.process(ac, str);
                    //std::cout << "from ogn:" << std::endl;
                }
                //std::cout << str;
                out_con.sendMsgOut(str);
            }
            adsb_parser.gprmc(str);
            //std::cout << str << std::endl;
            out_con.sendMsgOut(str);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        adsb_thread.join();
        ogn_thread.join();
        conn_thread.join();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return;
    }
    return;
}

int VFRB::vecfind(std::string& id)
{
    unsigned int i;
    for (i = 0; i < VFRB::vec.size(); ++i) {
        if (VFRB::vec.at(i)->id.compare(id) == 0) {
            return i;
        }
    }
    return -1;
}

void VFRB::pushAircraft(Aircraft* ac)
{
    VFRB::vec.push_back(ac);
    return;
}

void VFRB::invalidateAircrafts()
{
    unsigned int i;
    for (i = 0; i < VFRB::vec.size(); ++i) {
        VFRB::vec.at(i)->valid++;
        if (VFRB::vec.at(i)->valid >= INVALIDATE) {
            delete VFRB::vec.at(i);
            VFRB::vec.erase(VFRB::vec.begin() + i);
        }
    }
    return;
}

Aircraft* VFRB::getAircraft(int i)
{
    return VFRB::vec.at(i);
}

void VFRB::handle_connections(ConnectOutNMEA* out_con)
{
    if (out_con->listenOut() == -1) return;
    out_con->connectClient();
    return;
}

void VFRB::do_adsb(long double latitude, long double longitude, int altitude, const char* adsb_host, int adsb_port)
{
    ConnectInADSB adsb_con(adsb_host, adsb_port);
    ParserADSB parser(latitude, longitude, altitude);

    if (adsb_con.connectIn() == -1) return;

    std::cout << "Scan for incoming adsb-msgs..." << std::endl;

    while (1) {
        int error;
        if ((error = adsb_con.readLineIn(adsb_con.getAdsbInSock())) < 0) {
            continue;
        }
        //need msg3 only
        if (adsb_con.getResponse().at(4) == '3') {
            VFRB::vec_lock.lock();
            parser.unpack(adsb_con.getResponse());
            VFRB::vec_lock.unlock();
        }
    }
    return;
}

void VFRB::do_ogn(long double latitude, long double longitude, int altitude, const char* ogn_host, int ogn_port, const char* user, const char* pass)
{//"glidern1.glidernet.org", 14580, "D5234", "12772"
    ConnectInOGN ogn_con(ogn_host, ogn_port, user, pass);
    ParserOGN parser(latitude, longitude, altitude);

    if (ogn_con.connectIn() == -1) return;

    std::cout << "Scan for incoming ogn-msgs..." << std::endl;

    while (1) {
        int error;
        if ((error = ogn_con.readLineIn(ogn_con.getOgnInSock())) < 0) {
            continue;
        }
        VFRB::vec_lock.lock();//std::cout << ogn_con.getResponse() << std::endl;
        parser.unpack(ogn_con.getResponse());
        VFRB::vec_lock.unlock();
    }
    return;
}
