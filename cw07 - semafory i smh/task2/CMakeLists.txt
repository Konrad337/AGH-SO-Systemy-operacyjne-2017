cmake_minimum_required(VERSION 3.0)
project (lab6_1)
file(GLOB SOURCES "./src/*.c")

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(SERVER_SOURCES ${CMAKE_SOURCE_DIR}/barber.c)
set(CLIENT_SOURCES ${CMAKE_SOURCE_DIR}/clients.c)


#normal
add_executable(barber ${SERVER_SOURCES})
add_executable(clients ${CLIENT_SOURCES})

target_link_libraries(barber rt pthread)
target_link_libraries(clients rt pthread)

set_property(TARGET barber PROPERTY C_STANDARD 99)
set_property(TARGET clients PROPERTY C_STANDARD 99)
