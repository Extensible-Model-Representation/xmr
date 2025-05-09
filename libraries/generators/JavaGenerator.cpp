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
 * @filename: JavaGenerator.cpp
 * @brief:
 *
 ***********************************************************/
#include "generators/JavaGenerator.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <unordered_map>

using namespace std;

static unordered_map<string, bool> generatedSymbols;
static unordered_map<string, vector<string>> idNameMap;
static fstream workingFile;                        // keeps track of file we are currently in
static bool mainGenerated = false;                 // generate main once, currently in first module created
static unordered_set<std::string> noNoNames = {};  // empty for now, left for future use if needed

namespace XMR {
/*
 * Helper function that outputs the full name based
 * on the qualified name list given
 */
void outputFullName(vector<string> namelist) {
  workingFile << "src.";
  for (int i = 0; i < namelist.size(); ++i) {
    workingFile << namelist[i];
    if (i < (namelist.size() - 1)) {
      workingFile << ".";
    }
  }
}
/*
 * Helper function that returns the path to a class's
 * File based on the fully qualified name list given
 */
string returnFileLocation(vector<string> namelist) {
  string path = "./src/";
  for (int i = 0; i < namelist.size(); ++i) {
    path += namelist[i];
    if (i < (namelist.size() - 1)) {
      path += "/";
    }
  }
  path += ".java";
  return path;
}
/*
 * Helper function that returns the path to a package directory
 * based on the fully qualified name list given
 */
string returnPackagePath(vector<string> namelist) {
  string path = "./src/";
  for (int i = 0; i < namelist.size(); ++i) {
    path += namelist[i];
    if (i < (namelist.size() - 1)) {
      path += "/";
    }
  }
  return path;
}
/*
 * Helper function that returns the name of a package
 * based on the fully qualified name list given
 */
string packageName(vector<string> namelist) {
  string path = "src.";
  for (int i = 0; i < namelist.size(); ++i) {
    path += namelist[i];
    if (i < (namelist.size() - 1)) {
      path += ".";
    }
  }
  return path;
}
bool checkOperatorName(char* name) {
  // lookup no no phrased for java, currently blank, left if I think of
  // something that wouldn't work
  if (noNoNames.contains(name)) {
    cerr << "Error: Java operator name: " << name << " reserved operator" << endl;
    return false;
  }

  return true;
}
/*
 * Checks if the module passed inherits from a max of 1 other module
 * as multiple inheritance is not allowed in Java.
 */
bool checkSingleInheritance(ModuleNode* module) { return module->generalizations_.size() <= 1; }

bool generateOperator(std::ostream& os, Operator* op) {
  if (checkOperatorName(op->name_)) {
    if (op->visibility_ == Visibility::PRIVATE) {
      workingFile << "private ";
    } else if (op->visibility_ == Visibility::PUBLIC) {
      workingFile << "public ";
    } else if (op->visibility_ == Visibility::PROTECTED) {
      workingFile << "protected ";
    }  // if it is package public, we don't need to print anything

    if (op->returnType_) {
      if (op->returnType_->unlimited_) {
        workingFile << "java.util.List<";
        if (op->returnType_->type_->isPrimitive_) {
          std::string hash = "#";
          size_t index = strcspn(op->returnType_->type_->type_, hash.c_str());
          std::string type = std::to_string(op->returnType_->type_->type_[index]);
          if (type == "Boolean") {
            workingFile << "Boolean";
          } else if (type == "Real") {
            workingFile << "Double";
          } else {
            workingFile << "Integer";
          }

        } else {
          outputFullName(idNameMap[op->returnType_->type_->type_]);
        }
        workingFile << ">";
      } else {
        if (op->returnType_->type_->isPrimitive_) {
          std::string hash = "#";
          size_t index = strcspn(op->returnType_->type_->type_, hash.c_str());
          std::string type = std::to_string(op->returnType_->type_->type_[index]);
          if (type == "Boolean") {
            workingFile << "boolean";
          } else if (type == "Real") {
            workingFile << "double";
          } else {
            workingFile << "int";
          }

        } else {
          outputFullName(idNameMap[op->returnType_->type_->type_]);
        }

        // handle multiplicity
        if (!op->returnType_->unlimited_ && op->returnType_->multiplicity_ > 1) {
          workingFile << "[" << op->returnType_->multiplicity_ << "]";
        }
      }
    } else {
      workingFile << "void";
    }

    workingFile << " " << op->name_ << "(";

    if (!op->params_.empty()) {
      for (size_t i = 0; i < op->params_.size() - 1; i++) {
        if (op->params_[i]->unlimited_) {
          workingFile << "java.util.List<";
          if (op->params_[i]->type_->isPrimitive_) {
            std::string hash = "#";
            size_t index = strcspn(op->params_[i]->type_->type_, hash.c_str());
            std::string type = std::to_string(op->params_[i]->type_->type_[index]);
            if (type == "Boolean") {
              workingFile << "Boolean";
            } else if (type == "Real") {
              workingFile << "Double";
            } else {
              workingFile << "Integer";
            }

          } else {
            outputFullName(idNameMap[op->params_[i]->type_->type_]);
          }
          workingFile << ">";
        } else {
          if (op->params_[i]->type_->isPrimitive_) {
            std::string hash = "#";
            size_t index = strcspn(op->params_[i]->type_->type_, hash.c_str());
            std::string type = std::to_string(op->params_[i]->type_->type_[index]);
            if (type == "Boolean") {
              workingFile << "boolean";
            } else if (type == "Real") {
              workingFile << "double";
            } else {
              workingFile << "int";
            }

          } else {
            outputFullName(idNameMap[op->params_[i]->type_->type_]);
          }

          // handle multiplicity
          if (!op->params_[i]->unlimited_ && op->params_[i]->multiplicity_ > 1) {
            workingFile << "[" << op->params_[i]->multiplicity_ << "]";
          }
        }
        workingFile << " " << op->params_[i]->name_ << ", ";
      }
      if (op->params_[op->params_.size() - 1]->unlimited_) {
        workingFile << "java.util.List<";
        if (op->params_[op->params_.size() - 1]->type_->isPrimitive_) {
          std::string hash = "#";
          size_t index = strcspn(op->params_[op->params_.size() - 1]->type_->type_, hash.c_str());
          std::string type = std::to_string(op->params_[op->params_.size() - 1]->type_->type_[index]);
          if (type == "Boolean") {
            workingFile << "Boolean";
          } else if (type == "Real") {
            workingFile << "Double";
          } else {
            workingFile << "Integer";
          }

        } else {
          // lookup type name of id
          outputFullName(idNameMap[op->params_[op->params_.size() - 1]->type_->type_]);
        }
        workingFile << ">";
      } else {
        if (op->params_[op->params_.size() - 1]->type_->isPrimitive_) {
          std::string hash = "#";
          size_t index = strcspn(op->params_[op->params_.size() - 1]->type_->type_, hash.c_str());
          std::string type = std::to_string(op->params_[op->params_.size() - 1]->type_->type_[index]);
          if (type == "Boolean") {
            workingFile << "boolean";
          } else if (type == "Real") {
            workingFile << "double";
          } else {
            workingFile << "int";
          }

        } else {
          // lookup type name of id
          outputFullName(idNameMap[op->params_[op->params_.size() - 1]->type_->type_]);
        }
        // Handle multiplicity
        if (!op->params_[op->params_.size() - 1]->unlimited_ && op->params_[op->params_.size() - 1]->multiplicity_ > 1) {
          workingFile << "[" << op->params_[op->params_.size() - 1]->multiplicity_ << "]";
        }
      }
      workingFile << " " << op->params_[op->params_.size() - 1]->name_;
    }

    workingFile << "){}" << endl;
    return true;
  } else {
    return false;
  }
}
bool generateAttribute(std::ostream& os, Attribute* attribute) {
  if (attribute->visibility_ == Visibility::PRIVATE) {
    workingFile << "private ";
  } else if (attribute->visibility_ == Visibility::PUBLIC) {
    workingFile << "public ";
  } else if (attribute->visibility_ == Visibility::PROTECTED) {
    workingFile << "protected ";
  }  // if it is package public, we don't need to print anything
  if (attribute->unlimited_) {
    workingFile << "java.util.List<";
    if (attribute->type_->isPrimitive_) {
      std::string hash = "#";
      size_t index = strcspn(attribute->type_->type_, hash.c_str());
      std::string type = std::to_string(attribute->type_->type_[index]);
      if (type == "Boolean") {
        workingFile << "Boolean";
      } else if (type == "Real") {
        workingFile << "Double";
      } else {
        workingFile << "Integer";
      }
    } else {
      outputFullName(idNameMap[attribute->type_->type_]);
    }
    workingFile << ">";
  } else {
    if (attribute->type_->isPrimitive_) {
      std::string hash = "#";
      size_t index = strcspn(attribute->type_->type_, hash.c_str());
      std::string type = std::to_string(attribute->type_->type_[index]);
      if (type == "Boolean") {
        workingFile << "boolean";
      } else if (type == "Real") {
        workingFile << "double";
      } else {
        workingFile << "int";
      }

    } else {
      outputFullName(idNameMap[attribute->type_->type_]);
    }
    // Handle multiplicity
    if (!attribute->unlimited_ && attribute->multiplicity_ > 1) {
      workingFile << "[" << attribute->multiplicity_ << "]";
    }
  }
  workingFile << " " << attribute->name_ << ";" << endl;
  return true;
}

bool generateModule(std::ostream& os, ModuleNode* module) {
  bool result = true;
  if (!checkSingleInheritance(module)) {
    result = false;
    cerr << "Generation error with module \"" << module->name_ << "\": Java does not support multiple inheritance" << endl;
    return result;
  }
  // Now that it is using the fully qualified name for everything,
  // we don't even need import statements.
  // If this needs to be added back in, we would just import everything
  // in the hard and soft dependancies
  //
  // workingFile << "// Forward Decl" << endl;
  // std::vector<string> deps = module->getSoftDependencies();
  // for (size_t i = 0; i < deps.size(); i++) {
  //   if (!generatedSymbols[deps[i]] && classImports[deps[i]] != classImports[module->id_]) {
  //     workingFile << "import " << classImports[deps[i]] << ";" << endl;
  //   }
  // }

  workingFile << endl;

  if (module->visibility_ == Visibility::PUBLIC) {
    workingFile << "public class " << module->name_;
  } else if (module->visibility_ == Visibility::PRIVATE) {
    workingFile << "private class " << module->name_;
  } else if (module->visibility_ == Visibility::PROTECTED) {
    workingFile << "protected class " << module->name_;
  } else if (module->visibility_ == Visibility::PACKAGE) {
    workingFile << "class " << module->name_;
  }

  if (module->generalizations_.size() == 1) {
    workingFile << " extends ";
    outputFullName(idNameMap[module->generalizations_[0]]);
  }
  workingFile << " {" << endl;

  // Generate nested modules
  workingFile << "// modules" << endl;
  for (size_t i = 0; i < module->publicModules_.size(); i++) {
    result = generateModule(workingFile, module->publicModules_[i]) && result;
  }
  for (size_t i = 0; i < module->privateModules_.size(); i++) {
    result = generateModule(workingFile, module->privateModules_[i]) && result;
  }
  for (size_t i = 0; i < module->protectedModules_.size(); i++) {
    result = generateModule(workingFile, module->protectedModules_[i]) && result;
  }
  for (size_t i = 0; i < module->packageModules_.size(); i++) {
    result = generateModule(workingFile, module->packageModules_[i]) && result;
  }

  // Generate attributes
  workingFile << "// attributes" << endl;
  for (size_t i = 0; i < module->publicAttributes_.size(); i++) {
    result = generateAttribute(workingFile, module->publicAttributes_[i]) && result;
  }
  for (size_t i = 0; i < module->privateAttributes_.size(); i++) {
    result = generateAttribute(workingFile, module->privateAttributes_[i]) && result;
  }
  for (size_t i = 0; i < module->protectedAttributes_.size(); i++) {
    result = generateAttribute(workingFile, module->protectedAttributes_[i]) && result;
  }
  for (size_t i = 0; i < module->packageAttributes_.size(); i++) {
    result = generateAttribute(workingFile, module->packageAttributes_[i]) && result;
  }

  // Generate operators
  workingFile << "// operators" << endl;
  for (size_t i = 0; i < module->publicOperators_.size(); i++) {
    result = generateOperator(workingFile, module->publicOperators_[i]) && result;
  }
  for (size_t i = 0; i < module->privateOperators_.size(); i++) {
    result = generateOperator(workingFile, module->privateOperators_[i]) && result;
  }
  for (size_t i = 0; i < module->protectedOperators_.size(); i++) {
    result = generateOperator(workingFile, module->protectedOperators_[i]) && result;
  }
  for (size_t i = 0; i < module->packageOperators_.size(); i++) {
    result = generateOperator(workingFile, module->packageOperators_[i]) && result;
  }

  // TODO: put main in proper spot
  if (!mainGenerated) {
    mainGenerated = true;

    workingFile << "public static void main(String[] args) {" << endl << endl;
    workingFile << "}" << endl;
  }

  workingFile << "} // class " << module->name_ << " " << module->id_ << endl << endl;

  generatedSymbols[module->id_] = true;
  return result;
}

bool generatePackage(ostream& os, Package* package) {
  bool result = true;

  for (size_t i = 0; i < package->packages_.size(); i++) {
    filesystem::create_directory(returnPackagePath(package->packages_[i]->fullyQualified_));
    result = generatePackage(os, package->packages_[i]) && result;
  }

  for (size_t i = 0; i < package->modules_.size(); i++) {
    if (workingFile.is_open()) {
      workingFile.close();
    }
    workingFile.open(returnFileLocation(idNameMap[package->modules_[i]->id_]), ios::app);
    workingFile << "package " << packageName(package->fullyQualified_) << ";" << endl;
    if (package->modules_[i]->visibility_ == Visibility::PUBLIC || package->modules_[i]->visibility_ == Visibility::PACKAGE) {
      result = generateModule(os, package->modules_[i]) && result;
    } else {
      cerr << "Generation error with module \"" << package->modules_[i]->name_ << "\": Private and Protected modules must be nested in another module." << endl;
    }
  }

  return result;
}

bool JavaGenerator::generate(std::ostream& os, ModelNode* root) {
  bool result = true;
  string rootPackage;  // keeps track of the root directory
  idNameMap = root->idNameMap_;
  string modelName = root->name_;
  rootPackage = "src/" + modelName;
  modelName = "src." + modelName;
  filesystem::create_directory("src");  // TODO find better way than hardcoding
  // java needs the package name to match the relative file path
  filesystem::create_directory(rootPackage);

  for (size_t i = 0; i < root->modules_.size(); i++) {
    if (workingFile.is_open()) {
      workingFile.close();
    }
    workingFile.open(returnFileLocation(idNameMap[root->modules_[i]->id_]), ios::app);
    workingFile << "package " << modelName << ";" << endl;
    if (root->modules_[i]->visibility_ == Visibility::PUBLIC || root->modules_[i]->visibility_ == Visibility::PACKAGE) {
      result = generateModule(os, root->modules_[i]) && result;
    } else {
      cerr << "Generation error with module \"" << root->modules_[i]->name_ << "\": Private and Protected modules must be nested in another module." << endl;
    }
  }

  for (size_t i = 0; i < root->packages_.size(); i++) {
    filesystem::create_directory(returnPackagePath(root->packages_[i]->fullyQualified_));
    result = generatePackage(os, root->packages_[i]) && result;
    os << endl << endl;
  }

  return result;
}

extern "C" IGenerator* create_generator() { return new JavaGenerator; }
extern "C" void destroy_generator(IGenerator* generator) { delete generator; }
}  // namespace XMR