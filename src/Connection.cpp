/*
 * Connection.cpp
 *
 *  Created on: 09.06.2016
 *      Author: lula
 */

#include "Connection.h"
#include <unistd.h>

Connection::Connection()
{
}

Connection::~Connection()
{
    if (con_sock != -1) {
        close(con_sock);
        con_sock = -1;
    }
}

