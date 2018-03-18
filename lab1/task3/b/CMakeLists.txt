cmake_minimum_required(VERSION 3.10.2)
project (lab1_2)
file(GLOB SOURCES "../../task2/*.c")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})


SET( CMAKE_CXX_FLAGS  "-O0" )

#static
set ( PROJECT_LINK_LIBS libcArrayStatic.a)
link_directories( ./libs)
add_executable(staticO0 ${SOURCES})
target_compile_definitions(staticO0 PUBLIC -DSTATIC)
target_link_libraries(staticO0 ${PROJECT_LINK_LIBS} )

#shared
set ( PROJECT_LINK_LIBS libcArrayShared.so)
link_directories( ./libs)
add_executable(sharedO0 ${SOURCES})
target_link_libraries(sharedO0 ${PROJECT_LINK_LIBS} )

#dynamic
SET(GCC_DYNAMIC_DLL_FLAG "-ldl")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_DYNAMIC_DLL_FLAG}")
add_executable(dynamicO0 ${SOURCES})
target_compile_definitions(dynamicO0 PUBLIC -DDYNAMIC)

execute_process (

    COMMAND ./bin/dynamicO0 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testDynO0

)

execute_process (

    COMMAND ./bin/sharedO0 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testSharedO0

)

execute_process (

    COMMAND ./bin/staticO0 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testStaticO0

)

SET( CMAKE_CXX_FLAGS  "-O2" )

#static
set ( PROJECT_LINK_LIBS libcArrayStatic.a)
link_directories( ./libs)
add_executable(staticO2 ${SOURCES})
target_compile_definitions(staticO2 PUBLIC -DSTATIC)
target_link_libraries(staticO2 ${PROJECT_LINK_LIBS} )

#shared
set ( PROJECT_LINK_LIBS libcArrayShared.so)
link_directories( ./libs)
add_executable(sharedO2 ${SOURCES})
target_link_libraries(sharedO2 ${PROJECT_LINK_LIBS} )

#dynamic
SET(GCC_DYNAMIC_DLL_FLAG "-ldl")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_DYNAMIC_DLL_FLAG}")
add_executable(dynamicO2 ${SOURCES})
target_compile_definitions(dynamicO2 PUBLIC -DDYNAMIC)

execute_process (

    COMMAND ./bin/dynamicO2 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testDynO2

)

execute_process (

    COMMAND ./bin/sharedO2 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testSharedO2

)

execute_process (

    COMMAND ./bin/staticO2 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testStaticO2

)



SET( CMAKE_CXX_FLAGS  "-Os" )


#static
set ( PROJECT_LINK_LIBS libcArrayStatic.a)
link_directories( ./libs)
add_executable(staticOs ${SOURCES})
target_compile_definitions(staticOs PUBLIC -DSTATIC)
target_link_libraries(staticOs ${PROJECT_LINK_LIBS} )

#shared
set ( PROJECT_LINK_LIBS libcArrayShared.so)
link_directories( ./libs)
add_executable(sharedOs ${SOURCES})
target_link_libraries(sharedOs ${PROJECT_LINK_LIBS} )

#dynamic
SET(GCC_DYNAMIC_DLL_FLAG "-ldl")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_DYNAMIC_DLL_FLAG}")
add_executable(dynamicOs ${SOURCES})
target_compile_definitions(dynamicOs PUBLIC -DDYNAMIC)

execute_process (

    COMMAND ./bin/dynamicOs 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testDynOs

)

execute_process (

    COMMAND ./bin/sharedOs 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testSharedOs

)

execute_process (

    COMMAND ./bin/staticOs 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000
    OUTPUT_VARIABLE testStaticOs

)


file(WRITE "results3b.txt" "
    O0:\n\n
./bin/dynamicO0 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testDynO0}\n\n
./bin/sharedO0 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testSharedO0}\n\n
./bin/staticO0 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testStaticO0}\n\n
    O2:\n\n
./bin/dynamicO2 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testDynO2}\n\n
./bin/sharedO2 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testSharedO2}\n\n
./bin/staticO2 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testStaticO2}\n\n
    Os:\n\n
./bin/dynamicOs 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testDynOs}\n\n
./bin/sharedOs 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testSharedOs}\n\n
./bin/staticOs 1000 10 static create_table 10000 10 search_element 1000 add 10000 remove 10000 remove_blocks_and_add 10000\n
${testStaticOs}\n\n")
