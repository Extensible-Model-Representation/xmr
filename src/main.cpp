/**********************************************************
 *
 * @filename: main.cpp
 * @brief:
 *
 ***********************************************************/
#include <ctype.h>
#include <dlfcn.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "generators/IGenerator.hpp"
#include "parsers/IParser.hpp"

using namespace XMR;
using namespace std;

int main(int argc, char* argv[]) {
  // Below is the argument parser. Currently takes arg -f for filename
  char* file_name = NULL;
  char* parser_file = NULL;
  char* generator_file = NULL;
  char* out_file_name = NULL;
  int c;

  opterr = 0;
  while ((c = getopt(argc, argv, "f:p:g:o:")) !=
         -1)  // The last arg contains a list of valid arguments
  {
    switch (c) {
      case 'o':
        out_file_name = optarg;
        break;
      case 'f':
        file_name = optarg;
        break;
      case 'p':
        parser_file = optarg;
        break;
      case 'g':
        generator_file = optarg;
        break;
      case '?':
        if (optopt == 'f' || optopt == 'p' || optopt == 'g' || optopt == 'o') {
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
  if (parser_file == NULL) {
    std::cout << "Using default papyrus parser" << std::endl;
    parser_file = "./parsers/libpapyrus.so";
  }
  if (generator_file == NULL) {
    std::cout << "Using default cpp generator" << std::endl;
    generator_file = "./generators/libcpp.so";
  }
  if (out_file_name == NULL) {
    std::cout << "Default output file name to a.cpp" << std::endl;
    out_file_name = "a.cpp";
  }

  void* parser_handle = dlopen(parser_file, RTLD_LAZY);
  if (parser_handle == NULL) {
    cerr << "Could not load parser .so file. Error: " << dlerror() << endl;
    abort();
  }
  std::ofstream outputFile;
  outputFile.open(out_file_name);
  if (!outputFile.is_open()) {
    std::cerr << "Failed to create and open outputfile!";
    abort();
  }

  // Dynamically load the parser create and destroy methods
  IParser* (*parser_create)() =
      (IParser * (*)()) dlsym(parser_handle, "create_parser");
  if (parser_create == NULL) {
    cerr << "Could not load parse object create method: " << dlerror() << endl;
    abort();
  }
  void (*parser_destroy)(IParser*) =
      (void (*)(IParser*))dlsym(parser_handle, "destroy_parser");
  if (parser_destroy == NULL) {
    cerr << "Could not load parse object delete method: " << dlerror() << endl;
    abort();
  }
  IParser* parser = (IParser*)parser_create();  // create parser object

  // Parsing the document
  if (!parser->setInputFile(file_name)) {
    cerr << "Failed to set input file" << file_name << endl;
    parser_destroy(parser);
    dlclose(parser_handle);
    return -1;
  };
  ModelNode* root = parser->parse();
  parser_destroy(parser);
  dlclose(parser_handle);
  // Done parsing the document

  void* generator_handle = dlopen(generator_file, RTLD_LAZY);
  if (generator_handle == NULL) {
    cerr << "Could not load generator .so file. Error: " << dlerror() << endl;
    abort();
  }

  // Dynamically load the generator create and destroy methods
  IGenerator* (*generator_create)() =
      (IGenerator * (*)()) dlsym(generator_handle, "create_generator");
  if (generator_create == NULL) {
    cerr << "Could not load generator object create method: " << dlerror()
         << endl;
    abort();
  }
  void (*generator_destroy)(IGenerator*) =
      (void (*)(IGenerator*))dlsym(generator_handle, "destroy_generator");
  if (generator_destroy == NULL) {
    cerr << "Could not load generator object delete method: " << dlerror()
         << endl;
    abort();
  }
  IGenerator* generator =
      (IGenerator*)generator_create();  // create generator object

  if (root != nullptr) {
    generator->generate(outputFile, root);
  } else {
    cerr << "Root returned is null" << endl;
    generator_destroy(generator);
    dlclose(generator_handle);
    return -1;
  }
  generator_destroy(generator);
  dlclose(generator_handle);
  return 0;
}