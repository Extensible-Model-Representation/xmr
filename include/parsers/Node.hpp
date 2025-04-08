/**********************************************************
 *
 * @filename: Node.hpp
 * @brief:
 *
 ***********************************************************/
#pragma once
#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define MAX_STRING_SIZE 100

namespace XMR {

// Node class that all XMR generateble elements inherit from
class Node {
 public:
  /**
   * Calls this node generate function that will stream code to the based in
   * ostream
   * @param[out] os: modified ostream with the node code gen
   */
  virtual void generate(std::ostream& os) = 0;

 protected:
  Node() = default;
  ~Node() = default;
};

enum Visibility { PUBLIC, PRIVATE };
enum Direction { IN, OUT };

class PackageImport : public Node {};

class Relationship : public Node {};

class Type {
 public:
  char* type_;
  bool isPrimitive_;
  Type(char* type, bool isPrimitive = false) : type_(type), isPrimitive_(isPrimitive) {}
};

class Param {
 public:
  char* name_ = nullptr;
  char* id_ = nullptr;
  Type* type_ = nullptr;
  Direction direction_;

  Param(char* name, char* id, Type* type, Direction direction = Direction::IN) : name_(name), id_(id), type_(type), direction_(direction) {}

  friend std::ostream& operator<<(std::ostream& os, const Param node) {
    os << "Param Name: " << node.name_ << " Param ID: " << node.id_ << " Param Type: " << node.type_->type_ << " Param Direction: " << node.direction_ << std::endl;
    return os;
  }
};

class Operator : public Node {
 public:
  char* name_ = nullptr;
  char* id_ = nullptr;
  Visibility visibility_;
  std::vector<Param*> params_;
  Type* returnType_ = nullptr;

  Operator(char* name, char* id, Visibility visibility = Visibility::PUBLIC, Type* returnType = nullptr) : name_(name), id_(id), visibility_(visibility), returnType_(returnType) {}

  void generate(std::ostream& os) final {
    os << "Called Operator Generate for Operator Node: " << std::endl;
    os << *this << std::endl;
  }

  void addParam(Param* param) { params_.push_back(param); }
  void addReturnType(Type* returnType) { returnType_ = returnType; }

  friend std::ostream& operator<<(std::ostream& os, const Operator node) {
    os << "Operator Name: " << node.name_ << std::endl;
    os << "Operator Id: " << node.id_ << std::endl;
    os << "Operator Visiblity: " << node.visibility_ << std::endl;
    os << "Operator Return Type: " << (node.returnType_ == nullptr ? "Void" : node.returnType_->type_) << std::endl;
    os << "Operator Params: " << std::endl;
    for (size_t i = 0; i < node.params_.size(); i++) {
      os << *node.params_[i];
    }
    os << std::endl;
    return os;
  }
};

class Attribute : public Node {
 public:
  char* name_ = nullptr;
  char* id_ = nullptr;
  Type* type_ = nullptr;
  Visibility visibility_;

  Attribute(char* name, char* id, Type* type, Visibility visibility = Visibility::PUBLIC) : name_(name), id_(id), type_(type), visibility_(visibility) {}

  void generate(std::ostream& os) final {
    os << "Called Attribute Generate for Attribute Node: " << std::endl;
    os << *this << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& os, const Attribute node) {
    os << "Attribute Name: " << node.name_ << std::endl;
    os << "Attribute Id: " << node.id_ << std::endl;
    os << "Attribute Type: " << node.type_->type_ << std::endl;
    os << "Attribute Visiblity: " << node.visibility_ << std::endl;
    return os;
  }
};

class ModuleNode : public Node {
 public:
  char* name_ = nullptr;
  char* id_ = nullptr;
  Visibility visibility_;

  std::vector<Operator*> publicOperators_;
  std::vector<Operator*> privateOperators_;
  std::vector<Attribute*> publicAttributes_;
  std::vector<Attribute*> privateAttributes_;
  std::unordered_map<std::string, std::string> dependencyList_;
  std::vector<std::string> fullyQualified_;  // this module inclusive

