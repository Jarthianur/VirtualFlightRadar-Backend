#!/bin/bash
set -eE

REGR_PORT_SBS=44400
REGR_PORT_APRS=44401
REGR_PORT_GPS=44402
REGR_PORT_SENS=44403
REGR_PORT_VFRB=44404

function cleanup() {
    kill -9 $(cat sbs_srv.pid) $(cat aprs_srv.pid) $(cat gps_srv.pid) $(cat sens_srv.pid)
    rm sens_srv.pid aprs_srv.pid sbs_srv.pid gps_srv.pid
}

case "$1" in
"serve")
    lua server.lua $REGR_PORT_SBS sbs >/dev/null &
    echo -en $! >sbs_srv.pid
    lua server.lua -r $REGR_PORT_APRS aprs >aprs.log &
    echo -en $! >aprs_srv.pid
    lua server.lua -r $REGR_PORT_GPS gps >gps.log &
    echo -en $! >gps_srv.pid
    lua server.lua $REGR_PORT_SENS sens >/dev/null &
    echo -en $! >sens_srv.pid
    ;;
"receive")
    nc localhost $REGR_PORT_VFRB >recv.log &
    ;;
"check")
    LOG=$(cat aprs.log | grep -o "regression")
    if [ "$LOG" == "" ]; then
        echo "APRS login regression failed"
        $(exit 1)
    fi
    LOG=$(cat aprs.log | grep -o "#keep-alive beacon" | wc -l)
    if [ $LOG -lt 5 ]; then
        echo "APRS keep-alive beacon regression failed"
        $(exit 1)
    fi
    LOG=$(cat gps.log | grep -o "?WATCH={\"enable\":true,\"nmea\":true}")
    if [ "$LOG" == "" ]; then
        echo "VFRB - gps_watch_enable regression failed"
        $(exit 1)
    fi
    LOG=$(cat gps.log | grep -o "?WATCH={\"enable\":false}")
    if [ "$LOG" == "" ]; then
        echo "VFRB - gps_watch_disable regression failed"
        $(exit 1)
    fi
    LOG=$(cat recv.log | grep -o -P 'GPRMC,\d{6},A,5000.05,N' | head -n1)
    if [ "$LOG" == "" ]; then
        echo "VFRB - gprmc regression failed"
        $(exit 1)
    fi
    LOG=$(cat recv.log | grep -o -P 'GPGGA,\d{6},5000.0466,N' | head -n1)
    if [ "$LOG" == "" ]; then
        echo "VFRB - gpgga regression failed"
        $(exit 1)
    fi
    LOG=$(cat recv.log | grep -o 'PFLAU,,,,1,0,-180,0,-105' | head -n1)
    if [ "$LOG" == "" ]; then
        echo "VFRB - pflau regression failed"
        $(exit 1)
    fi
    LOG=$(cat recv.log | grep -o 'PFLAA,0,-11008362,-33512,-105' | head -n1)
    if [ "$LOG" == "" ]; then
        echo "VFRB - pflaa regression failed"
        $(exit 1)
    fi
    LOG=$(cat recv.log | grep -o 'WIMDA,29.7987,I,1.0091,B,14.8' | head -n1)
    if [ "$LOG" == "" ]; then
        echo "VFRB - wimda regression failed"
        $(exit 1)
    fi
    LOG=$(cat recv.log | grep -o 'WIMWV,242.8,R,6.9,N,A' | head -n1)
    if [ "$LOG" == "" ]; then
        echo "VFRB - wimwv regression failed"
        $(exit 1)
    fi
    cleanup
    ;;
*)
    echo "Run this script with 'serve' to start regression servers."
    echo "Run this script with 'receive' to connect to regression servers from 'serve' call."
    ;;
esac
exit 0
