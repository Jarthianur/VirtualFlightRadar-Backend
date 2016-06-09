/*
 * Connection.h
 *
 *  Created on: 09.06.2016
 *      Author: lula
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <netinet/in.h>

class Connection
{
public:
    Connection();
    virtual ~Connection();

    struct sockaddr_in con_addr;
    int con_sock = -1;
};

#endif /* CONNECTION_H_ */
