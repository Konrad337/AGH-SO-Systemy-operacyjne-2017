cmake_minimum_required(VERSION 2.8)
project (lab1_2)
file(GLOB SOURCES "../../task2/*.c")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})




#static
set ( PROJECT_LINK_LIBS libcArrayStatic.a)
link_directories( ../../task1/bin)
add_executable(static ${SOURCES})
target_compile_definitions(static PUBLIC -DSTATIC)
target_link_libraries(static ${PROJECT_LINK_LIBS} )

#shared
set ( PROJECT_LINK_LIBS libcArrayShared.so)
link_directories( ../../task1/bin)
add_executable(shared ${SOURCES})
target_link_libraries(shared ${PROJECT_LINK_LIBS} )

#dynamic
SET(GCC_DYNAMIC_DLL_FLAG "-ldl")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_DYNAMIC_DLL_FLAG}")
add_executable(dynamic ${SOURCES})
target_compile_definitions(dynamic PUBLIC -DDYNAMIC)

execute_process (

    COMMAND ./bin/dynamic 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testDyn

)

execute_process (

    COMMAND ./bin/shared 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testShared

)

execute_process (

    COMMAND ./bin/static 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testStatic

)



file(WRITE "results3a.txt" "
Komentarz tak jak task2\n
./bin/dynamic 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testDyn}\n\n
./bin/shared 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testShared}\n\n
./bin/static 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testStatic}")

set_property(TARGET static PROPERTY C_STANDARD 99)
set_property(TARGET shared PROPERTY C_STANDARD 99)
set_property(TARGET dynamic PROPERTY C_STANDARD 99)
