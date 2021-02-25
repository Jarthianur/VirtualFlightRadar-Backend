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

set(vfrb_release_bin vfrb-${VFRB_BIN_TAG})
set(vfrb_test_bin vfrb_test-${VFRB_BIN_TAG})
set(vfrb_prof_bin profiling-${VFRB_BIN_TAG})

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

file(GLOB_RECURSE vfrb_sources src/*.cpp)
file(GLOB vfrb_test_sources test/*.cpp)
file(GLOB vfrb_test_impl_sources test/impl/*.cpp)
file(GLOB_RECURSE vfrb_prof_sources profiling/*.cpp)
set(vfrb_sources_for_test "")
list(APPEND vfrb_sources_for_test ${vfrb_sources})
list(APPEND vfrb_sources_for_test ${vfrb_test_impl_sources})
list(REMOVE_ITEM vfrb_sources_for_test ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM vfrb_sources_for_test ${PROJECT_SOURCE_DIR}/src/CVfrb.cpp)
list(REMOVE_ITEM vfrb_sources_for_test ${PROJECT_SOURCE_DIR}/src/object/impl/CDateTimeAsio.cpp)
list(REMOVE_ITEM vfrb_sources_for_test ${PROJECT_SOURCE_DIR}/src/server/net/impl/CAcceptorAsio.cpp)
list(REMOVE_ITEM vfrb_sources_for_test ${PROJECT_SOURCE_DIR}/src/server/net/impl/CSocketAsio.cpp)

set(CMAKE_BUILD_TYPE staged)
set(VFRB_CXX_W_FLAGS "-Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Werror")
set(VFRB_CXX_WNOERR_FLAGS "-Wno-error=attributes -Wno-error=unused-result -Wno-error=unused-value")
set(CMAKE_CXX_FLAGS_STAGED "${VFRB_CXX_W_FLAG} ${VFRB_CXX_WNOERR_FLAGS} -DBOOST_SPIRIT_THREADSAFE -DASIO_HAS_BOOST_DATE_TIME -pipe")
set(VFRB_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test/include ${PROJECT_SOURCE_DIR}/profiling/include)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_STAGED "${CMAKE_CXX_FLAGS_STAGED} -Wthread-safety -Wthread-safety-reference -Wthread-safety-negative")
endif()

set(Boost_USE_STATIC_LIBS ON)
set(VFRB_LINK_LIBS Threads::Threads fmt::fmt-header-only)
set(VFRB_TEST_LINK_LIBS gcov gomp tpp)

#
# dependencies
#
add_subdirectory(vendor/fmt EXCLUDE_FROM_ALL)
add_subdirectory(vendor/TestPlusPlus EXCLUDE_FROM_ALL)

find_package(Threads REQUIRED)

set(VFRB_INCLUDE_DIRS ${VFRB_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/vendor/boost)
set(VFRB_INCLUDE_DIRS ${VFRB_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/vendor/asio/asio/include)

#
# target: release
#
add_executable(release ${vfrb_sources})
target_compile_options(release PUBLIC -O2 -DNDEBUG -DVERSION=\"${CMAKE_PROJECT_VERSION}\")
set_target_properties(release PROPERTIES OUTPUT_NAME ${vfrb_release_bin})
target_include_directories(release PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(release PUBLIC ${VFRB_LINK_LIBS})

#
# target: unittest
#
add_executable(unittest ${vfrb_test_sources} ${vfrb_sources_for_test})
target_compile_options(unittest PUBLIC -O0 -g --coverage -fopenmp -DLOG_ENABLE_DEBUG)
set_target_properties(unittest PROPERTIES OUTPUT_NAME ${vfrb_test_bin})
target_include_directories(unittest PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(unittest PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

#
# targets for single tests
#
add_executable(util.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestUtil.cpp ${vfrb_sources_for_test})
target_compile_options(util.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(util.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(util.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(string.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestString.cpp ${vfrb_sources_for_test})
target_compile_options(string.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(string.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(string.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(server.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestServer.cpp ${vfrb_sources_for_test})
target_compile_options(server.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(server.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(server.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(object.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestObject.cpp ${vfrb_sources_for_test})
target_compile_options(object.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(object.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(object.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(math.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestMath.cpp ${vfrb_sources_for_test})
target_compile_options(math.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(math.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(math.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(parser.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestFeedParser.cpp ${vfrb_sources_for_test})
target_compile_options(parser.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(parser.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(parser.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(feed.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestFeed.cpp ${vfrb_sources_for_test})
target_compile_options(feed.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(feed.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(feed.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(processor.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestDataProcessor.cpp ${vfrb_sources_for_test})
target_compile_options(processor.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(processor.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(processor.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(data.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestData.cpp ${vfrb_sources_for_test})
target_compile_options(data.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(data.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(data.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(config.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestConfig.cpp ${vfrb_sources_for_test})
target_compile_options(config.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(config.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(config.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

add_executable(client.test ${PROJECT_SOURCE_DIR}/test/UnitTests.cpp ${PROJECT_SOURCE_DIR}/test/TestClient.cpp ${vfrb_sources_for_test})
target_compile_options(client.test PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG)
target_include_directories(client.test PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(client.test PUBLIC ${VFRB_LINK_LIBS} ${VFRB_TEST_LINK_LIBS})

#
# target: profiling
#
add_executable(profiling ${vfrb_prof_sources})
target_compile_options(profiling PUBLIC -O2 -DNDEBUG)
set_target_properties(profiling PROPERTIES OUTPUT_NAME ${vfrb_prof_bin})
target_include_directories(profiling PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(profiling PUBLIC ${VFRB_LINK_LIBS})

#
# target: install
#
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
install(CODE "file(READ ${PROJECT_SOURCE_DIR}/vfrb.service.in service_file_data)\n
  string(REPLACE \"%VFRB_EXEC_PATH%\" \"${PROJECT_BINARY_DIR}/${vfrb_release_bin}\" service_file_data \"\${service_file_data}\")\n
  string(REPLACE \"%VFRB_CONF_PATH%\" \"${PROJECT_BINARY_DIR}/$ENV{VFRB_CONF}\" service_file_data \"\${service_file_data}\")\n
  file(WRITE /etc/systemd/system/vfrb.service \"\${service_file_data}\")")

#
# compiledb dummy target
#
add_executable(compiledb_dummy EXCLUDE_FROM_ALL ${vfrb_test_sources} ${vfrb_sources_for_test})
target_compile_options(compiledb_dummy PUBLIC -O0 -g -fopenmp -DLOG_ENABLE_DEBUG -Wthread-safety -Wthread-safety-reference -Wthread-safety-negative)
target_include_directories(compiledb_dummy PUBLIC ${VFRB_INCLUDE_DIRS})
target_link_libraries(compiledb_dummy PUBLIC ${VFRB_LINK_LIBS} gomp tpp)

if("${GENERATE_COMPILEDB}")
  if(NOT "${COMPILEDB_TARGET}")
    set(COMPILEDB_TARGET compiledb_dummy)
  endif()
  message(STATUS "Run cmake twice to generate/update compile_commands.json (workaround).")
  execute_process(COMMAND compiledb --command-style -n make -C ${CMAKE_BINARY_DIR} ${COMPILEDB_TARGET})
endif()
