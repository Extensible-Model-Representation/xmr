project(XMR)
include(GNUInstallDirs)
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

include(FetchContent)

FetchContent_Declare(
    xerces-c
    GIT_REPOSITORY https://github.com/apache/xerces-c.git
    GIT_TAG 099150e21dc2ae3605ba3cb60454d56831874bb5
    FIND_PACKAGE_ARGS NAMES XercesC
)

FetchContent_MakeAvailable(xerces-c)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG e39786088138f2749d64e9e90e0f9902daa77c40
    FIND_PACKAGE_ARGS NAMES GTest
)

FetchContent_MakeAvailable(googletest)

set(${BINARY_DIR} build/bin/)

FILE(GLOB SRCS src/*)
FILE(GLOB HEADERS include/*)

include_directories(include)
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} PUBLIC xerces-c)