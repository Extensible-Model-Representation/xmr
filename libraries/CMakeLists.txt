##########################################################
# Copyright 2025 Jason Cisneros & Lucas Van Der Heijden
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##########################################################

# Include the xerces paths in deps folder
set(XERCESC_INCLUDE ${CMAKE_SOURCE_DIR}/build/_deps/xerces-c-src/src ${CMAKE_SOURCE_DIR}/build/_deps/xerces-c-build/src)

# Find and compile all the parser files into .so files
FILE(GLOB_RECURSE PARSERS ${CMAKE_CURRENT_LIST_DIR}/parsers/#.cpp)
foreach(SRC ${PARSERS})
    get_filename_component(PARSERNAME ${SRC} NAME_WE)
    add_library(${PARSERNAME} SHARED ${SRC})
    target_include_directories(${PARSERNAME} PUBLIC ${XERCESC_INCLUDE} ${CMAKE_CURRENT_LIST_DIR}/../include/)
    set_target_properties(${PARSERNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/parsers/)
    set_target_properties(${PARSERNAME} PROPERTIES OUTPUT_NAME ${PARSERNAME})
endforeach()

# Find and compile all the generator files into .so files
FILE(GLOB_RECURSE GENERATORS ${CMAKE_CURRENT_LIST_DIR}/generators/#.cpp)
foreach(SRC ${GENERATORS})
    get_filename_component(GENERATORNAME ${SRC} NAME_WE)
    add_library(${GENERATORNAME} SHARED ${SRC})
    target_include_directories(${GENERATORNAME} PUBLIC ${XERCESC_INCLUDE} ${CMAKE_CURRENT_LIST_DIR}/../include/)
    set_target_properties(${GENERATORNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/generators/)
    set_target_properties(${GENERATORNAME} PROPERTIES OUTPUT_NAME ${GENERATORNAME})
endforeach()