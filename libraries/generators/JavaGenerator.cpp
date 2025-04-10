#include "generators/JavaGenerator.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <unordered_map>

using namespace std;

static unordered_map<string, bool> generatedSymbols;
static unordered_map<string, vector<string>> idNameMap;
static fstream workingFile;                         // keeps track of file we are currently in
static bool mainGenerated = false;                  // generate main once, currently in first module created
static unordered_map<string, string> packagePath;   // tracks the directory per package (key, val)=(package id,
                                                    // path)
static unordered_map<string, string> packageName;   // tracks the java name per package (key, val)=(package id,
                                                    // java name)
static unordered_map<string, string> classImports;  // tracks the package name of each class (key, val) =
                                                    // (class id, import name)
static unordered_map<string, string> classPaths;    // tracks the file location of each class (key, val) =
                                                    // (class id, file path)
static unordered_set<std::string> noNoNames = {};   // empty for now, left for future use if needed

namespace XMR {
bool checkOperatorName(char* name) {
  // lookup no no phrased for java, currently blank, left if I think of
  // something that wouldn't work
  if (noNoNames.contains(name)) {
    cerr << "Error: Java operator name: " << name << " reserved operator" << endl;
    return false;
  }

  return true;
}
bool generateOperator(std::ostream& os, Operator* op) {
  if (checkOperatorName(op->name_)) {
    if (op->visibility_ == Visibility::PRIVATE) {
      workingFile << "private ";
    } else if (op->visibility_ == Visibility::PUBLIC) {
      workingFile << "public ";
    }

    if (op->returnType_) {
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
        workingFile << "root.";
        for (int i = 0; i < idNameMap[op->returnType_->type_->type_].size(); ++i) {
          workingFile << idNameMap[op->returnType_->type_->type_][i];
          if (i < (idNameMap[op->returnType_->type_->type_].size() - 1)) {
            workingFile << ".";
          }
        }
      }
    } else {
      workingFile << "void";
    }

    workingFile << " " << op->name_ << "(";

    if (!op->params_.empty()) {
      for (size_t i = 0; i < op->params_.size() - 1; i++) {
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
          workingFile << "root.";
          for (int j = 0; j < idNameMap[op->params_[i]->type_->type_].size(); ++j) {
            workingFile << idNameMap[op->params_[i]->type_->type_][j];
            if (j < (idNameMap[op->params_[i]->type_->type_].size() - 1)) {
              workingFile << ".";
            }
          }
        }
        workingFile << " " << op->params_[i]->name_ << ", ";
      }

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
        workingFile << "root.";
        for (int j = 0; j < idNameMap[op->params_[op->params_.size() - 1]->type_->type_].size(); ++j) {
          workingFile << idNameMap[op->params_[op->params_.size() - 1]->type_->type_][j];
          if (j < (idNameMap[op->params_[op->params_.size() - 1]->type_->type_].size() - 1)) {
            workingFile << ".";
          }
        }
      }
      workingFile << " " << op->params_[op->params_.size() - 1]->name_;
    }

    //
    workingFile << "){}" << endl;
    return true;
  } else {
    return false;
  }
}
bool generateAttribute(std::ostream& os, Attribute* attribute) {
  if (attribute->type_->isPrimitive_) {
    if (attribute->visibility_ == Visibility::PRIVATE) {
      workingFile << "private ";
    } else if (attribute->visibility_ == Visibility::PUBLIC) {
      workingFile << "public ";
    }

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
    workingFile << "root.";
    for (int i = 0; i < idNameMap[attribute->type_->type_].size(); ++i) {
      workingFile << idNameMap[attribute->type_->type_][i];
      if (i < (idNameMap[attribute->type_->type_].size() - 1)) {
        workingFile << ".";
      }
    }
  }
  workingFile << " " << attribute->name_ << ";" << endl;

  return true;
}

