/*
 * NMEAFeedW.h
 *
 *  Created on: 10.06.2016
 *      Author: lula
 */

#ifndef NMEAFEEDW_H_
#define NMEAFEEDW_H_

#include <string>
#include <mutex>

class NMEAFeedW
{
public:
    NMEAFeedW();
    virtual ~NMEAFeedW();

    std::string& getNMEA();
    void writeNMEA(const std::string&);

private:
    std::mutex mutex;
    std::string nmea_str;
};

#endif /* NMEAFEEDW_H_ */