  //!@todo: Do we want to default visibility if not set? Will it never be not
  //! set in the metadata?
  ModuleNode(char* name, char* id, std::vector<std::string> fullyQualified, Visibility visibility = Visibility::PUBLIC)
      : name_(name), id_(id), fullyQualified_(fullyQualified), visibility_(visibility) {}

  std::vector<std::string> getDependencies() {
    std::vector<std::string> result;
    for (auto& pair : dependencyList_) {
      result.push_back(pair.first);
    }
    return result;
  }

  void addOperator(Operator* op) {
    for (size_t i = 0; i < op->params_.size(); i++) {
      if (!op->params_[i]->type_->isPrimitive_) {
        dependencyList_[op->params_[i]->type_->type_] = op->params_[i]->type_->type_;
      }
    }
    if (op->visibility_ == Visibility::PRIVATE) {
      privateOperators_.push_back(op);

    } else {
      publicOperators_.push_back(op);
    }
  }

  void addAttribute(Attribute* attribute) {
    if (!attribute->type_->isPrimitive_) {
      dependencyList_[attribute->type_->type_] = attribute->type_->type_;
    }
    if (attribute->visibility_ == Visibility::PRIVATE) {
      privateAttributes_.push_back(attribute);
    } else {
      publicAttributes_.push_back(attribute);
    }
  }

  void generate(std::ostream& os) final {
    os << "Called Module Generate for Module Node: " << std::endl;
    os << *this << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& os, const ModuleNode node) {
    os << "Module Name: " << node.name_ << std::endl;
    os << "Module Id: " << node.id_ << std::endl;
    os << "Module Visiblity: " << node.visibility_ << std::endl;
    return os;
  }
};

class Package : public Node {
 public:
  char* name_ = nullptr;
  char* id_ = nullptr;
  std::vector<Package*> packages_;
  std::vector<ModuleNode*> modules_;
  std::vector<Relationship*> relationships_;
  std::vector<std::string> fullyQualified_;  // This package inclusive

  Package(char* name, char* id, std::vector<std::string> fullyQualified) : name_(name), id_(id), fullyQualified_(fullyQualified) {}

  inline void addPackage(Package* package) { packages_.push_back(package); }

  inline void addModule(ModuleNode* module) { modules_.push_back(module); }

  inline void addRelationship(Relationship* relationship) { relationships_.push_back(relationship); }

  void generate(std::ostream& os) final {
    os << "Called Package Generate for Package Node: " << std::endl;
    os << *this << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& os, const Package node) {
    os << "Package Name: " << node.name_ << std::endl;
    os << "Package Id: " << node.id_ << std::endl;
    return os;
  }
};

class ModelNode : public Node {
 public:
  char* name_ = nullptr;
  char* id_ = nullptr;
  std::vector<PackageImport*> packageImports_;
  std::vector<Package*> packages_;
  std::vector<ModuleNode*> modules_;
  std::vector<Relationship*> relationships_;
  std::vector<std::string> fullyQualified_;  // This model inclusive

  // used to by copied at the end of parse so during code generation step
  // can be used to lookup type names by xmi id
  std::unordered_map<std::string, std::vector<std::string>> idNameMap_;

  ModelNode(char* name, char* id, std::vector<std::string> fullyQualified) : name_(name), id_(id), fullyQualified_(fullyQualified) {}

  inline void addPackageImport(PackageImport* packageImport) { packageImports_.push_back(packageImport); }

  inline void addPackage(Package* package) { packages_.push_back(package); }

  inline void addModule(ModuleNode* module) { modules_.push_back(module); }

  inline void addRelationship(Relationship* relationship) { relationships_.push_back(relationship); }

  void generate(std::ostream& os) final {
    os << "Called Model Generate for Model Node: " << std::endl;
    os << *this << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& os, const ModelNode node) {
    os << "Model Name: " << node.name_ << std::endl;
    os << "Model Id: " << node.id_ << std::endl;
    return os;
  }
};
}  // namespace XMR