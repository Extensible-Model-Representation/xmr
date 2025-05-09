/**********************************************************
 * Copyright 2025 Jason Cisneros & Lucas Van Der Heijden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
        bool global = true;
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
  os << "// Forward Decl" << endl;

  // Only forward declare soft dependencies that haven't been generated
  // In C++ hard dependencies must be resolved with topological sort of class generation order.
  vector<string> deps = module->getSoftDependencies();
  for (size_t i = 0; i < deps.size(); i++) {
    if (!generatedSymbols[deps[i]] && (deps[i] != module->id_)) {
      vector<string> closeBraces;
      const size_t MIN_LENGTH = min(idNameMap[deps[i]].size() - 1, currentScope_.size());
      for (size_t j = 0; j < MIN_LENGTH; j++) {
        if (currentScope_[j] != idNameMap[deps[i]][j]) {
          closeBraces.push_back("}");
          os << "namespace " << idNameMap[deps[i]][j] << " { " << endl;
        }
      }

      for (size_t j = MIN_LENGTH; j < idNameMap[deps[i]].size() - 1; j++) {
        closeBraces.push_back("}");
        os << "namespace " << idNameMap[deps[i]][j] << " { " << endl;
        currentScope_.push_back(idNameMap[deps[i]][j]);
      }

      os << "class " << idNameMap[deps[i]][idNameMap[deps[i]].size() - 1] << ";" << endl;

      for (size_t j = 0; j < closeBraces.size(); j++) {
        os << closeBraces[j];
        closeBraces.pop_back();
        currentScope_.pop_back();
      }
    }
  }

  os << endl;

  vector<string> closeBraces;
  const size_t MIN_LENGTH = min(module->fullyQualified_.size() - 1, currentScope_.size());
  for (size_t j = 0; j < MIN_LENGTH; j++) {
    if (currentScope_[j] != module->fullyQualified_[j]) {
      closeBraces.push_back("}");
      os << "namespace " << module->fullyQualified_[j] << " { " << endl;
      currentScope_.push_back(module->fullyQualified_[j]);
    }
  }

  for (size_t j = MIN_LENGTH; j < module->fullyQualified_.size() - 1; j++) {
    closeBraces.push_back("}");
    os << "namespace " << module->fullyQualified_[j] << " { " << endl;
    currentScope_.push_back(module->fullyQualified_[j]);
  }

  os << "class " << module->fullyQualified_[module->fullyQualified_.size() - 1] << endl;
  currentScope_.push_back(module->name_);

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
  currentScope_.pop_back();

  for (size_t j = 0; j < closeBraces.size(); j++) {
    os << closeBraces[j];
    currentScope_.pop_back();
    closeBraces.pop_back();
  }

  generatedSymbols[module->id_] = true;
  return result;
}

// Flattens all modules to be used for circular dependency checks and topalogical sort
vector<ModuleNode*> flatten(ModelNode* root) {
  cout << "Flattening Modules" << endl;
  vector<ModuleNode*> flattenedModules;
  for (auto& package : root->packages_) {
    flattenedModules.insert(flattenedModules.end(), package->modules_.begin(), package->modules_.end());
  }

  flattenedModules.insert(flattenedModules.end(), root->modules_.begin(), root->modules_.end());
  cout << "Finished flattening modules" << endl;
  return flattenedModules;
}

bool hasHardCircularDependency(vector<ModuleNode*>& flattenedModules) {
  cout << "Checking for circular dependencies" << endl;
  vector<ModuleNode*> hardDependencies;

  for (auto& module : flattenedModules) {
    if (module->hardDependencyList_.size() > 0) hardDependencies.push_back(module);
  }

  // If there is only one module that has any form of hard dependency, cannot have circular dependency
  // unless its on itself which is never allowed.
  if (hardDependencies.size() <= 1) {
    cout << "Finished Checking for circular dependencies" << endl;
    return false;
  }

  DependencyGraph dp;
  for (auto& module : hardDependencies) {
    for (auto& dep : module->hardDependencyList_) {
      dp.addEdge(module->id_, dep.first);
    }
  }

  cout << "Finished Checking for circular dependencies" << endl;
  return dp.hasCycle();
}

vector<ModuleNode*> sortHardDependencies(vector<ModuleNode*> flattenedModules) {
  cout << "Starting dependency sort" << endl;
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
  vector<ModuleNode*> sortedModules;
  if (hardDependencies.size() > 1) {
    DependencyGraph dp;
    for (auto& module : hardDependencies) {
      for (auto& dep : module->hardDependencyList_) {
        dp.addEdge(module->id_, dep.first);
      }
    }
    vector<string> sortedDeps = dp.topSort();
    for (size_t i = sortedDeps.size() - 1; i > hardDependencies.size(); i++) {
      if (softDependencyIds.contains(sortedDeps[i])) {
        sortedDeps.pop_back();
      } else {
        break;
      }
    }
    // At this point, the sorted deps contains xmi id's of only modules that have hard dependencies but are are sorted correctly
    for (auto& id : sortedDeps) {
      for (auto& module : hardDependencies) {
        if (id == module->id_) {
          sortedModules.push_back(module);
        }
      }
    }
  } else {
    sortedModules.insert(sortedModules.end(), hardDependencies.begin(), hardDependencies.end());
  }

  sortedModules.insert(sortedModules.end(), softDependenciesOnly.begin(), softDependenciesOnly.end());
  // now reverse order so elements with the least # of hard deps are first in gen order
  reverse(sortedModules.begin(), sortedModules.end());

  cout << "Finished dependency sort" << endl;
  return sortedModules;
}

bool CPPGenerator::check(ModelNode* root) {
  checkCalled_ = true;

  vector<ModuleNode*> flattenedModules = flatten(root);

  if (flattenedModules.empty()) {
    cerr << "Failed to flatten modules!" << endl;
    return false;
  }

  if (hasHardCircularDependency(flattenedModules)) {
    cerr << "C++ cannot have hard circular dependencies!" << endl;
    modelValid_ = false;
    return false;
  }

  // now can  inverse toplogical sort hard dependencies
  vector<ModuleNode*> sortedModules = sortHardDependencies(flattenedModules);
  if (sortedModules.empty()) {
    cerr << "Failed to sort modules by dependency order!" << endl;
  }
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
    cerr << "Failed to generate due to invalid model layout. Check other logs for conditions that failed check!" << endl;
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

  //!@note: We do not generate packages as these are removed when we flatten the modules

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