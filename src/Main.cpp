/*
 * NMEA Protocol:
 * PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
 * PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>(,<ID>)
 * GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
 *
 * //--GLL Latitude,N/S,Longitude,E/W,UTC,Status (A=Valid, V=Invalid),Checksum
 * XCSoar connects on port 4353
 */

#include "VFRB.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    //call: ./Flugleitertool 49.665263 9.003075 110 4353 14580 30003 localhost localhost D5234 12772

    if (argc < 11) {
        cout << "usage: ./Flugleitertool long lat alt out_port ogn_port adsb_port ogn_host adsb_host user pass"<< endl;
        return 0;
    }

    long double latitude = stold(argv[1]);
    long double longitude = stold(argv[2]);
    int altitude = atoi(argv[3]);
    int out_port = atoi(argv[4]);
    int ogn_port = atoi(argv[5]);
    int adsb_port = atoi(argv[6]);
    const string ogn_host(argv[7]);
    const string adsb_host(argv[8]);
    const string user(argv[9]);
    const string pass(argv[10]);

    VFRB::run(latitude, longitude, altitude, out_port, ogn_port, adsb_port, ogn_host.c_str(), adsb_host.c_str(), user.c_str(), pass.c_str());

    return 0;
}
