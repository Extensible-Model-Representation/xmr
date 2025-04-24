/**********************************************************
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