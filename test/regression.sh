#!/bin/bash
REGR_PORT_SBS=44400
REGR_PORT_APRS=44401
REGR_PORT_GPS=44402
REGR_PORT_SENS=44403
REGR_PORT_VFRB=44404

function serve {
 | nc -l "$2" &
}

case "$1" in
    "serve")
    serve resources/sbs_regr.txt $REGR_PORT_SBS
    serve resources/aprs_regr.txt $REGR_PORT_APRS
    serve resources/gps_regr.txt $REGR_PORT_GPS
    serve resources/sens_regr.txt $REGR_PORT_SENS
    ;;
    "receive")
    nc localhost $REGR_PORT_VFRB > sbs.log &
    nc localhost $REGR_PORT_VFRB > aprs.log &
    nc localhost $REGR_PORT_VFRB > gps.log &
    nc localhost $REGR_PORT_VFRB > sens.log &
    ;;
esac
exit 0