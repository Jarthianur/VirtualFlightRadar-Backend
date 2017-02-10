#!/bin/bash

# setup vars
source bootstrap.sh
NAME=${NAME:-vfrb}
VERSION=$(cat version.txt)
COMPILER=${COMPILER:-g++}
BOOST_LIBS_PATH=${BOOST_LIBS_PATH:-}
VFRB_EXEC_PATH=${EXEC_PATH:-bin/$NAME}
VFRB_PROP_PATH=${PROP_PATH:-vfrb.properties}

##########################################
echo "BOOTSTRAP VFRB"
# error level
# 1: failure
# 2: boost libs at default
error=0

if [ -z ${NAME+x} ]; then
    echo "NAME not set, using default:"
else
    echo "NAME:"
fi
echo $NAME

if [ -z ${VERSION+x} ]; then
    echo "VERSION not set:"
    exit(1)
else
    echo "VERSION:"
fi
echo $VERSION

if [ -z ${COMPILER+x} ]; then
    echo "COMPILER not set, using default:"
else
    echo "COMPILER:"
fi
echo $COMPILER

if [ -z ${BOOST_LIBS_PATH+x} ]; then
    echo "BOOST_LIBS_PATH not set, assuming default"
    error=2
else
    echo "BOOST_LIBS_PATH:"
    echo $BOOST_LIBS_PATH
fi

echo $VFRB_EXEC_PATH
echo $VFRB_PROP_PATH

if [ -z ${+x} ]; then
    
else
    
fi
if [ -z ${+x} ]; then
    
else
    
fi
if [ -z ${+x} ]; then
    
else
    
fi

