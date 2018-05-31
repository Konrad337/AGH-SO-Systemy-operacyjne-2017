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


file(MAKE_DIRECTORY ${CMAKE_TEST_DIR})

set(run1 "${CMAKE_BINARY_DIR}/task 1 ./data/pic1 ./data/filtr1 out_file")

set(run4 "${CMAKE_BINARY_DIR}/task 4 ./data/pic1 ./data/filtr1 out_file")

set(run8 "${CMAKE_BINARY_DIR}/task 8 ./data/pic1 ./data/filtr1 out_file")

set(run2 "${CMAKE_BINARY_DIR}/task 2 ./data/pic1 ./data/filtr1 out_file")

enable_testing()
add_test(NAME test COMMAND sh -c
         "rm -f ${CMAKE_TEST_DIR}/1.txt \
          && ${run1} \
      >> ${CMAKE_TEST_DIR}/1.txt")

add_test(NAME test2 COMMAND sh -c
       "rm -f ${CMAKE_TEST_DIR}/2.txt \
        && ${run2} \
    >> ${CMAKE_TEST_DIR}/2.txt")

add_test(NAME test4 COMMAND sh -c
         "rm -f ${CMAKE_TEST_DIR}/4.txt \
          && ${run4} \
      >> ${CMAKE_TEST_DIR}/4.txt")

add_test(NAME test8 COMMAND sh -c
       "rm -f ${CMAKE_TEST_DIR}/8.txt \
        && ${run8} \
    >> ${CMAKE_TEST_DIR}/8.txt")
