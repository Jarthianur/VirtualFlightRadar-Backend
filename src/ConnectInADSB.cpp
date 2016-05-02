#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "ConnectInADSB.h"

ConnectInADSB::ConnectInADSB(const char* adsb_host, const int adsb_port)
: ConnectIn(adsb_host, adsb_port)
{
}

ConnectInADSB::~ConnectInADSB()
{
}

int ConnectInADSB::connectIn()
{
    //std::cout << "connect to adsb ..." << std::endl;

    if (::connect(in_sock, (struct sockaddr*) &in_addr, sizeof(struct sockaddr)) == -1) {
        //std::cout << "Could not connect to server!" << std::endl;
        return -1;
    }

    //std::cout << "connected to ADS-B" << std::endl;
    return 0;
}

int ConnectInADSB::setupConnectIn()
{
    if ((in_host_info = gethostbyname(in_hostname)) == NULL) {
        //std::cout << "Could not resolve Hostname!" << std::endl;
        return -1;
    }

    if ((in_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        //std::cout << "Could not create socket!" << std::endl;
        return -1;
    }

    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = htons(in_port);
    in_addr.sin_addr = *((struct in_addr*) in_host_info->h_addr);

    //std::cout << "adsb port= " << ntohs(in_addr.sin_port) << std::endl;
    //std::cout << "adsb addr= " << inet_ntoa(in_addr.sin_addr) << std::endl;
    return 0;
}
