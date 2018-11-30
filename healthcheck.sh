#!/bin/sh

curl -m 2 -f -s -o /dev/null localhost:4353
err=$?
if [ $err -eq 28 ] || [ $err -eq 52 ]; then
    exit 0
fi
exit 1