bool generateModule(std::ostream& os, ModuleNode* module) {
  bool result = true;

  workingFile << "// Forward Decl" << endl;
  std::vector<string> deps = module->getSoftDependencies();
  for (size_t i = 0; i < deps.size(); i++) {
    if (!generatedSymbols[deps[i]] && classImports[deps[i]] != classImports[module->id_]) {
      workingFile << "import " << classImports[deps[i]] << ";" << endl;
    }
  }

  workingFile << endl;

  if (module->visibility_ == Visibility::PUBLIC) {
    workingFile << "public class " << module->name_ << " {" << endl;
  } else if (module->visibility_ == Visibility::PRIVATE) {
    workingFile << "private class " << module->name_ << " {" << endl;
  }
  // generate private first since CPP classes default to private
  workingFile << "// attributes" << endl;
  for (size_t i = 0; i < module->privateAttributes_.size(); i++) {
    result = generateAttribute(workingFile, module->privateAttributes_[i]) && result;
  }
  workingFile << "// operators" << endl;
  for (size_t i = 0; i < module->privateOperators_.size(); i++) {
    result = generateOperator(workingFile, module->privateOperators_[i]) && result;
  }
  workingFile << "// attributes" << endl;

  for (size_t i = 0; i < module->publicAttributes_.size(); i++) {
    result = generateAttribute(workingFile, module->publicAttributes_[i]) && result;
  }

  workingFile << "// operators" << endl;
  for (size_t i = 0; i < module->publicOperators_.size(); i++) {
    result = generateOperator(workingFile, module->publicOperators_[i]) && result;
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
    result = generatePackage(os, package->packages_[i]) && result;
  }

  for (size_t i = 0; i < package->modules_.size(); i++) {
    if (workingFile.is_open()) {
      workingFile.close();
    }
    workingFile.open(classPaths[package->modules_[i]->id_], ios::app);
    result = generateModule(os, package->modules_[i]) && result;
  }

  return result;
}

// utility function to store the import paths for each module. Used in first
// pass
static void genModuleLocations(vector<ModuleNode*> modules, string package_path, string package_name) {
  // java has public/private/abstract/none(package private)
  for (size_t i = 0; i < modules.size(); ++i) {
    classImports[modules[i]->id_] = package_name + "." + modules[i]->name_;
    classPaths[modules[i]->id_] = package_path + "/" + modules[i]->name_ + ".java";
    workingFile.open(package_path + "/" + modules[i]->name_ + ".java", fstream::out);
    workingFile << "package " << package_name << ";" << endl << endl;  // need to declare package in src files
    workingFile.close();
  }
}

// utility function to recursively generate the package names, also makes
// required directories. Used in first pass
static void createPackageNames(Package* package, string relative_path, string relative_name) {
  // compute our relative path in format for java package names
  string current_name = relative_name + "." + package->name_;
  string current_path = relative_path + "/" + package->name_;
  packageName[package->id_] = current_name;
  packagePath[package->id_] = current_path;
  filesystem::create_directory(current_path);

  genModuleLocations(package->modules_, current_path, current_name);

  if (package->packages_.size() == 0) {
    return;
  }

  for (size_t i = 0; i < package->packages_.size(); ++i) {
    createPackageNames(package->packages_[i], current_path, current_name);
  }
}

bool JavaGenerator::generate(std::ostream& os, ModelNode* root) {
  bool result = true;
  string rootPackage;  // keeps track of the root directory
  idNameMap = root->idNameMap_;
  string modelName = root->name_;
  rootPackage = "root/" + modelName;
  modelName = "root." + modelName;
  filesystem::create_directory("root");  // TODO find better way than hardcoding
  // java needs the package name to match the relative file path
  filesystem::create_directory(rootPackage);

  genModuleLocations(root->modules_, rootPackage, modelName);

  // doing a two pass generator, First pass:
  for (size_t i = 0; i < root->packages_.size(); i++) {
    createPackageNames(root->packages_[i], rootPackage,
                       modelName);  // generate the names
  }

  for (size_t i = 0; i < root->modules_.size(); i++) {
    if (workingFile.is_open()) {
      workingFile.close();
    }
    workingFile.open(classPaths[root->modules_[i]->id_], ios::app);
    result = generateModule(os, root->modules_[i]) && result;
  }

  for (size_t i = 0; i < root->packages_.size(); i++) {
    result = generatePackage(os, root->packages_[i]) && result;
    os << endl << endl;
  }

  return result;
}

extern "C" IGenerator* create_generator() { return new JavaGenerator; }
extern "C" void destroy_generator(IGenerator* generator) { delete generator; }
}  // namespace XMR