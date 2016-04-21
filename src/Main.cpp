/*
 * NMEA Protocol:
 * PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
 * PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>(,<ID>)
 * GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
 *
 * //--GLL Latitude,N/S,Longitude,E/W,UTC,Status (A=Valid, V=Invalid),Checksum
 * XCSoar connects on port 4353
 */

#include <iostream>
#include <cstring>
#include "Aircraft.h"
#include "ExtendedAircraft.h"
#include "ParserADSB.h"
#include "ParserOGN.h"
#include <thread>
#include "ConnectInADSB.h"
#include "ConnectInOGN.h"
#include "ConnectOutNMEA.h"
#include <vector>
#include <mutex>
#include <chrono>

using namespace std;

vector<Aircraft> vec;
mutex vec_lock;

int vecfind(Aircraft& ac)
{
    unsigned int i;
    for (i = 0; i < vec.size(); ++i) {
        if (vec.at(i).id.compare(ac.id) == 0) {
            return i;
        }
    }
    return -1;
}

void insertAircraft(Aircraft& ac)
{
    int i;
    if ((i = vecfind(ac)) > -1) {
        vec.erase(vec.begin() + i);
    }
    vec.push_back(ac);
    return;
}

//runs correctly
void do_adsb(/*host,port*/const char* out)
{
    ConnectInADSB adsb_con("localhost", 30003);
    ParserADSB parser(49.665263L, 9.003075L, 110);

    if (adsb_con.connectIn() == -1) return;

    cout << "Scan for incoming adsb-msgs..." << endl;
    Aircraft ac;
    string str;
    while (1) {
        int error;
        if ((error = adsb_con.readLineIn(adsb_con.getAdsbInSock())) < 0) {
            cout << error << endl;
        }
        //need msg3 only
        if (adsb_con.getResponse()[4] == '3') {
            if (parser.unpack(ac, adsb_con.getResponse()) == 0) {
                //process here for now, later in output thread
                parser.process(ac, str);
                if (strcmp(out, "-out")==0) {
                    cout << str << endl;
                }
                vec_lock.lock();
                insertAircraft(ac);
                vec_lock.unlock();
            }
        }
    }
    return;
}

//needs to be unpacked
void do_ogn(/*host,port*/const char* out)
{
    ConnectInOGN ogn_con("glidern1.glidernet.org", 14580, "D5234", "12772");
    ParserOGN parser(49.665263L, 9.003075L, 110);

    if (ogn_con.connectIn() == -1) return;

    cout << "Scan for incoming ogn-msgs..." << endl;
    ExtendedAircraft extAc;
    string str;
    while (1) {
        int error;
        if ((error = ogn_con.readLineIn(ogn_con.getOgnInSock())) < 0) {
            cout << "received from ogn : " << error << endl;
        }
        if (parser.unpack(extAc, ogn_con.getResponse()) == 0) {
            //parser.process(ac, str);
            cout << ogn_con.getResponse() << endl;
            if (strcmp(out, "-out") == 0) {
                //cout << str << endl;
                //cout << ogn_con.getResponse() << endl;
            }
            //vec_lock.lock();
            //vec.push_back(extAc);
            //vec_lock.unlock();
        }
    }
    return;
}

void handle_connections(const int port)
{
    ConnectOutNMEA out_con(port);
    //if (out_con.listenOut() == -1) return;
    //out_con.connectClient();

    ParserADSB parser(49.665263L, 9.003075L, 110);
    string str;
    while (1) {
        cout << "vec size: " << vec.size() << endl;
        for (Aircraft ac : vec) {
            parser.process(ac, str);
            cout << str << endl;
        }
        parser.gprmc(str);
        cout << str << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    return;
}


int main(int argc, char* argv[]) {

    string opt_outstr = "nop";
    if (argc == 3) opt_outstr = argv[2];
    const char* opt_out = opt_outstr.c_str();
    int port = atoi(argv[1]);

    thread t1(do_adsb, opt_out);
    thread t2(do_ogn, opt_out);
    thread t3(handle_connections, port);
    t1.join();
    t2.join();
    t3.join();

    /*ConnectorADSB ads("localhost", 30003, std::stoi(argv[/*4//1]));
   ParserADSB parser;

   if (ads.connectIn() == -1) return 0;

   if (/*(argc == 6) && (strcmp(argv[5], "-out")==0)//argc == 3 &&(strcmp(argv[2], "-out")==0)) {
      if (ads.listenOut() != 0) return 0;
      if (ads.connectClient() != 0) return 0;
      out = 1;
   }
   cout << "Scan for incoming msgs..." << endl;
   Aircraft ac;
   while (1) {
      int error;
      if ((error = ads.readLineIn(ads.getAdsbInSock())) < 0) {
         cout << error << endl;
         return -1;
      }
      if (ads.getResponse()[4] == '3') {
         //cout << ads.getResponse();

         if (parser.unpack(ac, ads.getResponse()) == 0) {
            //cout << "Aircraft " << ac.id << ", alt=" << std::to_string(ac.altitude);
            //cout << ", lat=" << std::to_string(ac.latitude) << ", long=" << std::to_string(ac.longitude);
            //cout << endl;
            std::string str;
            parser.process(ac, str, 49.665263L, 9.003075L, 110);
            //parser.process(ac, str, lat, lon, anth);
            if (out == 1) ads.sendMsgOut(str);
            else cout << str << endl;
         }
      }
   }*/
    return 0;
}
