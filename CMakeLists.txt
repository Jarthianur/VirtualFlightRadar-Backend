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
set(vfrb_test_bin vfrb_test-${CMAKE_PROJECT_VERSION}-${CMAKE_HOST_SYSTEM_PROCESSOR})
set(THREADS_PREFER_PTHREAD_FLAG ON)

file(GLOB_RECURSE vfrb_sources src/*.cpp)
file(GLOB_RECURSE vfrb_test_sources test/*.cpp)

set(CMAKE_BUILD_TYPE staged)

#
# dependencies
#

find_package(Boost REQUIRED COMPONENTS regex system program_options)
find_package(Threads REQUIRED)

#
# target: release
#
set(CMAKE_CXX_FLAGS_STAGED "-O3 -DNDEBUG")
add_executable(release ${vfrb_sources})
set_target_properties(release PROPERTIES OUTPUT_NAME ${vfrb_prod_bin})
set_target_properties(release PROPERTIES COMPILE_FLAGS "-DVERSION=\\\"${CMAKE_PROJECT_VERSION}\\\"")
target_compile_features(release PUBLIC cxx_std_14)
target_include_directories(release PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(release PUBLIC Boost::regex Boost::system Boost::program_options Threads::Threads)

#
# target: test
#

list(REMOVE_ITEM vfrb_sources ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(APPEND vfrb_test_sources ${vfrb_sources})
add_executable(test ${vfrb_test_sources})
set_target_properties(test PROPERTIES OUTPUT_NAME ${vfrb_test_bin})
target_compile_features(test PUBLIC cxx_std_14)
target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test/include)
target_link_libraries(test PUBLIC Boost::regex Boost::system Boost::program_options Threads::Threads)
