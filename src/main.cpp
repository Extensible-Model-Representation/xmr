/**********************************************************
 *
 * @filename: main.cpp
 * @brief:
 *
 ***********************************************************/
#include <ctype.h>
#include <unistd.h>

#include <iostream>

#include "parsers/PapyrusParser.hpp"

using namespace XMR;
using namespace xercesc;
using namespace std;

int main(int argc, char* argv[]) {
  // Below is the argument parser. Currently takes arg -f for filename
  char* file_name = NULL;
  int c;

  opterr = 0;
  while ((c = getopt(argc, argv, "f:")) !=
         -1)  // The last arg contains a list of valid arguments
  {
    switch (c) {
      case 'f':
        file_name = optarg;
        break;
      case '?':
        if (optopt == 'f') {
          std::cerr << "Option -f requires an argument" << std::endl;
        } else if (isprint(optopt)) {
          std::cerr << "Unknown option. Usage: -f <filename>" << std::endl;
        } else {
          std::cerr << "Unkown character" << std::endl;
        }
        return 1;
      default:
        std::cerr << "Unspecified input error" << std::endl;
        abort();
    }
  }

  for (int index = optind; index < argc; ++index) {
    std::cout << "Non-option argument " << argv[index] << "\n" << std::endl;
  }
  if (file_name == NULL) {
    std::cerr << "Must specify an input file. Usage: -f <filename>"
              << std::endl;
    abort();
  }

  PapyrusParser* parser = new PapyrusParser();
  if (!parser->setInputFile(file_name)) {
    cerr << "Failed to set input file" << file_name << endl;
    delete parser;
    return -1;
  };

  ModelNode* root = parser->parse();
  if (root != nullptr)
    root->generate(cout);

  else {
    cerr << "Root returned is null" << endl;
    delete parser;
    return -1;
  }
  return 0;
}