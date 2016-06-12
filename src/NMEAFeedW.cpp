/*
 * NMEAFeedW.cpp
 *
 *  Created on: 10.06.2016
 *      Author: lula
 */

#include "NMEAFeedW.h"

NMEAFeedW::NMEAFeedW()
{
    nmea_str = "\r\n";
}

NMEAFeedW::~NMEAFeedW()
{
    // TODO Auto-generated destructor stub
}

std::string& NMEAFeedW::getNMEA()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return std::ref(nmea_str);
}

void NMEAFeedW::writeNMEA(const std::string& str)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (str.substr(0,6).compare("$WIMWV") == 0) {
        nmea_str = str;
    }
    return;
}
