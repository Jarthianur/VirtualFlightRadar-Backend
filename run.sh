#!/bin/bash

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

set -e

# set env vars
VFRB_ROOT=${BUILD_DIR:-$PWD}
export VFRB_VERSION=$(cat "$VFRB_ROOT/version.txt" | tr -d '\n')
export VFRB_INI="vfrb.ini"

source "$VFRB_ROOT/bootstrap.sh"

# print a description of usage and arguments
function print_help() {
    echo 'VirtualFlightRadar-Backend install script.'
    echo ''
    echo 'Usage: ./run.sh [OPTIONS] <TASKS>'
    echo ''
    echo 'OPTIONS:'
    echo '  --ini=<NAME>           : Set a custom name for the config file.'
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
    echo '            The vfrb.ini.in will be copied, so edit it before running this command.'
    echo ''
    echo 'ENVIRONMENT:'
    echo 'Following adjustments can be made with environment variables.'
    echo '  VFRB_LINK_STATIC : Link libraries static where possible.'
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
    --ini=*)
        VFRB_INI="${arg#*=}"
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
    format)
        for f in $(find "$VFRB_ROOT/src" "$VFRB_ROOT/include" -type f); do
            echo $f
            clang-format -i -style=file $f
        done
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

# task "docker"
if [ -n "$DO_DOCKER" ]; then
    docker_image
    log -i Run e.g. "'sudo docker run --name vfrb -p 4353:4353 -dit vfrb:latest -g'"
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
    if [ ! -d $VFRB_ROOT/build ]; then
        mkdir -p $VFRB_ROOT/build
    fi
    build
fi

# task "install"
if [ -n "$DO_INSTALL" ]; then
    if [ -f "$VFRB_ROOT/build/$VFRB_INI" ]; then
        log -w "\"$VFRB_ROOT/build/$VFRB_INI\"" already exists.
        ! confirm Replace the existing one\?
        if [ $? -eq 0 ]; then
            sed "s|%VERSION%|${VFRB_VERSION}|" <"$VFRB_ROOT/vfrb.ini.in" >"$VFRB_ROOT/build/$VFRB_INI"
            log -i "$VFRB_INI" created.
        fi
    fi
    install
fi

log -i FINISHED
