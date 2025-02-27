/**********************************************************
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

  virtual Node* parse() = 0;

  /**
   * Sets file to parse from
   * @param[in] fileName: Input file name for this parser to parse from
   * @returns true if file was able to be opened, false otherwise
   */
  virtual bool setInputFile(char* fileName) = 0;
};

}  // namespace XMR