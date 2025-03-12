# Find and compile all the parser files into .so files
FILE(GLOB_RECURSE PARSERS ${CMAKE_CURRENT_LIST_DIR}/parsers/*.cpp)
foreach(SRC ${PARSERS})
    get_filename_component(PARSERNAME ${SRC} NAME_WE)
    add_library(${PARSERNAME} SHARED ${SRC})
    set_target_properties(${PARSERNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/parsers/)
    set_target_properties(${PARSERNAME} PROPERTIES OUTPUT_NAME ${PARSERNAME})
endforeach()

# Find and compile all the generator files into .so files
FILE(GLOB_RECURSE GENERATORS ${CMAKE_CURRENT_LIST_DIR}/generators/*.cpp)
foreach(SRC ${GENERATORS})
    get_filename_component(GENERATORNAME ${SRC} NAME_WE)
    add_library(${GENERATORNAME} SHARED ${SRC})
    set_target_properties(${GENERATORNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/generators/)
    set_target_properties(${GENERATORNAME} PROPERTIES OUTPUT_NAME ${GENERATORNAME})
endforeach()