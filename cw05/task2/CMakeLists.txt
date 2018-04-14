cmake_minimum_required(VERSION 3.0)
project (lab3_2)
file(GLOB SOURCES "./*.c")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_TEST_DIR ${CMAKE_SOURCE_DIR}/tests)
set(SLAVE_SOURCES ${CMAKE_SOURCE_DIR}/slave.c)
set(MASTER_SOURCES ${CMAKE_SOURCE_DIR}/master.c)


#normal
add_executable(master ${MASTER_SOURCES})
add_executable(slave ${SLAVE_SOURCES})


set_property(TARGET master PROPERTY C_STANDARD 99)
set_property(TARGET slave PROPERTY C_STANDARD 99)
