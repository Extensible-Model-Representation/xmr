/**********************************************************
 *
 * @filename: CPPGenerator.cpp
 * @brief:
 *
 ***********************************************************/
#include <generators/IGenerator.hpp>
namespace XMR {

class CPPGenerator : public IGenerator {
 public:
  CPPGenerator() {}

  ~CPPGenerator() {}

  bool generate(std::ostream& os, ModelNode* root) final;
};
}  // namespace XMR