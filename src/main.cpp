/**********************************************************
 *
 * @filename: main.cpp
 * @brief:
 *
 ***********************************************************/
#include <ctype.h>
#include <dlfcn.h>
#include <unistd.h>

#include <iostream>

#include "parsers/PapyrusParser.hpp"

using namespace XMR;
using namespace xercesc;
using namespace std;

int main(int argc, char* argv[]) {
  // Below is the argument parser. Currently takes arg -f for filename
  char* file_name = NULL;
  char* library_path = NULL;
  int c;

  opterr = 0;
  while ((c = getopt(argc, argv, "f:l:")) !=
         -1)  // The last arg contains a list of valid arguments
  {
    switch (c) {
      case 'f':
        file_name = optarg;
        break;
      case 'l':
        library_path = optarg;
        break;
      case '?':
        if (optopt == 'f' || optopt == 'l') {
          cerr << "Option " << optopt << " requires an argument" << endl;
        } else if (isprint(optopt)) {
          cerr << "Unknown option. Usage: -f <filename>" << endl;
        } else {
          cerr << "Unkown character" << endl;
        }
        return 1;
      default:
        cerr << "Unspecified input error" << endl;
        abort();
    }
  }

  for (int index = optind; index < argc; ++index) {
    cout << "Non-option argument " << argv[index] << "\n" << endl;
  }
  if (file_name == NULL) {
    cerr << "Must specify an input file. Usage: -f <filename>" << endl;
    abort();
  }
  if (library_path == NULL) {
    cerr << "Must specify a library path. Usage: -l <library_path>" << endl;
    abort();
  }

  void* handle = dlopen(library_path, RTLD_LAZY);
  if (handle == NULL) {
    cerr << "Could not load library .so file" << endl;
    abort();
  }

  IParser* (*create)() = (IParser * (*)()) dlsym(handle, "create");
  if (create == NULL) {
    cerr << "Could not load parse object create method: " << dlerror() << endl;
    abort();
  }
  void (*destroy)(IParser*) = (void (*)(IParser*))dlsym(handle, "destroy");
  if (create == NULL) {
    cerr << "Could not load parse object delete method: " << dlerror() << endl;
    abort();
  }
  IParser* parser = (IParser*)create();

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
  destroy(parser);
  dlclose(handle);
  return 0;
}