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
 * @filename: JavaGenerator.hpp
 * @brief:
 *
 ***********************************************************/

#pragma once
#include "IGenerator.hpp"

namespace XMR {

class JavaGenerator : public IGenerator {
 public:
  JavaGenerator() {}

  ~JavaGenerator() {}

  bool generate(std::ostream& os, ModelNode* root) final;
  bool check(ModelNode* root) final {
    //!@todo: Lucas work this
    checkCalled_ = true;
    return true;
  }

 private:
  bool checkCalled_ = false;
};
}  // namespace XMR