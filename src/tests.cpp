/*
 * tests.cpp
 *
 *  Created on: 26.04.2016
 *      Author: lula
 */

#include "VFRB.h"
#include "ParserADSB.h"
#include "ParserOGN.h"
#include "Aircraft.h"
#include <iostream>

ParserADSB p_ads;
ParserOGN p_ogn;
long double base_lat, base_long, base_alt;
std::vector<std::string> adsb_msgs_valid;
    std::vector<std::string> ogn_msgs_valid;
    std::vector<std::string> adsb_msgs_invalid;
    std::vector<std::string> ogn_msgs_invalid;

void fill_valid_adsb()
{

}

void fill_invalid_adsb()
{

}

void fill_valid_ogn()
{

}

void fill_invalid_ogn()
{

}

//before
void setup_run()
{
    base_lat = 49.665263L;
    base_long = 9.003075L;
    base_alt = 110;
    p_ads(base_lat, base_long, base_alt);
    p_ogn(base_lat, base_long, base_alt);

    test_adsb_unpack();
    test_adsb_process();

    test_ogn_unpack();
    test_ogn_process();

    test_parser_checksum();
    test_parser_degree();
    test_parser_dmsToDeg();
    test_parser_radian();
    test_parser_gprmc();
    test_parser_ldToI();
    test_parser_calcPosInfo();

    test_vfrb_vecfind();
    test_vfrb_invalidateAircrafts();

}

void test_adsb_unpack()
{

}

void test_adsb_process()
{

}

void test_ogn_unpack()
{

}

void test_ogn_process()
{

}

void test_parser_gprmc()
{

}

void test_parser_checksum()
{

}

void test_parser_ldToI()
{

}

void test_parser_calcPosInfo()
{

}

void test_parser_radian()
{

}

void test_parser_degree()
{

}

void test_parser_dmsToDeg()
{

}

void test_vfrb_vecfind()
{

}

void test_vfrb_invalidateAircrafts()
{

}
