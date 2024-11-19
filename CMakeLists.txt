project(XMR)
include(GNUInstallDirs)
cmake_minimum_required(VERSION 3.25)

include(FetchContent)

FetchContent_Declare(
    XercesC
    GIT_REPOSITORY https://github.com/apache/xerces-c.git
    GIT_TAG 099150e21dc2ae3605ba3cb60454d56831874bb5
)

FetchContent_MakeAvailable(XercesC)

set(${BINARY_DIR} build/bin/)

FILE(GLOB SRCS src/*)
FILE(GLOB HEADERS include/*)
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC xerces-c)