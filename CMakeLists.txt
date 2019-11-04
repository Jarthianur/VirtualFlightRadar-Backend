cmake_minimum_required(VERSION 3.7.2)
project(VirtualFlightRadar-Backend LANGUAGES CXX)

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

#
#definitions
#

file(READ ${PROJECT_SOURCE_DIR}/version.txt TMP_VERSION)
string(STRIP "${TMP_VERSION}" CMAKE_PROJECT_VERSION)

if(NOT DEFINED VFRB_BIN_TAG)
    set(VFRB_BIN_TAG "${CMAKE_PROJECT_VERSION}-${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

set(vfrb_release_bin vfrb-${VFRB_BIN_TAG})
set(vfrb_regression_bin vfrb_regression-${VFRB_BIN_TAG})
set(vfrb_test_bin vfrb_test-${VFRB_BIN_TAG})
set(vfrb_prof_bin profiling-${VFRB_BIN_TAG})

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE vfrb_sources src/*.cpp)
file(GLOB_RECURSE vfrb_test_sources test/*.cpp)
file(GLOB_RECURSE vfrb_prof_sources profiling/*.cpp)
list(APPEND vfrb_test_sources ${vfrb_sources})
list(REMOVE_ITEM vfrb_test_sources ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM vfrb_test_sources ${PROJECT_SOURCE_DIR}/src/object/impl/DateTimeImplBoost.cpp)

set(CMAKE_BUILD_TYPE staged)
set(CMAKE_CXX_FLAGS_STAGED "-Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Werror -Wno-error=attributes")

if(DEFINED BOOST_STATIC)
    message(STATUS "Linking boost statically")
    set(Boost_USE_STATIC_LIBS ON)
endif()

set(VFRB_LINK_LIBS Boost::system Boost::program_options Threads::Threads)

#
# dependencies
#
find_package(Boost REQUIRED COMPONENTS system program_options)
find_package(Threads REQUIRED)

#
# target: release
#
add_executable(release ${vfrb_sources})
target_compile_options(release PUBLIC -O2 -DNDEBUG -DVERSION=\"${CMAKE_PROJECT_VERSION}\")
set_target_properties(release PROPERTIES OUTPUT_NAME ${vfrb_release_bin})
target_include_directories(release PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(release PUBLIC ${VFRB_LINK_LIBS})

#
# target: regression
#
add_executable(regression ${vfrb_sources})
target_compile_options(regression PUBLIC -O0 -g --coverage -DLOG_ENABLE_DEBUG -DVERSION=\"${CMAKE_PROJECT_VERSION}\")
set_target_properties(regression PROPERTIES OUTPUT_NAME ${vfrb_regression_bin})
target_include_directories(regression PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(regression PUBLIC ${VFRB_LINK_LIBS} gcov)

#
# target: test
#
add_executable(unittest ${vfrb_test_sources})
target_compile_options(unittest PUBLIC -O0 -g --coverage -fopenmp -DLOG_ENABLE_DEBUG -DSCTF_CUSTOM_EPSILON=0.000001)
set_target_properties(unittest PROPERTIES OUTPUT_NAME ${vfrb_test_bin})
target_include_directories(unittest PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test/include)
target_link_libraries(unittest PUBLIC ${VFRB_LINK_LIBS} gcov gomp)

#
# target: profiling
#
add_executable(profiling ${vfrb_prof_sources})
target_compile_options(profiling PUBLIC -O2 -DNDEBUG)
set_target_properties(profiling PROPERTIES OUTPUT_NAME ${vfrb_prof_bin})
target_include_directories(profiling PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/profiling/include)
target_link_libraries(profiling PUBLIC ${VFRB_LINK_LIBS})

#
# target: install
#
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
install(CODE "file(READ ${PROJECT_SOURCE_DIR}/vfrb.service.in service_file_data)\n
  string(REPLACE \"%VFRB_EXEC_PATH%\" \"${PROJECT_BINARY_DIR}/${vfrb_release_bin}\" service_file_data \"\${service_file_data}\")\n
  string(REPLACE \"%VFRB_INI_PATH%\" \"${PROJECT_BINARY_DIR}/$ENV{VFRB_INI}\" service_file_data \"\${service_file_data}\")\n
  file(WRITE /etc/systemd/system/vfrb.service \"\${service_file_data}\")")
