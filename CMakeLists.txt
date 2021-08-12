cmake_minimum_required(VERSION 3.1)

project (STAMP)

include_directories(${CMAKE_SOURCE_DIR}/utils ${CMAKE_SOURCE_DIR}/protocols)

aux_source_directory(utils UTILS)
aux_source_directory(protocols PROTOCOL)

set(SOURCES ${UTILS} ${PROTOCOL})

add_executable(stamp-client stamp-client/stamp-client.cc ${SOURCES})
add_executable(stamp-server stamp-server/stamp-server.cc ${SOURCES})

add_executable(temp_test temp_test.cc ${SOURCES})
