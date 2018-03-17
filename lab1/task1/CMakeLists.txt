cmake_minimum_required(VERSION 3.10.2)
project (lab1_1)
file(GLOB SOURCES "./*.c")
add_library(carray SHARED ${SOURCES})
