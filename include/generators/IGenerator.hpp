/**********************************************************
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
};
}  // namespace XMR