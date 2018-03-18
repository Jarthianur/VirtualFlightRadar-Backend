#!/bin/bash

if [ "$(whoami)" != 'root' ]; then
    SUDO='sudo'
fi
export SUDO="${SUDO:-}"

function ifelse() {
    if [ $1 ]; then
        printf '%s' "$2"
    else
        printf '%s' "$3"
    fi
}

export PROMPT="$(basename $0)"

function log() {
    PROMPT="$PROMPT"$(ifelse "'${FUNCNAME[1]}' == ''" '' "->${FUNCNAME[1]}")
    TIME=`date +"%T"`
    case $1 in
    -i)
        echo -en "\033[0;32m[INFO ]\033[0m"
    ;;
    -w)
        echo -en "\033[0;33m[WARN ]\033[0m"
    ;;
    -e)
        echo -en "\033[0;31m[ERROR]\033[0m"
    ;;
    esac
    shift
    echo " $TIME $PROMPT:: $*"
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
    if [ -z "$AUTO_CONFIRM" ]; then
        read -r -p "Confirm? [yes|no]: " READ
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
    log -i INSTALL DEPENDENCIES
    resolve_pkg_manager
    require PKG_MANAGER
    case $PKG_MANAGER in
    apt-get)
        UPDATE="apt-get update"
        SETUP=''
        BOOST='libboost-dev libboost-all-dev'
        PYTHON='python python-pip'
        GCC='g++ g++-multilib make'
    ;;
    yum)
        UPDATE='yum clean all'
        SETUP='yum -y install epel-release'
        BOOST='boost boost-devel'
        PYTHON='python python-pip'
        GCC='gcc-c++ make'
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
}

function install_test_deps() {
    set -e
    log -i INSTALL TEST DEPENDENCIES
    resolve_pkg_manager
    require PKG_MANAGER
    case $PKG_MANAGER in
    apt-get)
        TOOLS='cppcheck clang-format-5.0 wget'
    ;;
    *)
        log -w Tests currently only run under ubuntu/debian systems.
        return 1
    ;;
    esac
    ALL="$TOOLS"
    log -i "$SUDO" "$PKG_MANAGER" -y install "$ALL"
    $SUDO $PKG_MANAGER -y install $ALL
    wget 'http://ftp.de.debian.org/debian/pool/main/l/lcov/lcov_1.11.orig.tar.gz'
    tar xf lcov_1.11.orig.tar.gz
    make -C lcov-1.11/ install
}

function buid_test() {
    set -eE
    log -i BUILD VFRB TESTS
    require VFRB_ROOT VFRB_COMPILER
    if [ ! -z "$CUSTOM_BOOST" ]; then
        require BOOST_LIBS_L BOOST_ROOT_I
    fi
    export VFRB_DEBUG="-g --coverage"
    export VFRB_OPT="0"
    trap "fail popd Build has failed!" ERR
    pushd "$VFRB_ROOT/test/build/"
    make all -j2
    popd
    pushd "$VFRB_ROOT/target/"    
    make all -j2
    popd
    trap - ERR
}

function static_analysis() {
    set -e
    log -i RUN STATIC CODE ANALYSIS
    cppcheck --enable=warning,style,performance,unusedFunction,missingInclude -I src/ --error-exitcode=1 --inline-suppr -q src/
    for f in `find src/ -type f`; do
        diff -u <(cat "$f") <(clang-format-5.0 -style=file "$f")
    done &> format.diff
    if [ "`wc -l format.diff | cut -d' ' -f1`" -gt 0 ]; then
        log -w Code format does not comply to the specification.
        return 1
    fi
}

function run_unit_test() {
    set -e
    log -i RUN UNIT TESTS
    require VFRB_ROOT
    lcov --initial --directory "$VFRB_ROOT/test/build" --capture --output-file test_base.info
    lcov --initial --directory "$VFRB_ROOT/target" --capture --output-file vfrb_base.info
    "$VFRB_ROOT/test/build/VFR-Test"
}

function run_regression() {
    set -eE
    log -i RUN REGRESSION TESTS
    VFRB_UUT="vfrb-$(cat version.txt)"
    if ! target/$VFRB_UUT; then $(exit 0); fi
    if ! target/$VFRB_UUT -g -c bla.txt; then $(exit 0); fi
    trap "fail 'popd; $SUDO pkill -2 -f $VFRB_UUT' Regression tests have failed!" ERR
    pushd test
    ./regression.sh serve
    ../target/$VFRB_UUT -c resources/test.ini &
    ./regression.sh receive
    ./regression.sh receive
    sleep 20
    sudo pkill -2 -f $VFRB_UUT || true
    ./regression.sh check
    popd
    trap - ERR
}

function publish_coverage() {
    set -e
    log -i PUBLISH COEVRAGE
    lcov --directory test/build --capture --output-file test.info
    lcov --directory target --capture --output-file vfrb.info
    lcov -a test_base.info -a test.info -a vfrb_base.info -a vfrb.info -o all.info
    lcov --remove all.info 'test/*' '/usr/*' -o cov.info
    lcov --list cov.info
}
