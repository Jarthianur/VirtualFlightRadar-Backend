# Copyright_License {
#
# Copyright (C) 2016 VirtualFlightRadar-Backend
# A detailed list of copyright holders can be found in the file "AUTHORS".
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License version 3
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# }

# set SUDO if not root
if [ $(id -u) -ne 0 ]; then
    SUDO='sudo'
fi
export SUDO="${SUDO:-}"

# set logging PROMPT
export PROMPT="$(basename $0)"

# logging function
# param 1 may indicate log level
# all other params are printed
function log() {
    local TIME=$(date +"%T")
    case $1 in
    -i)
        echo -en "\033[0;32m[INFO ]\033[0m"
        shift
        ;;
    -w)
        echo -en "\033[0;33m[WARN ]\033[0m"
        shift
        ;;
    -e)
        echo -en "\033[0;31m[ERROR]\033[0m"
        shift
        ;;
    esac
    local ROUTINE=''
    if [ -n "${FUNCNAME[1]}" ]; then
        ROUTINE="->${FUNCNAME[1]}"
    fi
    echo " ${TIME} ${PROMPT}${ROUTINE}:: $*"
}

# require an env var to be set and not empty
# params are the var names
function require() {
    local error=0
    for v in $*; do
        if [ -z "${!v}" ]; then
            log -e Env. $v is not set!
            error=1
        fi
    done
    return $error
}

# exit the script with 1 and execute/log given params
# all params preceeded by -e are executed
# all other params are logged
function fail() {
    local MSG=''
    while [ $# -gt 0 ]; do
        case $1 in
        -e)
            $2
            shift
            ;;
        *)
            MSG="${MSG} $1"
            ;;
        esac
        shift
    done
    log -e ${MSG}
    exit 1
}

# wait for user input to confirm or deny
# all params are logged
# if AUTO_CONFIRM is set skip
function confirm() {
    log -i $*
    if [ -z "${AUTO_CONFIRM}" ]; then
        read -r -p "Confirm? [yes|no]: " READ
        case ${READ} in
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

# set PKG_MANAGER to the distributions package manager
function resolve_pkg_manager() {
    local error=0
    if [ "$(which 2>&1)" == "" ]; then
        APT="$(which apt-get || true)"
        YUM="$(which yum || true)"
        DNF="$(which dnf || true)"
        APK="$(which apk || true)"
    else
        RELEASE="$(cat /etc/*-release)"
        if [ "$(echo $RELEASE | grep -oiE '(ubuntu|debian)')" != "" ]; then
            APT="apt-get"
        elif [ "$(echo $RELEASE | grep -oiE '(centos)')" != "" ]; then
            YUM="yum"
        elif [ "$(echo $RELEASE | grep -oiE '(fedora)')" != "" ]; then
            DNF="dnf"
        elif [ "$(echo $RELEASE | grep -oiE 'alpine')" != "" ]; then
            APK="apk"
        fi
    fi
    if [ -n "$APT" ]; then
        export PKG_MANAGER="$APT"
        log -i Using apt as package manager.
    elif [ -n "$YUM" ]; then
        export PKG_MANAGER="$YUM"
        log -i Using yum as package manager.
    elif [ -n "$DNF" ]; then
        export PKG_MANAGER="$DNF"
        log -i Using dnf as package manager.
    elif [ -n "$APK" ]; then
        export PKG_MANAGER="$APK"
        log -i Using apk as package manager.
    else
        log -e Could not determine package manager!
        error=1
    fi
    return $error
}

# install all build dependencies
function install_deps() {
    set -eE
    log -i INSTALL DEPENDENCIES
    trap "fail Failed to install dependencies!" ERR
    resolve_pkg_manager
    require PKG_MANAGER
    case $PKG_MANAGER in
    *apt-get)
        local UPDATE='apt-get update'
        local SETUP=''
        local INSTALL='install -y'
        local BOOST='libboost-dev libboost-system-dev libboost-regex-dev libboost-program-options-dev'
        local GCC="g++ make cmake"
        ;;
    *yum)
        local UPDATE=''
        local SETUP='yum -y install epel-release'
        local INSTALL='install -y'
        local BOOST='boost boost-devel'
        local GCC="gcc-c++ make cmake"
        ;;
    *dnf)
        local UPDATE=''
        local SETUP=''
        local INSTALL='install -y'
        local BOOST='boost boost-devel'
        local GCC="gcc-c++ make cmake"
        ;;
    *apk)
        local UPDATE='apk update'
        local SETUP=''
        local INSTALL='add'
        local BOOST='boost-dev boost-system boost-regex boost-program_options'
        local GCC='g++ make cmake'
        ;;
    esac
    require GCC BOOST INSTALL
    ALL="$GCC"
    if [ -z "$CUSTOM_BOOST" ]; then
        ALL="$ALL $BOOST"
    fi
    if [ -n "$UPDATE" ]; then $SUDO $UPDATE; fi
    if [ -n "$SETUP" ]; then $SUDO $SETUP; fi
    log -i "$SUDO" "$PKG_MANAGER" "$INSTALL" "$ALL"
    $SUDO $PKG_MANAGER $INSTALL $ALL
    trap - ERR
}

