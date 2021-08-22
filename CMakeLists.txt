cmake_minimum_required(VERSION 3.1)

project (STAMP)

include_directories(${CMAKE_SOURCE_DIR}/stamp-library)

aux_source_directory(stamp-library STAMP)
aux_source_directory(stamp-client CLIENT)
aux_source_directory(stamp-server SERVER)

add_library(stamp ${STAMP})

add_executable(stamp-client ${CLIENT})
add_dependencies(stamp-client stamp)
target_link_libraries(stamp-client stamp)

add_executable(stamp-server ${SERVER})
add_dependencies(stamp-server stamp)
target_link_libraries(stamp-server stamp)
