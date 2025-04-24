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