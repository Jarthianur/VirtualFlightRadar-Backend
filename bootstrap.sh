#!/bin/bash

# working
if [ "$(whoami)" != 'root' ]; then
    SUDO='sudo'
fi
export SUDO="${SUDO:-}"
export PROMPT="$(basename $0)"

# working
function ifelse() {
    if [ $1 ]; then
        printf '%s' "$2"
    else
        printf '%s' "$3"
    fi
}

# working
function log() {
    local TIME=`date +"%T"`
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
    echo " $TIME $PROMPT$(ifelse "'${FUNCNAME[1]}' == ''" '' "->${FUNCNAME[1]}"):: $*"
}

# working
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

# working
function fail() {
    local MSG=''
    while [ $# -gt 0 ]; do
        case $1 in
        -e)
            $2
            shift
        ;;
        *)
            MSG="$MSG $1"
        ;;
        esac
        shift
    done
    log -e $MSG
    exit 1
}

# working
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

# working
function prepare_path() {
    set -e
    local error=0
    if [ -e "$1" ]; then
        log -w "\"$1\"" already exists.
        confirm Replace the existing one\?
        error=$?
        $(ifelse "$error -eq 0" "rm -rf '$1'" '')
    else
        mkdir -p "$(dirname $1)"
        error=$?
    fi
    return $error
}

# working
function resolve_pkg_manager() {
    local error=0
    if [ "$(which 2>&1)" == "" ]; then
        APT="$(which apt-get || true)"
        YUM="$(which yum || true)"
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

# working
function install_deps() {
    set -eE
    log -i INSTALL DEPENDENCIES
    trap "fail Failed to install dependencies!" ERR
    resolve_pkg_manager
    require PKG_MANAGER VFRB_COMPILER
    case $PKG_MANAGER in
    *apt-get)
        local UPDATE="apt-get update"
        local SETUP=''
        local BOOST='libboost-dev libboost-system-dev libboost-regex-dev'
        #local PYTHON='python python-pip'
        ! $VFRB_COMPILER -v > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            local GCC="$(basename "$VFRB_COMPILER")"
        fi
        GCC="$GCC make"
    ;;
    *yum)
        local UPDATE='yum clean all'
        local SETUP='yum -y install epel-release'
        local BOOST='boost boost-devel'
        #local PYTHON='python python-pip'
        local GCC='gcc-c++ make'
    ;;
    esac
    require GCC BOOST UPDATE
    ALL="$GCC"
    if [ -z "$CUSTOM_BOOST" ]; then
        ALL="$ALL $BOOST"
    fi
    $(ifelse "-z '$SETUP'" "$SUDO $SETUP" '')
    $SUDO $UPDATE
    log -i "$SUDO" "$PKG_MANAGER" -y install "$ALL"
    $SUDO $PKG_MANAGER -y install $ALL
    trap - ERR
    #pip install --upgrade pip
    #pip install spline
}

# test for custom boost
# working
function build() {
    set -eE
    log -i BUILD VFRB
    require VFRB_ROOT VFRB_TARGET VFRB_COMPILER
    if [ ! -z "$CUSTOM_BOOST" ]; then
        require BOOST_LIBS_L BOOST_ROOT_I
    fi
    export VFRB_OPT=${VFRB_OPT:-"3"}
    export VFRB_DEBUG=""
    trap "fail -e popd Build has failed!" ERR
    pushd "$VFRB_ROOT/build/"
    make clean 
    make all -j4
    popd
    trap - ERR
}

# working
function install_service() {
    set -eE
    log -i INSTALL VFRB SERVICE
    require VFRB_ROOT VFRB_EXEC_PATH VFRB_INI_PATH
    local SYSD_PATH="/etc/systemd/system"
    trap "fail -e popd Service installation has failed!" ERR
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

# working
function install() {
    set -eE
    log -i INSTALL VFRB
    trap "fail Failed to install VFRB!" ERR
    require VFRB_TARGET VFRB_ROOT VFRB_EXEC_PATH VFRB_INI_PATH VFRB_VERSION REPLACE_INI
    if [ -x "$VFRB_ROOT/build/$VFRB_TARGET" ]; then
        mv "$VFRB_ROOT/build/$VFRB_TARGET" "$VFRB_EXEC_PATH"
        log -i "$VFRB_EXEC_PATH" created.
    else
        log -e "$VFRB_TARGET" does not exist!
        return 1
    fi
    if [ $REPLACE_INI -eq 0 ]; then
        sed "s|%VERSION%|${VFRB_VERSION}|" <"$VFRB_ROOT/vfrb.ini" >"$VFRB_INI_PATH"
        log -i "$VFRB_INI_PATH" created.
    fi
    trap - ERR
}

# working
function install_test_deps() {
    set -eE
    log -i INSTALL TEST DEPENDENCIES
    trap "fail Failed to install test dependencies!" ERR
    resolve_pkg_manager
    require PKG_MANAGER
    case $PKG_MANAGER in
    *apt-get)
        local TOOLS='cppcheck clang-format-5.0 wget netcat procps perl'
    ;;
    *)
        log -w Tests currently only run under ubuntu/debian systems.
        return 1
    ;;
    esac
    require TOOLS
    ALL="$TOOLS"
    log -i "$SUDO" "$PKG_MANAGER" -y install "$ALL"
    $SUDO $PKG_MANAGER -y install $ALL
    if [ "$(lcov --version | grep -o '1.11')" != '1.11' ]; then
        require VFRB_ROOT
        trap "fail -e 'rm lcov_1.11.orig.tar.gz' -e popd Failed to install test dependencies!" ERR
        pushd $VFRB_ROOT
        wget 'http://ftp.de.debian.org/debian/pool/main/l/lcov/lcov_1.11.orig.tar.gz'
        tar xf lcov_1.11.orig.tar.gz
        $SUDO make -C lcov-1.11/ install
        rm lcov_1.11.orig.tar.gz
        popd
    fi
    trap - ERR
}

