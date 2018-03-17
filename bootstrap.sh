#!/bin/bash

if [ "$(whoami)" != 'root' ]; then
    SUDO='sudo'
fi
export SUDO="${SUDO:-}"

function ifelse() {
    error=0
    if [ $1 -eq 0 ]; then
        printf '%s' "$2"
    else
        printf '%s' "$3"
        error=1
    fi
    return $error
}

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
    shift;
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

function resolve_pkg_manager() {
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
    log -i "... INSTALL DEPENDENCIES ..."
    resolve_pkg_manager
    require PKG_MANAGER
    case $PKG_MANAGER in
    apt-get)
        UPDATE="apt-get update"
        SETUP=""
        BOOST='libboost-dev libboost-all-dev'
        PYTHON='python python-pip'
    ;;
    yum)
        UPDATE='yum clean all'
        SETUP="yum -y install epel-release"
        BOOST='boost boost-devel'
        PYTHON='python python-pip'
    ;;
    esac
    ALL="$BOOST $PYTHON"
    $SUDO $SETUP
    $SUDO $UPDATE
    log -i "$SUDO" "$PKG_MANAGER" -y install "$ALL"
    $SUDO $PKG_MANAGER -y install $ALL
    pip install --upgrade pip
    pip install spline
    return $error
}
