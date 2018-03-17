cmake_minimum_required(VERSION 3.10.2)
project (lab1_1)
file(GLOB SOURCES "./*.c")

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#shared

add_library(cArrayShared SHARED ${SOURCES})

#static
add_definitions(-DSTATIC)
add_library(cArrayStatic STATIC ${SOURCES})
remove_definitions(-DSTATIC)


install(TARGETS cArrayShared cArrayStatic DESTINATION /home/konrad/Desktop/SO/lab1/task2/libs)
install(TARGETS cArrayShared DESTINATION /usr/lib)