#working
function build_test() {
    set -eE
    log -i BUILD VFRB TESTS
    require VFRB_ROOT VFRB_COMPILER
    if [ ! -z "$CUSTOM_BOOST" ]; then
        require BOOST_LIBS_L BOOST_ROOT_I
    fi
    export VFRB_DEBUG="-g --coverage"
    export VFRB_OPT="0"
    trap "fail -e popd Build has failed!" ERR
    pushd "$VFRB_ROOT/test/build/"
    make clean
    make all -j2
    popd
    pushd "$VFRB_ROOT/build/"
    make clean
    make all -j2
    popd
    trap - ERR
}

# working
function static_analysis() {
    set -eE
    log -i RUN STATIC CODE ANALYSIS
    require VFRB_ROOT
    trap "fail -e popd Static code analysis failed!" ERR
    pushd $VFRB_ROOT
    cppcheck --enable=warning,style,performance,unusedFunction,missingInclude --error-exitcode=1 --inline-suppr -I src/ -q src/
    for f in $(find src/ -type f); do
        diff -u <(cat $f) <(clang-format-5.0 -style=file $f) || true
    done &> format.diff
    if [ "$(wc -l format.diff | cut -d' ' -f1)" -gt 0 ]; then
        log -e Code format does not comply to the specification.
        cat format.diff
        rm format.diff
        return 1
    fi
    rm format.diff
    popd
    trap - ERR
}

# working
function run_unit_test() {
    set -eE
    log -i RUN UNIT TESTS
    require VFRB_ROOT
    trap "fail -e popd Unit tests failed!" ERR
    pushd $VFRB_ROOT
    lcov --initial --directory test/build --capture --output-file reports/test_base.info
    lcov --initial --directory build --capture --output-file reports/vfrb_base.info
    ! test/build/VFR-Test &> reports/unittests.xml
    local error=$(ifelse "$? -eq 1" '0' '1')
    cat reports/unittests.xml
    $(exit $error)
    popd
    trap - ERR
}

# working
function run_regression() {
    set -eE
    log -i RUN REGRESSION TESTS
    require VFRB_ROOT
    VFRB_UUT="vfrb-$(cat $VFRB_ROOT/version.txt)"
    if ! $VFRB_ROOT/build/$VFRB_UUT; then $(exit 0); fi
    if ! $VFRB_ROOT/build/$VFRB_UUT -g -c bla.txt; then $(exit 0); fi
    trap "fail -e popd -e '$SUDO pkill -2 -f $VFRB_UUT' Regression tests have failed!" ERR
    pushd $VFRB_ROOT/test
    log -i Start mocking servers
    ./regression.sh serve
    sleep 2
    log -i Start vfrb
    ../build/$VFRB_UUT -c resources/test.ini &
    sleep 2
    log -i Connect to vfrb
    ./regression.sh receive
    ./regression.sh receive
    sleep 20
    log -i Stop vfrb and run check
    $SUDO pkill -2 -f $VFRB_UUT || true
    ./regression.sh check
    sleep 4
    popd
    trap - ERR
}

# working
function gen_coverage() {
    set -eE
    log -i GENERATE COVERAGE REPORT
    require VFRB_ROOT
    trap "fail -e popd Coverage report generation failed!" ERR
    pushd $VFRB_ROOT
    lcov --directory test/build --capture --output-file reports/test.info
    lcov --directory build --capture --output-file reports/vfrb.info
    lcov -a reports/test_base.info -a reports/test.info -a reports/vfrb_base.info -a reports/vfrb.info \
        -o reports/all.info
    lcov --remove reports/all.info 'test/*' '/usr/*' -o reports/cov.info
    lcov --list reports/cov.info
    popd
    trap - ERR
}
