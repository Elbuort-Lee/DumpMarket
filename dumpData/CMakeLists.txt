cmake_minimum_required(VERSION 3.9)
project(dumpdata)

set(CMAKE_CXX_STANDARD 17)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Build/linux/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Build/linux/${CMAKE_BUILD_TYPE})

INCLUDE_DIRECTORIES(BEFORE .)
INCLUDE_DIRECTORIES(BEFORE ../Output/CRedis/include)
##INCLUDE_DIRECTORIES(BEFORE ../../trunk/libs/AtgoLogApi/include)
##INCLUDE_DIRECTORIES(BEFORE ../../trunk/libs/AtgoDtoApi/include)

LINK_DIRECTORIES(.)
LINK_DIRECTORIES(../Output/CRedis/linux)
##LINK_DIRECTORIES(../../trunk/libs/glog_0_3_5/lib/linux/x86_64)

add_compile_options(-fPIC)

IF (CMAKE_BUILD_TYPE MATCHES "Debug")
##    LINK_DIRECTORIES(../../trunk/libs/TDB/lib/linux/x86_x64/debug)
##	LINK_DIRECTORIES(../../trunk/libs/AtgoLogApi/lib/linux/x86_64/debug)
ELSE ()
##    LINK_DIRECTORIES(../../trunk/libs/TDB/lib/linux/x86_x64/release)
##	LINK_DIRECTORIES(../../trunk/libs/AtgoLogApi/lib/linux/x86_64/release)
ENDIF ()

aux_source_directory(. source_list)
add_executable(dumpdata ${source_list})
target_link_libraries(dumpdata dl pthread libCRedis.a -lhiredis)
add_custom_command(TARGET dumpdata POST_BUILD COMMAND cp ${PROJECT_BINARY_DIR}/Build/linux/${CMAKE_BUILD_TYPE}/* ${PROJECT_BINARY_DIR}/Output/)
