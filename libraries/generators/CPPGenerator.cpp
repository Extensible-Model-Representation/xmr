/**********************************************************
 *
 * @filename: CPPGenerator.cpp
 * @brief:
 *
 ***********************************************************/
#include "generators/CPPGenerator.hpp"

#include <cstring>
#include <unordered_map>

using namespace std;

static unordered_map<char*, bool> generatedSymbols;
static unordered_map<std::string, std::vector<std::string>> idNameMap;
static unordered_set<std::string> noNoNames = {"delete", "new"};
namespace XMR {

bool checkOperatorName(char* name) {
  // lookup no no phrased for c++ operator names, i.e. new delete
  if (noNoNames.contains(name)) {
    cerr << "Error: C++ operator name: " << name << " reserved operator"
         << endl;
    return false;
  }

  return true;
}
bool generateOperator(std::ostream& os, Operator* op) {
  if (checkOperatorName(op->name_)) {
    if (op->returnType_) {
      if (op->returnType_->isPrimitive_) {
        std::string hash = "#";
        size_t index = strcspn(op->returnType_->type_, hash.c_str());
        std::string type = std::to_string(op->returnType_->type_[index]);
        if (type == "Boolean") {
          os << "bool ";
        } else if (type == "Real") {
          os << "double ";
        } else {
          os << "int ";
        }

      } else {
        // lookup type name of id
        string qualifiedPath;
        for (auto& subPath : idNameMap[op->returnType_->type_]) {
          qualifiedPath = qualifiedPath + "::" + subPath;
        }
        os << qualifiedPath;
        if (!generatedSymbols[op->returnType_->type_]) {
          // inject a pointer as usage of incomplete type in class def not
          // permissible in C++
          os << "*";
        }
      }
    } else {
      os << "void";
    }

    os << " " << op->name_ << "(";

    if (!op->params_.empty()) {
      for (size_t i = 0; i < op->params_.size() - 1; i++) {
        if (op->params_[i]->type_->isPrimitive_) {
          std::string hash = "#";
          size_t index = strcspn(op->params_[i]->type_->type_, hash.c_str());
          std::string type =
              std::to_string(op->params_[i]->type_->type_[index]);
          if (type == "Boolean") {
            os << "bool ";
          } else if (type == "Real") {
            os << "double ";
          } else {
            os << "int ";
          }

        } else {
          // lookup type name of id
          string qualifiedName;
          for (auto& subPath : idNameMap[op->params_[i]->type_->type_]) {
            qualifiedName = qualifiedName + "::" + subPath;
          }
          os << qualifiedName;
          if (!generatedSymbols[op->params_[i]->type_->type_]) {
            // inject a pointer as usage of incomplete type in class def not
            // permissible in C++
            //!@todo: this feels icky
            os << "*";
          }
        }
        os << " " << op->params_[i]->name_ << ", ";
      }

      if (op->params_[op->params_.size() - 1]->type_->isPrimitive_) {
        std::string hash = "#";
        size_t index = strcspn(
            op->params_[op->params_.size() - 1]->type_->type_, hash.c_str());
        std::string type = std::to_string(
            op->params_[op->params_.size() - 1]->type_->type_[index]);
        if (type == "Boolean") {
          os << "bool ";
        } else if (type == "Real") {
          os << "double ";
        } else {
          os << "int ";
        }

      } else {
        // lookup type name of id
        std::string qualifiedName;
        for (auto& subPath :
             idNameMap[op->params_[op->params_.size() - 1]->type_->type_]) {
          qualifiedName = qualifiedName + "::" + subPath;
        }
        os << qualifiedName;
        if (!generatedSymbols[op->params_[op->params_.size() - 1]
                                  ->type_->type_]) {
          // inject a pointer as usage of incomplete type in class def not
          // permissible in C++
          //!@todo: this feels icky
          os << "*";
        }
      }
      os << " " << op->params_[op->params_.size() - 1]->name_;
    }

    //
    os << " ){}" << endl;
    return true;
  } else {
    return false;
  }
}

bool generateAttribute(std::ostream& os, Attribute* attribute) {
  if (attribute->type_->isPrimitive_) {
    std::string hash = "#";
    size_t index = strcspn(attribute->type_->type_, hash.c_str());
    std::string type = std::to_string(attribute->type_->type_[index]);
    if (type == "Boolean") {
      os << "bool ";
    } else if (type == "Real") {
      os << "double ";
    } else {
      os << "int ";
    }

  } else {
    // lookup type name of id
    std::string qualifiedName;
    for (auto& subPath : idNameMap[attribute->type_->type_]) {
      qualifiedName = qualifiedName + "::" + subPath;
    }
    os << qualifiedName;
    if (!generatedSymbols[attribute->type_->type_]) {
      // inject a pointer as usage of incomplete type in class def not
      // permissible in C++
      //!@todo: this feels icky
      os << "*";
    }
  }
  os << " " << attribute->name_ << ";" << endl;

  return true;
}

bool generateModule(std::ostream& os, ModuleNode* module) {
  bool result = true;

  os << "// Forward Decl" << endl;
  std::vector<char*> deps = module->getDependencies();
  for (size_t i = 0; i < deps.size(); i++) {
    if (!generatedSymbols[deps[i]]) {
      os << "class ";
      std::string qualifiedName;
      for (auto& subPath : idNameMap[deps[i]]) {
        qualifiedName = qualifiedName + "::" + subPath;
      }
      os << qualifiedName << ";" << endl;
    }
  }

  os << endl;

  os << "class " << module->name_ << " {" << endl;
  // generate private first since CPP classes default to private
  os << "// attributes" << endl;
  for (size_t i = 0; i < module->privateAttributes_.size(); i++) {
    result = generateAttribute(os, module->privateAttributes_[i]) && result;
  }
  os << "// operators " << endl;
  for (size_t i = 0; i < module->privateOperators_.size(); i++) {
    result = generateOperator(os, module->privateOperators_[i]) && result;
  }

  os << "public: " << endl << endl;
  os << "// attributes" << endl;

  for (size_t i = 0; i < module->publicAttributes_.size(); i++) {
    result = generateAttribute(os, module->publicAttributes_[i]) && result;
  }

  os << "// operators " << endl;
  for (size_t i = 0; i < module->publicOperators_.size(); i++) {
    result = generateOperator(os, module->publicOperators_[i]) && result;
  }

  os << "}; // class " << module->name_ << " " << module->id_ << endl << endl;

  generatedSymbols[module->id_] = true;

  return result;
}

bool generatePackage(ostream& os, Package* package) {
  bool result = true;
  os << "namespace " << package->name_ << "{" << endl << endl;

  for (size_t i = 0; i < package->packages_.size(); i++) {
    result = generatePackage(os, package->packages_[i]) && result;
  }

  for (size_t i = 0; i < package->modules_.size(); i++) {
    result = generateModule(os, package->modules_[i]) && result;
  }

  os << "} // namespace " << package->name_ << " " << package->id_ << endl
     << endl;

  return result;
}

bool CPPGenerator::generate(std::ostream& os, ModelNode* root) {
  bool result = true;
  idNameMap = root->idNameMap_;
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

  os << "} // namespace " << modelName << " " << root->id_ << endl << endl;

  os << "int main(int argc, char* argv[]) {" << endl << endl;
  os << "return 0;" << endl;
  os << "}" << endl;

  return result;
}
extern "C" IGenerator* create_generator() { return new CPPGenerator; }
extern "C" void destroy_generator(IGenerator* generator) { delete generator; }
}  // namespace XMR