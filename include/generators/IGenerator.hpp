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
 * @filename: IGenerator.cpp
 * @brief:
 *
 ***********************************************************/
#include <ostream>
#include <parsers/Node.hpp>

namespace XMR {
class IGenerator {
 public:
  /**
   *
   */
  virtual ~IGenerator() = default;

  virtual bool generate(std::ostream& os, ModelNode* root) = 0;

  /**
   * Checks if the model is generateble for the implementing IGenerator. This
   * methods preprocesses the tree as needed. After this method returns true
   * the same implementing IGenerator *should* be able to generate valued source
   * code with a subsequent call to generate. If check hasn't already been called
   * generate will call it.
   */
  virtual bool check(ModelNode* root) = 0;
};
}  // namespace XMR