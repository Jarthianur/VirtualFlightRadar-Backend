/*
 * ADSBparser.cpp
 *
 *  Created on: 13.04.2016
 *      Author: lula
 */

#include "ADSBparser.h"

ADSBparser::ADSBparser()
{
   // TODO Auto-generated constructor stub

}

ADSBparser::~ADSBparser()
{
   // TODO Auto-generated destructor stub
}

void ADSBparser::unpack(Aircraft& ac, std::string& sentence)
{
   std::string msg = sentence;
   /*
    * fields:
    * 4 : id
    * 11: altitude
    * 14: latitude
    * 15: longitude
    */
   msg.erase(0,6);
   int delim, index = 2;
   //last param in msg is unprocessed
   while((delim = msg.find(',')) > -1) {
      switch(index) {
         case 4:
            ac.id = msg.substr(0,delim);
            break;
         case 11:
            ac.altitude = std::stoi(msg.substr(0,delim), nullptr);
            break;
         case 14:
            ac.latitude = std::stod(msg.substr(0,delim), nullptr);
            break;
         case 15:
            ac.longitude = std::stod(msg.substr(0,delim), nullptr);
            break;
         default:
            break;
      }
      index++;
      msg.erase(0,delim+1);
   }
   return;
}
