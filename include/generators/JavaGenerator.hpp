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
};
}  // namespace XMR