# build for production
function build() {
    set -eE
    log -i BUILD VFRB
    require VFRB_ROOT
    local LS=""
    if [ -n "${VFRB_LINK_STATIC}" ]; then
        LS="-DBOOST_STATIC=1 --no-warn-unused-cli"
    fi
    if [ -n "${VFRB_BIN_TAG}" ]; then
        LS="$LS -DVFRB_BIN_TAG=$VFRB_BIN_TAG"
    fi
    trap "fail -e popd Build has failed!" ERR
    pushd $VFRB_ROOT/build/
    cmake .. $LS
    make release -j$(nproc)
    popd
    trap - ERR
}

# install systemd service
function install() {
    set -eE
    log -i INSTALL VFRB SERVICE
    require VFRB_ROOT VFRB_INI
    trap "fail -e popd Service installation has failed!" ERR
    pushd $VFRB_ROOT/build/
    $SUDO make install
    $SUDO systemctl enable vfrb.service
    log -i VFRB service created.
    popd
    trap - ERR
}

# install test dependencies
function install_test_deps() {
    set -eE
    log -i INSTALL TEST DEPENDENCIES
    trap "fail Failed to install test dependencies!" ERR
    resolve_pkg_manager
    require PKG_MANAGER
    case $PKG_MANAGER in
    *apt-get)
        local TOOLS='cppcheck clang-format-6.0 wget netcat procps perl lcov lua5.3 lua-argparse lua-socket lua-posix'
        ;;
    *)
        log -e Tests currently only run under ubuntu/debian systems.
        return 1
        ;;
    esac
    require TOOLS
    ALL="$TOOLS"
    log -i "$SUDO" "$PKG_MANAGER" -y install "$ALL"
    $SUDO $PKG_MANAGER -y install $ALL
    trap - ERR
}

# build for testing
function build_test() {
    set -eE
    log -i BUILD VFRB TESTS
    require VFRB_ROOT
    trap "fail -e popd Build has failed!" ERR
    pushd "$VFRB_ROOT/build/"
    cmake ..
    make unittest regression -j$(nproc)
    popd
    trap - ERR
}

# perform static code analysis
function static_analysis() {
    set -eE
    log -i RUN STATIC CODE ANALYSIS
    require VFRB_ROOT
    trap "fail -e popd Static code analysis failed!" ERR
    pushd $VFRB_ROOT
    cppcheck --enable=warning,style,performance,unusedFunction,missingInclude -q -I include/ src/
    local FORMAT="clang-format-6.0"
    ! $FORMAT --version >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        FORMAT="clang-format"
    fi
    for f in $(find src/ include/ -type f); do
        diff -u <(cat $f) <($FORMAT -style=file $f) || true
    done &>/tmp/format.diff
    if [ "$(wc -l /tmp/format.diff | cut -d' ' -f1)" -gt 0 ]; then
        log -e Code format does not comply to the specification.
        cat /tmp/format.diff
        rm /tmp/format.diff
        return 1
    fi
    rm /tmp/format.diff
    popd
    trap - ERR
}

