/**********************************************************
 *
 * @filename: CPPGenerator.cpp
 * @brief:
 *
 ***********************************************************/

#include "generators/CPPGenerator.hpp"

using namespace std;
namespace XMR {

bool generateModule(std::ostream& os, ModuleNode* module) {
  os << "class " << module->name_ << " {" << endl;

  os << "}; // class " << module->name_ << endl << endl;
}

bool generatePackage(ostream& os, Package* package) {
  bool result = true;
  os << "namespace " << package->name_ << "{" << endl << endl;

  for (size_t i = 0; i < package->modules_.size(); i++) {
    result = generateModule(os, package->modules_[i]) && result;
  }

  os << "} // namespace " << package->name_ << endl << endl;
}

bool CPPGenerator::generate(std::ostream& os, ModelNode* root) {
  bool result = true;
  char* modelName = root->name_;

  os << "namespace " << modelName << "{" << endl << endl;

  for (size_t i = 0; i < root->modules_.size(); i++) {
    result = generateModule(os, root->modules_[i]) && result;
    os << endl << endl;
  }

  for (size_t i = 0; i < root->packages_.size(); i++) {
    result = generatePackage(os, root->packages_[i]) && result;
    os << endl << endl;
  }

  os << "} // namespace " << modelName << endl << endl;

  os << "int main(int argc, char* argv[]) {" << endl << endl;
  os << "return 0;" << endl;
  os << "}" << endl;
}
extern "C" IGenerator* create_generator() { return new CPPGenerator; }
extern "C" void destroy_generator(IGenerator* generator) { delete generator; }
}  // namespace XMR