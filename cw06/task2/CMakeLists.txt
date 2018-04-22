cmake_minimum_required(VERSION 3.0)
project (lab6_1)
file(GLOB SOURCES "./src/*.c")

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(SERVER_SOURCES ${CMAKE_SOURCE_DIR}/server.c)
set(CLIENT_SOURCES ${CMAKE_SOURCE_DIR}/client.c)


#normal
add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})

target_link_libraries(server rt)
target_link_libraries(client rt)

set_property(TARGET server PROPERTY C_STANDARD 99)
set_property(TARGET client PROPERTY C_STANDARD 99)
