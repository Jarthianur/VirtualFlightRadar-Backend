#!/bin/bash

if [ "$(whoami)" != 'root' ]; then
    SUDO='sudo'
fi
export SUDO="${SUDO:-}"

function ifelse() {
    error=0
    if [ $1 ]; then
        printf '%s' "$2"
    else
        printf '%s' "$3"
        error=1
    fi
    return $error
}

export PROMPT="$(basename $0)`ifelse "-z $1" '' "->$1"`"

function log() {
    TIME=`date +"%T"`
    case $1 in -i)
        echo -en "\033[0;32m[INFO]\033[0m"
    ;;
    -w)
        echo -en "\033[0;33m[WARN]\033[0m"
    ;;
    -e)
        echo -en "\033[0;31m[WARN]\033[0m"
    ;;
    esac
    shift
    echo ' '"$TIME $PROMPT:: $*"
}

function require() {
    error=0
    for v in $*; do
        if [ -z "${!v}" ]; then
            log -e Env. $v is not set!
            error=1
        fi
    done
    return $error
}

function retval() {
    "$*" &> /dev/null
    echo $?
}

function fail() {
    $1
    shift
    log -e $*
    exit 1
}

function confirm() {
    log -i $*
    if [ ! -z "$AUTO_CONFIRM" ]; then
        read -r -p "Confirm? [yes|no]: " READ
        echo ''
        case $READ in
        [yY] | yes | YES)
            return 0
        ;;
        *)
            return 1
        ;;
        esac
    fi
    log -w Automatically confirmed.
    return 0
}

function resolve_pkg_manager() {
    set +e
    error=0
    if [ "$(which 2>&1)" == "" ]; then
        APT="$(which apt-get)"
        YUM="$(which yum)"
    else
        RELEASE="$(cat /etc/*-release)"
        if [ "$(echo $RELEASE | grep -oiP '(ubuntu|debian)')" != "" ]; then
            APT="apt-get"
        elif [ "$(echo $RELEASE | grep -oiP '(centos|fedora)')" != "" ]; then
            YUM="yum"
        fi
    fi
    if [ ! -z "$APT" ]; then
        export PKG_MANAGER="$APT"
        log -i Using apt as package manager.
    elif [ ! -z "$YUM" ]; then
        export PKG_MANAGER="$YUM"
        log -i Using yum as package manager.
    else
        log -e Could not determine package manager!
        error=1
    fi
    return $error
}

function install_deps() {
    set -e
    log -i INSTALL DEPENDENCIES
    resolve_pkg_manager
    require PKG_MANAGER
    case $PKG_MANAGER in
    apt-get)
        UPDATE="apt-get update"
        SETUP=""
        BOOST='libboost-dev libboost-all-dev'
        PYTHON='python python-pip'
        GCC='g++ g++-multilib'
    ;;
    yum)
        UPDATE='yum clean all'
        SETUP="yum -y install epel-release"
        BOOST='boost boost-devel'
        PYTHON='python python-pip'
        GCC='gcc-c++'
    ;;
    esac
    ALL="$GCC $PYTHON"
    if [ -z "$CUSTOM_BOOST" ]; then
        ALL="$ALL $BOOST"
    fi
    $SUDO $SETUP
    $SUDO $UPDATE
    log -i "$SUDO" "$PKG_MANAGER" -y install "$ALL"
    $SUDO $PKG_MANAGER -y install $ALL
    pip install --upgrade pip
    pip install spline
    return $error
}

function prepare_path() {
    set -e
    if [ -e "$1" ]; then
        log -w "\"$1\"" already exists.
        confirm Replace the existing one\?
        rm -rf "$1"
    else
        mkdir -p "$(dirname $1)"
    fi
}

function build() {
    set -eE
    log -i BUILD VFRB
    require VFRB_ROOT VFRB_TARGET VFRB_COMPILER
    if [ ! -z "$CUSTOM_BOOST" ]; then
        require BOOST_LIBS_L BOOST_ROOT_I
    fi
    export VFRB_OPT=${VFRB_OPT:-"3"}
    export VFRB_DEBUG=""
    trap "fail popd Build has failed!" ERR
    pushd "$VFRB_ROOT/target/"    
    make all -j2
    popd
    trap - ERR
}

function install_service() {
    set -eE
    log -i INSTALL VFRB SERVICE
    require VFRB_ROOT VFRB_EXEC_PATH VFRB_INI_PATH
    SYSD_PATH="/etc/systemd/system"
    trap "fail popd Service installation has failed!" ERR
    pushd "$SYSD_PATH"
    if [ ! -z "$CUSTOM_BOOST" ]; then
        require BOOST_ROOT
        $SUDO mkdir "vfrb.service.d"
        $SUDO sh -c "sed -e 's|%BOOST_LIBS_PATH%|${BOOST_ROOT}/stage/lib:|' \
        <'$VFRB_ROOT/service/vfrb.service.d/vfrb.conf' >vfrb.service.d/vfrb.conf"
    fi
    $SUDO sh -c "sed -e 's|%VFRB_EXEC_PATH%|${VFRB_EXEC_PATH}|' \
        -e 's|%VFRB_INI_PATH%|${VFRB_INI_PATH}|' \
        < ${VFRB_ROOT}/service/vfrb.service >vfrb.service"
    $SUDO systemctl enable vfrb.service
    log -i VFRB service created.
    trap - ERR
}

function install() {
    set -e
    log -i INSTALL VFRB
    require VFRB_TARGET VFRB_ROOT VFRB_EXEC_PATH VFRB_INI_PATH VFRB_VERSION REPLACE_INI
    if [ -x "$VFRB_ROOT/target/$VFRB_TARGET" ]; then
        mv "$VFRB_ROOT/target/$VFRB_TARGET" "$VFRB_EXEC_PATH"
        log -i "$VFRB_EXEC_PATH" created.
    else
        log -e "$VFRB_TARGET" does not exist!
        return 1
    fi
    if [ $[REPLACE_INI] -eq 0 ]; then
        sed "s|%VERSION%|${VFRB_VERSION}|" <"$VFRB_ROOT/vfrb.ini" >"$VFRB_INI_PATH"
        log -i "$VFRB_INI_PATH" created.
    fi
    install_service
}