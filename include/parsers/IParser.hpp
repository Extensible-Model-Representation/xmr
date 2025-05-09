/**********************************************************
 * Copyright 2025 Jason Cisneros & Lucas Van Der Heijden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @filename: IParser.hpp
 * @brief:
 *
 ***********************************************************/
#include <string>

#include "parsers/Node.hpp"

namespace XMR {
/**
 *
 */
class IParser {
 public:
  /**
   *
   */
  virtual ~IParser() = default;

  virtual ModelNode* parse() = 0;

  /**
   * Sets file to parse from
   * @param[in] fileName: Input file name for this parser to parse from
   * @returns true if file was able to be opened, false otherwise
   */
  virtual bool setInputFile(const char* fileName) = 0;
};

}  // namespace XMR