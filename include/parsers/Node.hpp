/**********************************************************
 *
 * @filename: Node.hpp
 * @brief:
 *
 ***********************************************************/

#include <cstddef>
#include <ostream>

namespace XMR {

// Node class that all XMR generateble elements inherit from
class Node {
 public:
  /**
   * Calls this node's ostream operator to stream out the node's code
   * representation to the passed in ostream
   * @param[out] os: modified ostream with the node code gen
   */
  void generate(std::ostream& os) { os << this; }

 protected:
   Node() = default;
  ~Node() = default;
};
}  // namespace XMR