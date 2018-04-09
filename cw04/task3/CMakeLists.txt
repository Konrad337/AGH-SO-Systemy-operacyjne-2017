cmake_minimum_required(VERSION 3.0)
project (lab3_1)
file(GLOB SOURCES "./*.c")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_TEST_DIR ${CMAKE_SOURCE_DIR}/tests)

#normal
add_executable(task ${SOURCES})


set_property(TARGET task PROPERTY C_STANDARD 99)
