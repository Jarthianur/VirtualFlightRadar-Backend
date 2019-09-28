/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
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

#pragma once

/**
 * @file Parameters.h
 * This file contains some parameters,
 * which should be fixed at compile-time.
 *
 * !!! Note that some of those values may have a high impact
 * on the performance, memory/CPU usage, stability and
 * even may cause the program not to work properly !!!
 */

/**
 * @def CLIENT_CONNECT_WAIT_TIMEVAL
 * Input-clients wait for this duration until
 * attempting a connect (reconnect).
 * [1 <= x] seconds
 * This value depends on Your infrastructure.
 * E.g. if Your receivers run on different hosts, which get
 * turned on 1 hour after VFR-B, it is pointless to attempt connect
 * every second. As well as waiting 1 hour if all receivers are up
 * after 2 minutes.
 */
#define CLIENT_CONNECT_WAIT_TIMEVAL 120

/**
 * @def APRSCCLIENT_BEACON_INTERVAL
 * APRSC servers are often configured to disconnect clients after some time without any sign of
 * being alive. Hence a keep-alive beacon is sent to the server in this interval. [1 <= x] seconds
 * OGN servers have a timeout of 30 minutes, so 10 minutes are absolutely ok.
 */
#define APRSCCLIENT_BEACON_INTERVAL 600

/**
 * @def WINDCLIENT_RECEIVE_TIMEOUT
 * Due to unstable hardware/drivers, it became apparent that
 * it is necessary to timeout the wind-sensors input.
 * [1 <= x] seconds
 * A good way is to set this value to 1.5 * Y, where
 * Y is the time interval the wind-sensor sends its data.
 */
#define WINDCLIENT_RECEIVE_TIMEOUT 5

/**
 * @def SERVER_MAX_CLIENTS
 * Max amount of clients, which can connect to the VFR-B's
 * internal NMEA-server.
 * [1 <= x]
 * More clients, more network traffic; but at least 1 client is recommended.
 * Consider someone else wants surrounding traffic displayed from
 * somewhere else, like pilots and flight instructors.
 */
#define SERVER_MAX_CLIENTS 3

/**
 * @def ESTIMATED_TRAFFIC
 * Initial amount of space reserved for aircrafts.
 * The higher this value, the more memory is preallocated for the internal container.
 * This can increase the performance, but also uses up more RAM.
 * [0 <= x]
 * If traffic of 50 aircrafts is common at your base, set it accordingly.
 * If 10 aircrafts are a high amount of traffic,
 * there is no need to set it higher than that.
 * In doubt, set it to a lower value.
 */
#define ESTIMATED_TRAFFIC 10
