/**********************************************************
 *
 * @filename: CPPGenerator.cpp
 * @brief:
 *
 ***********************************************************/
#include "generators/CPPGenerator.hpp"

#include <algorithm>
#include <cstring>
#include <set>
#include <unordered_map>

#include "generators/Graph.hpp"

using namespace std;

static vector<string> currentScope_;
static unordered_map<string, bool> generatedSymbols;
static unordered_map<string, vector<string>> idNameMap;
static unordered_set<string> noNoNames = {"delete", "new"};
namespace XMR {

string generateQualifedName(string fullName) {
  string qualifiedName;
  const size_t MIN_LENGTH = min(idNameMap[fullName].size(), currentScope_.size());

  for (size_t j = 0; j < MIN_LENGTH; j++) {
    string subPath = idNameMap[fullName][j];
    if (subPath == currentScope_[j]) {
      continue;
    } else {
      // Check if qualified name is starting in the global namespace
      if (qualifiedName.empty() && j == 0) {
        qualifiedName = "::" + subPath;
      }
      // Check if first namespace to be added
      else if (qualifiedName.empty() && j != 0) {
        qualifiedName = subPath;
      }
      // Append subpath to qualified name
      else {
        qualifiedName = qualifiedName + "::" + subPath;
      }
    }
  }

  // Check if there is any remaining names in fully qualified path
  // given that current scope was the min length
  if (MIN_LENGTH < idNameMap[fullName].size()) {
    for (size_t j = MIN_LENGTH; j < idNameMap[fullName].size(); j++) {
      string subPath = idNameMap[fullName][j];
      // If empty append global namespace
      if (qualifiedName.empty()) {
        qualifiedName = "::" + subPath;
      } else {
        qualifiedName = qualifiedName + "::" + subPath;
      }
    }
  }

  if (qualifiedName.empty()) {
    qualifiedName = fullName.back();
  }

  return qualifiedName;
}

bool checkOperatorName(char* name) {
  // lookup no no phrased for c++ operator names, i.e. new delete
  if (noNoNames.contains(name)) {
    cerr << "Error: C++ operator name: " << name << " reserved operator" << endl;
    return false;
  }

  return true;
}
bool generateOperator(std::ostream& os, Operator* op) {
  if (checkOperatorName(op->name_)) {
    if (op->returnType_) {
      if (op->returnType_->type_->isPrimitive_) {
        std::string hash = "#";
        size_t index = strcspn(op->returnType_->type_->type_, hash.c_str());
        std::string type = std::to_string(op->returnType_->type_->type_[index]);
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
        const size_t MIN_LENGTH = min(idNameMap[op->returnType_->type_->type_].size(), currentScope_.size());

        for (size_t i = 0; i < MIN_LENGTH; i++) {
          string subPath = idNameMap[op->returnType_->type_->type_][i];
          if (subPath == currentScope_[i]) {
            continue;
          } else {
            // Check if qualified name is starting in the global namespace
            if (qualifiedName.empty() && i == 0) {
              qualifiedName = "::" + subPath;
            }
            // Check if first namespace to be added
            else if (qualifiedName.empty() && i != 0) {
              qualifiedName = subPath;
            }
            // Append subpath to qualified name
            else {
              qualifiedName = qualifiedName + "::" + subPath;
            }
          }
        }

        // Check if there is any remaining names in fully qualified path
        // given that current scope was the min length
        if (MIN_LENGTH < idNameMap[op->returnType_->type_->type_].size()) {
          for (size_t i = MIN_LENGTH; i < idNameMap[op->returnType_->type_->type_].size(); i++) {
            string subPath = idNameMap[op->returnType_->type_->type_][i];
            // If empty append global namespace
            if (qualifiedName.empty()) {
              qualifiedName = "::" + subPath;
            } else {
              qualifiedName = qualifiedName + "::" + subPath;
            }
          }
        }

        // If this is true, the dependency class is the class itself.
        if (qualifiedName.empty()) {
          // Last string in the vector is the name of the class
          qualifiedName = currentScope_.back();
        }

        os << qualifiedName;

        // if (!generatedSymbols[op->params_[i]->type_->type_]) {
        //   // inject a pointer as usage of incomplete type in class def not
        //   // permissible in C++
        //   //!@todo: this feels icky
        //   os << "*";
        // }
      }

      if (op->returnType_->nilable_) {
        os << "*";
      }

      if (op->returnType_->unlimited_) {
        os << "*";
      }

      // Check if multiplicity between 2 - 6
      if (!op->returnType_->unlimited_ && op->returnType_->multiplicity_ > 1) {
        os << "[ " << op->returnType_->multiplicity_ << " ] ";
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
          std::string type = std::to_string(op->params_[i]->type_->type_[index]);
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
          const size_t MIN_LENGTH = min(idNameMap[op->params_[i]->type_->type_].size(), currentScope_.size());
          for (size_t j = 0; j < MIN_LENGTH; j++) {
            string subPath = idNameMap[op->params_[i]->type_->type_][j];
            if (subPath == currentScope_[j]) {
              continue;
            } else {
              // Check if qualified name is starting in the global namespace
              if (qualifiedName.empty() && j == 0) {
                qualifiedName = "::" + subPath;
              }
              // Check if first namespace to be added
              else if (qualifiedName.empty() && j != 0) {
                qualifiedName = subPath;
              }
              // Append subpath to qualified name
              else {
                qualifiedName = qualifiedName + "::" + subPath;
              }
            }
          }

          // Check if there is any remaining names in fully qualified path
          // given that current scope was the min length
          if (MIN_LENGTH < idNameMap[op->params_[i]->type_->type_].size()) {
            for (size_t j = MIN_LENGTH; j < idNameMap[op->params_[i]->type_->type_].size(); j++) {
              string subPath = idNameMap[op->params_[i]->type_->type_][j];
              // If empty append global namespace
              if (qualifiedName.empty()) {
                qualifiedName = "::" + subPath;
              } else {
                qualifiedName = qualifiedName + "::" + subPath;
              }
            }
          }

          // If this is true, the dependency class is the class itself.
          if (qualifiedName.empty()) {
            qualifiedName = op->params_[i]->name_;
          }

          os << qualifiedName;

          // if (!generatedSymbols[op->params_[i]->type_->type_]) {
          //   // inject a pointer as usage of incomplete type in class def not
          //   // permissible in C++
          //   //!@todo: this feels icky
          //   os << "*";
          // }
        }

        if (op->params_[i]->nilable_) {
          os << "*";
        }

        if (op->params_[i]->unlimited_) {
          os << "*";
        }

        os << " " << op->params_[i]->name_;

        // Check if multiplicity between 2 - 6
        if (!op->params_[i]->unlimited_ && op->params_[i]->multiplicity_ > 1) {
          os << "[ " << op->params_[i]->multiplicity_ << " ] ";
        }

        os << ", ";
      }

      if (op->params_[op->params_.size() - 1]->type_->isPrimitive_) {
        std::string hash = "#";
        size_t index = strcspn(op->params_[op->params_.size() - 1]->type_->type_, hash.c_str());
        std::string type = std::to_string(op->params_[op->params_.size() - 1]->type_->type_[index]);
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
        const size_t MIN_LENGTH = min(idNameMap[op->params_[op->params_.size() - 1]->type_->type_].size(), currentScope_.size());

        for (size_t i = 0; i < MIN_LENGTH; i++) {
          string subPath = idNameMap[op->params_[op->params_.size() - 1]->type_->type_][i];
          if (subPath == currentScope_[i]) {
            continue;
          } else {
            // Check if qualified name is starting in the global namespace
            if (qualifiedName.empty() && i == 0) {
              qualifiedName = "::" + subPath;
            }
            // Check if first namespace to be added
            else if (qualifiedName.empty() && i != 0) {
              qualifiedName = subPath;
            }
            // Append subpath to qualified name
            else {
              qualifiedName = qualifiedName + "::" + subPath;
            }
          }
        }

        // Check if there is any remaining names in fully qualified path
        // given that current scope was the min length
        if (MIN_LENGTH < idNameMap[op->params_[op->params_.size() - 1]->type_->type_].size()) {
          for (size_t i = MIN_LENGTH; i < idNameMap[op->params_[op->params_.size() - 1]->type_->type_].size(); i++) {
            string subPath = idNameMap[op->params_[op->params_.size() - 1]->type_->type_][i];
            // If empty append global namespace
            if (qualifiedName.empty()) {
              qualifiedName = "::" + subPath;
            } else {
              qualifiedName = qualifiedName + "::" + subPath;
            }
          }
        }

        // If this is true, the dependency class is the class itself.
        if (qualifiedName.empty()) {
          qualifiedName = op->params_[op->params_.size() - 1]->name_;
        }

        os << qualifiedName;
        if (!generatedSymbols[op->params_[op->params_.size() - 1]->type_->type_]) {
          // inject a pointer as usage of incomplete type in class def not
          // permissible in C++
          //!@todo: this feels icky
          os << "*";
        }
      }

      if (op->params_[op->params_.size() - 1]->nilable_) {
        os << "*";
      }

      if (op->params_[op->params_.size() - 1]->unlimited_) {
        os << "*";
      }
      os << " " << op->params_[op->params_.size() - 1]->name_;

      // Check if multiplicity between 2 - 6
      if (!op->params_[op->params_.size() - 1]->unlimited_ && op->params_[op->params_.size() - 1]->multiplicity_ > 1) {
        os << "[ " << op->params_[op->params_.size() - 1]->multiplicity_ << " ] ";
      }
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
    string qualifiedName;
    const size_t MIN_LENGTH = min(idNameMap[attribute->type_->type_].size(), currentScope_.size());

    for (size_t i = 0; i < MIN_LENGTH; i++) {
      string subPath = idNameMap[attribute->type_->type_][i];
      if (subPath == currentScope_[i]) {
        continue;
      } else {
        // Check if qualified name is starting in the global namespace
        if (qualifiedName.empty() && i == 0) {
          qualifiedName = "::" + subPath;
        }
        // Check if first namespace to be added
        else if (qualifiedName.empty() && i != 0) {
          qualifiedName = subPath;
        }
        // Append subpath to qualified name
        else {
          qualifiedName = qualifiedName + "::" + subPath;
        }
      }
    }

    // Check if there is any remaining names in fully qualified path
    // given that current scope was the min length
    if (MIN_LENGTH < idNameMap[attribute->type_->type_].size()) {
      for (size_t i = MIN_LENGTH; i < idNameMap[attribute->type_->type_].size(); i++) {
        string subPath = idNameMap[attribute->type_->type_][i];
        // If empty append global namespace
        if (qualifiedName.empty()) {
          qualifiedName = "::" + subPath;
        } else {
          qualifiedName = qualifiedName + "::" + subPath;
        }
      }
    }

    // If this is true, the dependency class is the class itself.
    if (qualifiedName.empty()) {
      // Last string in the vector is the name of the class
      qualifiedName = currentScope_.back();
    }

    os << qualifiedName;

    // if (!generatedSymbols[attribute->type_->type_] && !attribute->nilable_) {
    //   // inject a pointer as usage of incomplete type in class def not
    //   // permissible in C++
    //   //!@todo: this feels icky
    //   os << "*";
    // }
  }

  if (attribute->nilable_) {
    os << "*";
  }

  if (attribute->unlimited_) {
    os << "*";
  }

  os << " " << attribute->name_;
  if (!attribute->unlimited_ && attribute->multiplicity_ > 1) {
    os << "[ " << attribute->multiplicity_ << " ] ";
  }

  os << ";" << endl;

  return true;
}

bool generateModule(std::ostream& os, ModuleNode* module) {
  bool result = true;
  currentScope_.push_back(module->name_);
  os << "// Forward Decl" << endl;

  // Only forward declare soft dependencies that haven't been generated
  // In C++ hard dependencies must be resolved with topological sort of class generation order.
  vector<string> deps = module->getSoftDependencies();
  for (size_t i = 0; i < deps.size(); i++) {
    if (!generatedSymbols[deps[i]]) {
      os << "class ";
      string qualifiedName;
      const size_t MIN_LENGTH = min(idNameMap[deps[i]].size(), currentScope_.size());

      for (size_t j = 0; j < MIN_LENGTH; j++) {
        string subPath = idNameMap[deps[i]][j];
        if (subPath == currentScope_[j]) {
          continue;
        } else {
          // Check if qualified name is starting in the global namespace
          if (qualifiedName.empty() && j == 0) {
            qualifiedName = "::" + subPath;
          }
          // Check if first namespace to be added
          else if (qualifiedName.empty() && j != 0) {
            qualifiedName = subPath;
          }
          // Append subpath to qualified name
          else {
            qualifiedName = qualifiedName + "::" + subPath;
          }
        }
      }

      // Check if there is any remaining names in fully qualified path
      // given that current scope was the min length
      if (MIN_LENGTH < idNameMap[deps[i]].size()) {
        for (size_t j = MIN_LENGTH; j < idNameMap[deps[i]].size(); j++) {
          string subPath = idNameMap[deps[i]][j];
          // If empty append global namespace
          if (qualifiedName.empty()) {
            qualifiedName = "::" + subPath;
          } else {
            qualifiedName = qualifiedName + "::" + subPath;
          }
        }
      }

      // If this is true, the dependency class is the class itself.
      if (qualifiedName.empty()) {
        qualifiedName = module->name_;
      }
      os << qualifiedName << ";" << endl;
    }
  }

  os << endl;

  os << "class " << module->name_;

  // Check for inheritance
  if (!module->generalizations_.empty()) {
    // If only one generate single, else generate n - 1 then generate last one to handle not adding comma
    if (module->generalizations_.size() == 1) {
      os << " : public ";
      string qualifiedName = generateQualifedName(module->generalizations_[0]);
      os << qualifiedName;

    } else {
      // more than 1 generalization, generate first n - 1
      os << " : ";
      for (size_t i = 0; i < module->generalizations_.size() - 1; i++) {
        os << "public ";
        string qualifiedName = generateQualifedName(module->generalizations_[i]);
        os << qualifiedName << ", ";
      }

      // Generate the nth qualified name;
      os << "public ";
      string qualifiedName = generateQualifedName(module->generalizations_[module->generalizations_.size() - 1]);
      os << qualifiedName;
    }
  }

  os << " {" << endl;
  // generate private first since CPP classes default to private
  os << "// attributes" << endl;
  for (size_t i = 0; i < module->privateAttributes_.size(); i++) {
    result = generateAttribute(os, module->privateAttributes_[i]) && result;
  }
  os << "// operators " << endl;
  for (size_t i = 0; i < module->privateOperators_.size(); i++) {
    result = generateOperator(os, module->privateOperators_[i]) && result;
  }

  // Next protected
  os << "protected: " << endl << endl;
  os << "// attributes" << endl;

  for (size_t i = 0; i < module->protectedAttributes_.size(); i++) {
    result = generateAttribute(os, module->protectedAttributes_[i]) && result;
  }

  os << "// operators " << endl;
  for (size_t i = 0; i < module->protectedOperators_.size(); i++) {
    result = generateOperator(os, module->protectedOperators_[i]) && result;
  }

  // Finally public.
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
  currentScope_.pop_back();
  return result;
}

bool generatePackage(ostream& os, Package* package) {
  bool result = true;
  currentScope_.push_back(package->name_);
  os << "namespace " << package->name_ << "{" << endl << endl;

  for (size_t i = 0; i < package->packages_.size(); i++) {
    result = generatePackage(os, package->packages_[i]) && result;
  }

  for (size_t i = 0; i < package->modules_.size(); i++) {
    result = generateModule(os, package->modules_[i]) && result;
  }

  os << "} // namespace " << package->name_ << " " << package->id_ << endl << endl;
  currentScope_.pop_back();
  return result;
}

// Flattens all modules to be used for circular dependency checks and topalogical sort
vector<ModuleNode*> flatten(ModelNode* root) {
  vector<ModuleNode*> flattenedModules;
  for (auto& package : root->packages_) {
    flattenedModules.insert(flattenedModules.end(), package->modules_.begin(), package->modules_.end());
  }

  flattenedModules.insert(flattenedModules.begin(), root->modules_.begin(), root->modules_.end());
  return flattenedModules;
}

bool hasHardCircularDependency(vector<ModuleNode*>& flattenedModules) {
  //!@todo: Last thing todo for robust checking
  return false;
}

vector<ModuleNode*> sortHardDependencies(vector<ModuleNode*> flattenedModules) {
  vector<ModuleNode*> softDependenciesOnly;
  set<string> softDependencyIds;
  vector<ModuleNode*> hardDependencies;

  for (auto& module : flattenedModules) {
    if (module->hardDependencyList_.size() > 0) {
      hardDependencies.push_back(module);
    } else {
      softDependenciesOnly.push_back(module);
      softDependencyIds.insert(module->id_);
    }
  }
  DependencyGraph dp;
  for (auto& module : hardDependencies) {
    for (auto& dep : module->hardDependencyList_) {
      cout << "Adding edge: v: " << module->id_ << " w: " << dep.first << endl;
      dp.addEdge(module->id_, dep.first);
    }
  }
  vector<string> sortedDeps = dp.topSort();
  cout << "Sorted Deps: ";
  for (auto& dep : sortedDeps) {
    cout << " " << dep;
  }
  for (size_t i = sortedDeps.size() - 1; i > hardDependencies.size(); i++) {
    if (softDependencyIds.contains(sortedDeps[i])) {
      sortedDeps.pop_back();
    } else {
      break;
    }
  }
  vector<ModuleNode*> sortedModules;
  // At this point, the sorted deps contains xmi id's of only modules that have hard dependencies but are are sorted correctly
  for (auto& id : sortedDeps) {
    for (auto& module : hardDependencies) {
      if (id == module->id_) {
        sortedModules.push_back(module);
      }
    }
  }

  sortedModules.insert(sortedModules.end(), softDependenciesOnly.begin(), softDependenciesOnly.end());
  // now reverse order so elements with the least # of hard deps are first in gen order
  reverse(sortedModules.begin(), sortedModules.end());
  return sortedModules;
}

bool CPPGenerator::check(ModelNode* root) {
  checkCalled_ = true;

  vector<ModuleNode*> flattenedModules = flatten(root);
  if (hasHardCircularDependency(flattenedModules)) {
    cerr << "C++ cannot have hard circular dependencies!" << endl;
    modelValid_ = false;
    return false;
  }

  // now can  inverse toplogical sort hard dependencies
  vector<ModuleNode*> sortedModules = sortHardDependencies(flattenedModules);
  root->packages_.clear();
  root->modules_ = sortedModules;
  modelValid_ = true;
  return true;
}

bool CPPGenerator::generate(std::ostream& os, ModelNode* root) {
  bool result;
  if (!checkCalled_) {
    result = this->check(root);
  } else {
    result = modelValid_;
  }

  if (!result) {
    cerr << "Failed to generate due to invalid model layout. Check other logs for conditions that failed check!";
    return false;
  }

  currentScope_.push_back(root->name_);
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
  currentScope_.pop_back();
  return result;
}
extern "C" IGenerator* create_generator() { return new CPPGenerator; }
extern "C" void destroy_generator(IGenerator* generator) { delete generator; }
}  // namespace XMR