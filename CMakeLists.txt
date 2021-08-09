cmake_minimum_required(VERSION 3.1)

project (STAMP)

include_directories(${CMAKE_SOURCE_DIR}/utils)

aux_source_directory(utils UTILS)

set(SOURCES temp_test.cc ${UTILS})

add_executable(STAMP ${SOURCES})

