cmake_minimum_required(VERSION 3.7.2)
project(VirtualFlightRadar-Backend LANGUAGES CXX)

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

#
# definitions
#

file(READ ${PROJECT_SOURCE_DIR}/version.txt TMP_VERSION)
string(STRIP "${TMP_VERSION}" CMAKE_PROJECT_VERSION)
set(vfrb_prod_bin vfrb-${CMAKE_PROJECT_VERSION}-${CMAKE_HOST_SYSTEM_PROCESSOR})
set(vfrb_regression_bin vfrb_regression-${CMAKE_PROJECT_VERSION}-${CMAKE_HOST_SYSTEM_PROCESSOR})
set(vfrb_test_bin vfrb_test-${CMAKE_PROJECT_VERSION}-${CMAKE_HOST_SYSTEM_PROCESSOR})
set(THREADS_PREFER_PTHREAD_FLAG ON)

file(GLOB_RECURSE vfrb_sources src/*.cpp)
file(GLOB_RECURSE vfrb_test_sources test/*.cpp)

set(CMAKE_BUILD_TYPE staged)

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
set(CMAKE_CXX_FLAGS_STAGED "-O3 -DNDEBUG -Wall -Wextra -Werror")
add_executable(release ${vfrb_sources})
set_target_properties(release PROPERTIES OUTPUT_NAME ${vfrb_prod_bin})
set_target_properties(release PROPERTIES COMPILE_FLAGS "-DVERSION=\\\"${CMAKE_PROJECT_VERSION}\\\"")
target_compile_features(release PUBLIC cxx_std_14)
target_include_directories(release PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(release PUBLIC Boost::regex Boost::system Boost::program_options Threads::Threads)

#
# target: regression
#
set(CMAKE_CXX_FLAGS_STAGED "-O0 -g -fprofile-arcs -ftest-coverage -coverage")
add_executable(regression ${vfrb_sources})
set_target_properties(regression PROPERTIES OUTPUT_NAME ${vfrb_regression_bin})
set_target_properties(regression PROPERTIES COMPILE_FLAGS "-DVERSION=\\\"${CMAKE_PROJECT_VERSION}\\\"")
target_compile_features(regression PUBLIC cxx_std_14)
target_include_directories(regression PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(regression PUBLIC Boost::regex Boost::system Boost::program_options Threads::Threads)

#
# target: test
#
set(CMAKE_CXX_FLAGS_STAGED "-O0 -g -fprofile-arcs -ftest-coverage -coverage -fopenmp -DSCTF_CUSTOM_EPSILON=0.000001")
list(REMOVE_ITEM vfrb_sources ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(APPEND vfrb_test_sources ${vfrb_sources})
add_executable(test ${vfrb_test_sources})
set_target_properties(test PROPERTIES OUTPUT_NAME ${vfrb_test_bin})
target_compile_features(test PUBLIC cxx_std_14)
target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test/include ${PROJECT_SOURCE_DIR}/test/framework)
target_link_libraries(test PUBLIC Boost::regex Boost::system Boost::program_options Threads::Threads)

#
# target: install
#
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
install(CODE "file(READ vfrb.service.in service_file_data)\n
  string(REPLACE \"%VFRB_EXEC_PATH%\" \"${PROJECT_BINARY_DIR}/${vfrb_prod_bin}\" service_file_data \"\${service_file_data}\")\n
  string(REPLACE \"%VFRB_INI_PATH%\" \"${PROJECT_BINARY_DIR}/$ENV{VFRB_INI}\" service_file_data \"\${service_file_data}\")\n
  file(WRITE /etc/systemd/system/vfrb.service \"\${service_file_data}\")")
