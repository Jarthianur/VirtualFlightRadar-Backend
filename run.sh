#!/bin/bash

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

set -e

# set env vars
VFRB_ROOT=${WORKSPACE:-$PWD}
export BOOST_ROOT=${BOOST_ROOT:-}
export VFRB_VERSION=${VFRB_VERSION:-$(cat "$VFRB_ROOT/version.txt" | tr -d '\n')}
export VFRB_COMPILER=${VFRB_COMPILER:-g++}
export VFRB_TARGET="vfrb-$VFRB_VERSION"
VFRB_EXEC_PATH=${VFRB_EXEC_PATH:-"$VFRB_ROOT/build/$VFRB_TARGET"}
VFRB_INI_PATH=${VFRB_INI_PATH:-"$VFRB_ROOT/build/vfrb.ini"}

source "$VFRB_ROOT/bootstrap.sh"

# print a description of usage and arguments
function print_help() {
    echo 'VirtualFlightRadar-Backend install script.'
    echo ''
    echo 'Usage: ./run.sh [OPTIONS] <TASKS>'
    echo ''
    echo 'OPTIONS:'
    echo '  --path=<PATH/NAME>     : Set a custom path, including filename, where to install the executable.'
    echo '  --ini-path=<PATH/NAME> : Set a custom path, including filename, where to install the config file.'
    echo '  -y | --confirm-yes     : Automatically confirm all decisions. (dangerous)'
    echo '  -n | --no-update       : Disable package installation and updates'
    echo '  -h | --help            : Display this message'
    echo ''
    echo 'TASKS:'
    echo '  build   : Build the VFRB executable.'
    echo '  install : Build and install the VFRB executable, config file and service.'
    echo '  test    : Build and run the unit, regression tests and code analysis.'
    echo '            Also generate test/coverage report.'
    echo '  docker  : Build a minimal docker image. Cannot be combined with other tasks.'
    echo '            The vfrb.ini will be copied, so edit it before running this command.'
    echo ''
    echo 'ENVIRONMENT:'
    echo 'Following adjustments can be made with environment variables.'
    echo '  BOOST_ROOT       : Assume boost manually installed at this path.'
    echo '  VFRB_COMPILER    : Use this compiler.'
    echo '  VFRB_LINK_STATIC : Link libraries static where possible.'
    echo '  VFRB_EXEC_PATH   : Same as "--path=".'
    echo '  VFRB_INI_PATH    : Same as "--ini-path=".'
    echo '  VFRB_VERSION     : Set this version.'
    echo ''
}

# print help if no args given
if [ $# -eq 0 ]; then
    print_help
    exit 1
fi

# resolve given args
for arg in $@; do
    case $arg in
    --path=*)
        VFRB_EXEC_PATH="${arg#*=}"
    ;;
    --ini-path=*)
        VFRB_INI_PATH="${arg#*=}"
    ;;
    -y | --confirm-yes)
        export AUTO_CONFIRM=1
    ;;
    build)
        DO_BUILD=1
    ;;
    install)
        DO_INSTALL=1
        DO_BUILD=1
    ;;
    test)
        DO_TEST=1
    ;;
    docker)
        DO_DOCKER=1
    ;;
    -n | --no-update)
        NO_UPDATE=1
    ;;
    -h | --help | *)
        print_help
        exit 1
    ;;
    esac
done

# set boost paths if manually installed (deprecated)
if [ -n "$BOOST_ROOT" ]; then
    log -i Using custom boost: "$BOOST_ROOT"
    export BOOST_LIBS_L="-L${BOOST_ROOT}/stage/lib"
    export BOOST_ROOT_I="-I${BOOST_ROOT}"
    export CUSTOM_BOOST=1
fi

# task "docker"
if [ -n "$DO_DOCKER" ]; then
    docker_image
    log -i Run e.g. "'sudo docker run --name vfrb -p 4353:4353 -dit user/vfrb:latest -g'"
    exit 0
fi

# task "test"
if [ -n "$DO_TEST" ]; then
    if [ -z "$NO_UPDATE" ]; then
        install_deps
        install_test_deps
    fi
    if [ ! -d $VFRB_ROOT/reports/ ]; then
        mkdir $VFRB_ROOT/reports/
    fi
    static_analysis
    build_test
    run_unit_test
    run_regression
    gen_coverage
fi

# task "build"
if [ -n "$DO_BUILD" ]; then
    if [ -z "$NO_UPDATE" ]; then
        install_deps
    fi
    build
fi

# task "install"
if [ -n "$DO_INSTALL" ]; then
    if [ "$(basename $VFRB_INI_PATH | grep -o '.ini')" == "" ]; then
        log -e "\"$VFRB_INI_PATH\"" is not a valid path to an ini file!
        exit 1
    fi
    prepare_path "$VFRB_EXEC_PATH"
    REPLACE_INI=1
    ! prepare_path "$VFRB_INI_PATH"
    if [ $? -eq 1 ]; then
        REPLACE_INI=0
    fi
    export REPLACE_INI
    log -i VFRB executable will be at "$VFRB_EXEC_PATH"
    log -i VFRB ini file will be at "$VFRB_INI_PATH"
    install
    install_service
fi

log -i FINISHED
