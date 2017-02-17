/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
 A detailed list of copyright holders can be found in the file "AUTHORS".

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License version 3
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 }
 */

#ifndef APRSCCLIENT_H_
#define APRSCCLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "../../parser/APRSParser.h"
#include "Client.h"

class APRSCClient: public Client
{
public:
    APRSCClient(const APRSCClient&) = delete;
    APRSCClient& operator=(const APRSCClient&) = delete;

    APRSCClient(boost::asio::signal_set& s, const std::string& host,
            const std::string& port, const std::string& login);
    virtual ~APRSCClient() throw ();

private:
    void process();
    void connect();

    void handleResolve(const boost::system::error_code& ec,
            boost::asio::ip::tcp::resolver::iterator it);
    void handleConnect(const boost::system::error_code& ec,
            boost::asio::ip::tcp::resolver::iterator it);
    void handleLogin(const boost::system::error_code& ec, std::size_t s);

    std::string login_str;
    APRSParser parser;
};

#endif /* APRSCCLIENT_H_ */
