cmake_minimum_required(VERSION 3.10.2)
project (lab1_2)
file(GLOB SOURCES "../../task2/*.c")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})




#static
set ( PROJECT_LINK_LIBS libcArrayStatic.a)
link_directories( ./libs)
add_executable(static ${SOURCES})
target_compile_definitions(static PUBLIC -DSTATIC)
target_link_libraries(static ${PROJECT_LINK_LIBS} )

#shared
set ( PROJECT_LINK_LIBS libcArrayShared.so)
link_directories( ./libs)
add_executable(shared ${SOURCES})
target_link_libraries(shared ${PROJECT_LINK_LIBS} )

#dynamic

SET(GCC_DYNAMIC_DLL_FLAG "-ldl")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_DYNAMIC_DLL_FLAG}")
add_executable(dynamic ${SOURCES})
target_compile_definitions(dynamic PUBLIC -DDYNAMIC)
