add_library(parser ./parsers/PapyrusParser.cpp)
set_target_properties(parser PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules)