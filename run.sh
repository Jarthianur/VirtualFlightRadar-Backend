#!/bin/bash
set -e
source bootstrap.sh

VFRB_VERSION=$(cat version.txt)
BOOST_ROOT=${BOOST_ROOT:-}
VFRB_ROOT=$PWD
export VFRB_COMPILER=${VFRB_COMPILER:-g++}
export VFRB_TARGET="vfrb-$VFRB_VERSION"
VFRB_EXEC_PATH=${VFRB_EXEC_PATH:-"$VFRB_ROOT/target/$VFRB_TARGET"}
VFRB_INI_PATH=${VFRB_INI_PATH:-"$VFRB_ROOT/target/vfrb.ini"}

for arg in $@; do
    case $arg in
    --path=*)
        VFRB_EXEC_PATH=${arg#*=:-"$VFRB_EXEC_PATH"}
        VFRB_INI_PATH=${arg#*=:-"$VFRB_INI_PATH"}
    ;;
    --ini-path=*)
        VFRB_INI_PATH=${arg#*=:-"$VFRB_INI_PATH"}
    ;;
    --build)
        DO_BUILD=1
    ;;
    --install)
        DO_INSTALL=1
    ;;
    --test)
        DO_TEST=1
    ;;
    -y | --confirm-yes)
        export AUTO_CONFIRM=1
    ;;
    -h | --help | *)
        echo TODO
    ;;
    esac
done

if [ "$(basename $VFRB_INI_PATH | grep -o '.ini')" == "" ]; then
    log -e "\"$VFRB_INI_PATH\"" is not a valid path to an ini file!
    exit 1
fi
prepare_path "$VFRB_EXEC_PATH"
prepare_path "$VFRB_INI_PATH" || true
export REPLACE_INI=$?
log -i VFRB executable will be at "$VFRB_EXEC_PATH"
log -i VFRB ini file will be at "$VFRB_INI_PATH"

if [ ! -z "$BOOST_ROOT" ]; then
    log -i Using custom boost: "$BOOST_ROOT"
    export BOOST_LIBS_L="-L${BOOST_ROOT}/stage/lib"
    export BOOST_ROOT_I="-I${BOOST_ROOT}"
    export CUSTOM_BOOST=1
fi

if [ ! -z "$DO_BUILD" ]; then
    install_deps
    build
fi

if [ ! -z "$DO_TEST" ]; then
    install_deps
    install_test_deps
    static_analysis
    build_test
    run_unit_test
    run_regression
    publish_coverage
fi

if [ ! -z "$DO_INSTALL" ]; then
    install
    install_service
fi

log -i FINISHED
