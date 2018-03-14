#!/bin/bash

PROG=$(basename $0)
function log {
    echo "[$(date +%T)] $PROG: $*"
}

# setup vars
source bootstrap.sh
export VFRB_NAME=${VFRB_NAME_B:-vfrb}
export VFRB_INI=${VFRB_INI_B:-vfrb}
export VFRB_VERSION=$(cat version.txt)
export VFRB_COMPILER=${VFRB_COMPILER_B:-g++}
export BOOST_ROOT=${BOOST_ROOT_B:-}
export VFRB_EXEC_PATH=${VFRB_EXEC_PATH_B:-$PWD/target/}
export VFRB_INI_PATH=${VFRB_INI_PATH_B:-$PWD/target/}
export VFRB_TARGET=$VFRB_NAME-$VFRB_VERSION
VFRB_ROOT=$PWD
BOOST_MAN=0
INST_SERVICE=0
INST_DEPS=0

for arg in $*; do
    case "$arg" in
        [sS][eE][rR][vV][iI][cC][eE])
            INST_SERVICE=1
        ;;
        [dD][eE][pP][sS])
            INST_DEPS=1
        ;;
        [hH] | [hH][eE][lL][pP])
            echo "This is the VFR-B install script."
            echo "Usage: ./install.sh [help] [service] [deps]"
            echo "Given 'help','h' will display this message."
            echo "Given 'service' will install the VFR-B systemd service (run with 'sudo')."
            echo "Given 'deps' will install all dependencies like boost,... (run with 'sudo')."
            exit 1
        ;;
        *)
        ;;
    esac
done

##########################################
if [ $INST_DEPS -eq 1 ]; then
    log "... INSTALL DEPENDENCIES ..."
    sudo apt-get update
    sudo apt-get install libboost-dev libboost-system-dev libboost-thread-dev \
        libboost-regex-dev libboost-chrono-dev libboost-signals-dev
    echo ""
fi
log "... INSTALL VFRB ..."
echo ""

log "... CECK VARIABLES ..."
echo ""
log "WORKDIR = $VFRB_ROOT"
if [ -z "$VFRB_NAME_B" ]; then
    log "VFRB_NAME not set, using default: $VFRB_NAME"
else
    log "VFRB_NAME:" $VFRB_NAME
fi
if [ -z "$VFRB_INI_B" ]; then
    log "VFRB_INI not set, using default: $VFRB_INI"
else
    log "VFRB_INI: $VFRB_INI"
fi
if [ -z "$VFRB_VERSION" ]; then
    log "VFRB_VERSION not set"
else
    log "VFRB_VERSION: $VFRB_VERSION"
fi
if [ -z "$VFRB_COMPILER_B" ]; then
    log "VFRB_COMPILER not set, using default: $VFRB_COMPILER"
else
    log "VFRB_COMPILER: $VFRB_COMPILER"
fi
if [ -z "$BOOST_ROOT_B" ]; then
    log "BOOST_ROOT not set, assuming default"
else
    log "BOOST_ROOT: $BOOST_ROOT"
    export BOOST_LIBS_L="-L${BOOST_ROOT}/stage/lib"
    export BOOST_ROOT_I="-I${BOOST_ROOT}"
    BOOST_MAN=1
fi
if [ -z "$VFRB_EXEC_PATH_B" ]; then
    log "VFRB_EXEC_PATH not set, using default: $VFRB_EXEC_PATH"
else
    log "VFRB_EXEC_PATH: $VFRB_EXEC_PATH"
fi
if [ -z "$VFRB_INI_PATH_B" ]; then
    log "VFRB_INI_PATH not set, using default: $VFRB_INI_PATH"
else
    log "VFRB_INI_PATH: $VFRB_INI_PATH"
fi
unset VFRB_NAME_B
unset VFRB_INI_B
unset VFRB_COMPILER_B
unset BOOST_ROOT_B
unset VFRB_EXEC_PATH_B
unset VFRB_INI_PATH_B
echo ""
log "... RUN MAKE ..."
echo ""
pushd ${VFRB_ROOT}/target/
export VFRB_OPT=${VFRB_OPT:-"3"}
make all -j2
error=$?
unset BOOST_LIBS_L
unset BOOST_ROOT_I
popd
if [ $error -ne 0 ]; then
    log "ERROR: make failed with $error"
    exit $error
fi
echo ""
log "... COPY TARGETS ..."
echo ""
if [ -x "${VFRB_ROOT}/target/${VFRB_TARGET}" ]; then
    mv ${VFRB_ROOT}/target/${VFRB_TARGET} ${VFRB_EXEC_PATH}/${VFRB_TARGET}
    echo ""
    log "$VFRB_TARGET copied to $VFRB_EXEC_PATH"
else
    echo ""
    log "ERROR: $VFRB_TARGET does not exist"
    exit 1
fi
sed "s|%VERSION%|${VFRB_VERSION}|" <${VFRB_ROOT}/vfrb.ini >${VFRB_INI_PATH}/${VFRB_INI}.ini
log "${VFRB_INI}.ini copied to $VFRB_INI_PATH"
echo ""
BASH_LD=$(cat ~/.bashrc | grep "export LD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib/:\$LD_LIBRARY_PATH")
if [ $BOOST_MAN -eq 1 ] && [ "$BASH_LD" == "" ]; then
    log "... ADD TO LD_LIBRARY_PATH ..."
    echo ""
    log "export LD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib/:\$LD_LIBRARY_PATH" >> ~/.bashrc
    log "export LD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib/:\$LD_LIBRARY_PATH"
    echo ""
fi
if [ $INST_SERVICE -eq 1 ]; then
    log "... SETUP SERVICE ..."
    echo ""
    SYSD_PATH="/etc/systemd/system"
    if [ $BOOST_MAN -eq 1 ]; then
        sudo mkdir ${SYSD_PATH}/${VFRB_NAME}.service.d
        pushd ${SYSD_PATH}/${VFRB_NAME}.service.d
        sudo sed -e "s|%BOOST_LIBS_PATH%|${BOOST_ROOT}/stage/lib:|" <${VFRB_ROOT}/service/vfrb.service.d/vfrb.conf >${VFRB_NAME}.conf
        popd
    fi
    pushd $SYSD_PATH
    echo ""
    sudo sh -c "sed -e 's|%VFRB_NAME%|${VFRB_NAME}|' \
        -e 's|%VFRB_EXEC_PATH%|${VFRB_EXEC_PATH}/${VFRB_TARGET}|' \
        -e 's|%VFRB_INI_PATH%|${VFRB_INI_PATH}/${VFRB_INI}.ini|' \
        < ${VFRB_ROOT}/service/vfrb.service >${VFRB_NAME}.service"
    log "${VFRB_NAME}.service created in $SYSD_PATH"
    sudo systemctl enable ${VFRB_NAME}.service
    echo ""
    popd
    echo ""
fi
log "... FINISHED ..."
