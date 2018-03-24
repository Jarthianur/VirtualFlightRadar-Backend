#!/bin/bash
REGR_PORT_SBS=44400
REGR_PORT_APRS=44401
REGR_PORT_GPS=44402
REGR_PORT_SENS=44403
REGR_PORT_VFRB=44404

function serve {
    local count=0
    while [ $count -le 15 ]
    do
    sleep 1
    count=$[count+1]
    cat $1
    done | nc -C -nlvp $2 > $3 &
}

case "$1" in
    "serve")
    serve resources/sbs_regr.txt $REGR_PORT_SBS /dev/null
    serve resources/aprs_regr.txt $REGR_PORT_APRS aprs.log
    serve resources/gps_regr.txt $REGR_PORT_GPS gps.log
    serve resources/sens_regr.txt $REGR_PORT_SENS /dev/null
    ;;
    "receive")
    nc localhost $REGR_PORT_VFRB > vfrb.log &
    ;;
    "check")
    LOG=$(cat aprs.log | grep -o "regression")
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    LOG=$(cat gps.log | grep -o "?WATCH={\"enable\":true,\"nmea\":true}")
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    LOG=$(cat gps.log | grep -o "?WATCH={\"enable\":false}")
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    LOG=$(cat vfrb.log | grep -o -P 'GPRMC,\d{6},A,5000.05,N' | head -n1)
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    LOG=$(cat vfrb.log | grep -o -P 'GPGGA,\d{6},5000.0466,N' | head -n1)
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    LOG=$(cat vfrb.log | grep -o -P 'PFLAU,,,,1,0,-180,0,-105' | head -n1)
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    LOG=$(cat vfrb.log | grep -o -P 'PFLAA,0,-11008362,-33512,-105' | head -n1)
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    LOG=$(cat vfrb.log | grep -o -P 'WIMDA,29.7987,I,1.0091,B,14.8' | head -n1)
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    LOG=$(cat vfrb.log | grep -o -P 'WIMWV,242.8,R,6.9,N,A' | head -n1)
    if [ "$LOG" == "" ]; then
        exit 1
    fi
    ;;
    *)
    echo "Run this script with 'serve' to start regression (netcat) servers."
    echo "Run this script with 'receive' to connect to regression (netcat) servers from 'serve' call."
    ;;
esac
exit 0