cmake_minimum_required(VERSION 3.25)
project(XMR VERSION 0.0.0.1 LANGUAGES CXX)
include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include(FetchContent)

FetchContent_Declare(
    xerces-c
    GIT_REPOSITORY https://github.com/apache/xerces-c.git
    GIT_TAG 099150e21dc2ae3605ba3cb60454d56831874bb5
    FIND_PACKAGE_ARGS NAMES XercesC
)

FetchContent_MakeAvailable(xerces-c)

#FetchContent_Declare(
#    googletest
#    GIT_REPOSITORY https://github.com/google/googletest.git
#    GIT_TAG e39786088138f2749d64e9e90e0f9902daa77c40
#    FIND_PACKAGE_ARGS NAMES GTest
#)

#FetchContent_MakeAvailable(googletest)

set(${BINARY_DIR} build/bin/)
set(XERCESC_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/build/_deps/xerces-c-src/src ${CMAKE_CURRENT_LIST_DIR}/build/_deps/xerces-c-build/src)

FILE(GLOB_RECURSE SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
message("${SRCS}")
FILE(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp)
FILE(GLOB_RECURSE PARSER_FILES ${CMAKE_CURRENT_LIST_DIR}/libraries/parsers/*.cpp)
FILE(GLOB_RECURSE GENERATOR_FILES ${CMAKE_CURRENT_LIST_DIR}/libraries/generators/*.cpp)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS} ${PARSER_FILES} ${GENERATOR_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC xerces-c)
target_include_directories(${PROJECT_NAME} PUBLIC ${XERCESC_INCLUDE})
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libraries)
target_link_libraries(${PROJECT_NAME} PUBLIC papyrus)
target_link_libraries(${PROJECT_NAME} PUBLIC cpp)