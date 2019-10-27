cmake_minimum_required(VERSION 3.7.2)
project(VirtualFlightRadar-Backend LANGUAGES CXX)

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

#
# definitions
#

file(READ ${PROJECT_SOURCE_DIR}/version.txt TMP_VERSION)
string(STRIP "${TMP_VERSION}" CMAKE_PROJECT_VERSION)

if(NOT DEFINED VFRB_BIN_TAG)
    set(VFRB_BIN_TAG "${CMAKE_PROJECT_VERSION}-${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

set(vfrb_prod_bin vfrb-${VFRB_BIN_TAG})
set(vfrb_regression_bin vfrb_regression-${VFRB_BIN_TAG})
set(vfrb_test_bin vfrb_test-${VFRB_BIN_TAG})

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE vfrb_sources src/*.cpp)
file(GLOB_RECURSE vfrb_test_sources test/*.cpp)

set(CMAKE_BUILD_TYPE staged)
set(CMAKE_CXX_FLAGS_STAGED "-Wall -Wextra -Wpedantic")

if(DEFINED BOOST_STATIC)
    message(STATUS "Linking boost statically")
    set(Boost_USE_STATIC_LIBS ON)
endif()

#
# dependencies
#

find_package(Boost REQUIRED COMPONENTS regex system program_options)
find_package(Threads REQUIRED)

#
# target: release
#
add_executable(release ${vfrb_sources})
set_target_properties(release PROPERTIES OUTPUT_NAME ${vfrb_prod_bin})
target_compile_options(release PUBLIC -O2 -DNDEBUG -DVERSION=\"${CMAKE_PROJECT_VERSION}\")
target_include_directories(release PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(release PUBLIC Boost::regex Boost::system Boost::program_options Threads::Threads)

#
# target: regression
#
add_executable(regression ${vfrb_sources})
set_target_properties(regression PROPERTIES OUTPUT_NAME ${vfrb_regression_bin})
target_compile_options(regression PUBLIC -O0 -g --coverage -DLOG_ENABLE_DEBUG -DVERSION=\"${CMAKE_PROJECT_VERSION}\" -DAPRSCCLIENT_BEACON_INTERVAL=1 -DCLIENT_CONNECT_WAIT_TIMEVAL=2)
target_include_directories(regression PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(regression PUBLIC Boost::regex Boost::system Boost::program_options Threads::Threads gcov)

#
# target: test
#
list(REMOVE_ITEM vfrb_sources ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(APPEND vfrb_test_sources ${vfrb_sources})
add_executable(unittest ${vfrb_test_sources})
set_target_properties(unittest PROPERTIES OUTPUT_NAME ${vfrb_test_bin})
target_compile_options(unittest PUBLIC -O0 -g --coverage -fopenmp -DLOG_ENABLE_DEBUG -DSCTF_CUSTOM_EPSILON=0.000001)
target_include_directories(unittest PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test/include ${PROJECT_SOURCE_DIR}/test/framework)
target_link_libraries(unittest PUBLIC Boost::regex Boost::system Boost::program_options Threads::Threads gcov gomp)

#
# target: install
#
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
install(CODE "file(READ ${PROJECT_SOURCE_DIR}/vfrb.service.in service_file_data)\n
  string(REPLACE \"%VFRB_EXEC_PATH%\" \"${PROJECT_BINARY_DIR}/${vfrb_prod_bin}\" service_file_data \"\${service_file_data}\")\n
  string(REPLACE \"%VFRB_INI_PATH%\" \"${PROJECT_BINARY_DIR}/$ENV{VFRB_INI}\" service_file_data \"\${service_file_data}\")\n
  file(WRITE /etc/systemd/system/vfrb.service \"\${service_file_data}\")")
