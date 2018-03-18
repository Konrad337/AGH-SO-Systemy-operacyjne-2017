cmake_minimum_required(VERSION 3.10.2)
project (lab1_2)
file(GLOB SOURCES "./*.c")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#static
add_definitions(-DSTATIC)
set ( PROJECT_LINK_LIBS libcArrayStatic.a)
link_directories( ./libs)
add_executable(static ${SOURCES})
target_link_libraries(static ${PROJECT_LINK_LIBS} )
remove_definitions(-DSTATIC)

#shared
set ( PROJECT_LINK_LIBS libcArrayShared.so)
link_directories( ./libs)
add_executable(shared  ${SOURCES})
target_link_libraries(shared ${PROJECT_LINK_LIBS} )

#dynamic
add_definitions(-DDYNAMIC)
set ( PROJECT_LINK_LIBS libcArrayShared.so)
link_directories( ./libs)
add_executable(shared  ${SOURCES})
target_link_libraries(shared ${PROJECT_LINK_LIBS} )
remove_definitions(-DDYNAMIC)
