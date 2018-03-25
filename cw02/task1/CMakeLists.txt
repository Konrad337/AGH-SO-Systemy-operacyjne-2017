cmake_minimum_required(VERSION 2.8)
project (lab2_1)
file(GLOB SOURCES "./*.c")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_TEST_DIR ${CMAKE_SOURCE_DIR}/tests)


#system
add_executable(system ${SOURCES})
target_compile_definitions(system PUBLIC -DSYSTEM_FUNCTIONS)

#lib
add_executable(lib ${SOURCES})

file(MAKE_DIRECTORY ${CMAKE_TEST_DIR})

set(runSystemCommand "${CMAKE_BINARY_DIR}/system 1000 30 \
                      generate ${CMAKE_TEST_DIR}/testRandomFileSystem \
                      copy ${CMAKE_TEST_DIR}/testRandomFileSystem ${CMAKE_TEST_DIR}/testRandomFileSystem2 \
                      sort ${CMAKE_TEST_DIR}/testRandomFileSystem2")

set(runLibCommand "${CMAKE_BINARY_DIR}/lib 1000 30 \
                      generate ${CMAKE_TEST_DIR}/testRandomFileLib \
                      copy ${CMAKE_TEST_DIR}/testRandomFileLib ${CMAKE_TEST_DIR}/testRandomFileLib2 \
                      sort ${CMAKE_TEST_DIR}/testRandomFileLib2")


enable_testing()
add_test(NAME testSystem COMMAND sh -c
         "rm -f ${CMAKE_TEST_DIR}/systemResults.txt \
          && ${runSystemCommand} \
          >> ${CMAKE_TEST_DIR}/systemResults.txt")

add_test(NAME testLib COMMAND sh -c
         "rm -f ${CMAKE_TEST_DIR}/libResults.txt \
          && ${runLibCommand} \
          >> ${CMAKE_TEST_DIR}/libResults.txt")


set_property(TARGET system PROPERTY C_STANDARD 99)
set_property(TARGET lib PROPERTY C_STANDARD 99)
