cmake_minimum_required(VERSION 3.0)
project (lab6_1)
file(GLOB SOURCES "./src/*.c")

set(CMAKE_TEST_DIR ${CMAKE_SOURCE_DIR}/tests)

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(SERVER_SOURCES ${CMAKE_SOURCE_DIR}/task.c)


#normal
add_executable(task ${SERVER_SOURCES})
target_link_libraries(task m pthread)


set_property(TARGET task PROPERTY C_STANDARD 99)
