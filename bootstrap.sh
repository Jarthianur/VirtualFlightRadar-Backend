#
#    Copyright (C) 2016 Jarthianur
#    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".
#
#    This file is part of VirtualFlightRadar-Backend.
#
#    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
#

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

# install all build dependencies
function install_deps() {
    set -eE
    log -i INSTALL DEPENDENCIES
    trap "fail Failed to install dependencies!" ERR
    $SUDO apt update
    $SUDO apt install -y build-essential g++ make cmake
    trap - ERR
}

# build for production
function build() {
    set -eE
    log -i BUILD VFRB
    require VFRB_ROOT
    local LS=""
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
    require VFRB_ROOT VFRB_CONF
    trap "fail -e popd Service installation has failed!" ERR
    pushd $VFRB_ROOT/build/
    $SUDO make install
    $SUDO systemctl enable --now vfrb.service
    log -i VFRB service created.
    popd
    trap - ERR
}

# install test dependencies
function install_test_deps() {
    set -eE
    log -i INSTALL TEST DEPENDENCIES
    trap "fail Failed to install test dependencies!" ERR
    $SUDO apt -y install clang-format lcov
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
    make unittest -j$(nproc)
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
    for f in $(find src/ include/ -type f); do
        diff -u <(cat $f) <(clang-format -style=file $f) || true
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
    ! $VFRB_UUT -c --xml &>reports/unittests.xml
    if [ $? -eq 1 ]; then
        error=0
    fi
    cat reports/unittests.xml
    $(exit $error)
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
    lcov -a reports/test_base.info -a reports/test.info -o reports/all.info
    lcov --remove reports/all.info '*test/*' '/usr/*' '*vendor/*' -o reports/lcov.info
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
    $SUDO docker build . -t vfrb:latest -t vfrb:${VFRB_VERSION}
    popd
    trap - ERR
}