# execute testing binary
function run_unit_test() {
    set -eE
    log -i RUN UNIT TESTS
    require VFRB_ROOT
    local error=1
    local VFRB_UUT="$(find $VFRB_ROOT/build/ -name '*vfrb_test-*' -executable | head -n1)"
    trap "fail -e popd Unit tests failed!" ERR
    pushd $VFRB_ROOT
    lcov -i -d build/CMakeFiles/unittest.dir -c -o reports/test_base.info
    ! $VFRB_UUT &>reports/unittests.xml
    if [ $? -eq 1 ]; then
        error=0
    fi
    cat reports/unittests.xml
    $(exit $error)
    popd
    trap - ERR
}

# execute regression tests
function run_regression() {
    set -eE
    log -i RUN REGRESSION TESTS
    require VFRB_ROOT
    local VFRB_UUT="$(find $VFRB_ROOT/build/ -name '*vfrb_regression-*' -executable | head -n1)"
    lcov -i -d $VFRB_ROOT/build/CMakeFiles/regression.dir -c -o $VFRB_ROOT/reports/vfrb_base.info
    if ! $VFRB_UUT; then $(exit 0); fi
    if ! $VFRB_UUT -v -g -c bla.txt; then $(exit 0); fi
    trap "fail -e popd -e '$SUDO pkill -2 -f $VFRB_UUT' Regression tests have failed!" ERR
    pushd $VFRB_ROOT/test/resources
    log -i Start mocking servers
    ./regression.sh serve
    sleep 2
    log -i Start vfrb
    $VFRB_UUT -c test.ini &
    sleep 2
    log -i Connect to vfrb
    ./regression.sh receive
    ./regression.sh receive
    sleep 5
    log -i Stop vfrb and run check
    $SUDO pkill -2 -f $VFRB_UUT || true
    sleep 4
    ./regression.sh check
    log -i "Test for reconnects"
    $VFRB_UUT -c test.ini >vfrb.log 2>&1 &
    sleep 5
    ./regression.sh serve
    $SUDO pkill -2 -f $VFRB_UUT || true
    # just to cleanup servers
    ./regression.sh check >/dev/null 2>&1 || true
    local OUTPUT="$(cat vfrb.log)"
    echo $OUTPUT
    if [ $(echo $OUTPUT | grep -o 'connected to' | wc -l) -lt 4 ]; then
        log -e "reconnect test failed"
        $(exit 1)
    fi
    log -i Test windclient timeout
    lua server.lua 44405 nil >serv.log 2>&1 &
    local S_PID=$!
    $VFRB_UUT -c test.ini >/dev/null 2>&1 &
    sleep 10
    $SUDO pkill -2 -f $VFRB_UUT || true
    kill -9 $S_PID
    OUTPUT="$(cat serv.log)"
    if [ $(echo $OUTPUT | grep -o 'Connection from' | wc -l) -lt 2 ]; then
        log -e "timeout test failed"
        $(exit 1)
    fi
    popd
    trap - ERR
}

# generate coverage report for unit and regression tests
function gen_coverage() {
    set -eE
    log -i GENERATE COVERAGE REPORT
    require VFRB_ROOT
    trap "fail -e popd Coverage report generation failed!" ERR
    pushd $VFRB_ROOT
    lcov -d build/CMakeFiles/unittest.dir -c -o reports/test.info
    lcov -d build/CMakeFiles/regression.dir -c -o reports/vfrb.info
    lcov -a reports/test_base.info -a reports/test.info -a reports/vfrb_base.info -a reports/vfrb.info \
        -o reports/all.info
    lcov --remove reports/all.info '*test/*' '/usr/*' -o reports/lcov.info
    lcov --list reports/lcov.info
    popd
    trap - ERR
}

# build docker image for production
function docker_image() {
    set -eE
    log -i BUILD DOCKER IMAGE
    require VFRB_ROOT VFRB_VERSION
    if [ ! -x /usr/bin/docker ]; then
        log -e docker must be installed to create a docker image
        return 1
    fi
    trap "fail -e popd Docker image creation failed!" ERR
    pushd $VFRB_ROOT
    $SUDO docker build . -t user/vfrb:latest -t user/vfrb:${VFRB_VERSION}
    popd
    trap - ERR
}
