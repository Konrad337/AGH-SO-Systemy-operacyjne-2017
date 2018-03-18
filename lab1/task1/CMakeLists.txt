cmake_minimum_required(VERSION 3.10.2)
project (lab1_1)
file(GLOB SOURCES "./*.c")

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#shared
add_library(cArrayShared SHARED ${SOURCES})
install(TARGETS cArrayShared DESTINATION /home/konrad/Desktop/SO/lab1/task2/libs)
install(TARGETS cArrayShared DESTINATION /home/konrad/Desktop/SO/lab1/task3/a/libs)

#static
add_library(cArrayStatic STATIC ${SOURCES})
target_compile_definitions(cArrayStatic PUBLIC -DSTATIC)
install(TARGETS cArrayStatic DESTINATION /home/konrad/Desktop/SO/lab1/task2/libs)
install(TARGETS cArrayStatic DESTINATION /home/konrad/Desktop/SO/lab1/task3/a/libs)
