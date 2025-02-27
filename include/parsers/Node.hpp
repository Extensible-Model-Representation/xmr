/**********************************************************
 *
 * @filename: Node.hpp
 * @brief:
 *
 ***********************************************************/
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

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

class PackageImport : public Node {};

class Package : public Node {};

class Module : public Node {};

class Relationship : public Node {};

class Operator : public Node {};

class Attribute : public Node {};

class ModelNode : public Node {
  std::string name_;
  std::string id_;
  std::vector<PackageImport*> packageImports_;
  std::vector<Package*> packages_;
  std::vector<Module*> modules_;
  std::vector<Relationship*> relationships_;

 public:
  ModelNode(std::string name, std::string id) : name_(name), id_(id) {}

  inline void addPackageImport(PackageImport* packageImport) {
    packageImports_.push_back(packageImport);
  }

  inline void addPackage(Package* package) { packages_.push_back(package); }

  inline void addModule(Module* module) { modules_.push_back(module); }

  inline void addRelationship(Relationship* relationship) {
    relationships_.push_back(relationship);
  }

  void generate(std::ostream& os) final {
    os << *this;

    os << "//Generate Packages" << std::endl;
    for (size_t i = 0; i < packages_.size(); i++) {
      packages_[i]->generate(os);
      os << std::endl;
    }

    os << "//Generate Package Imports" << std::endl;
    for (size_t i = 0; i < packages_.size(); i++) {
      packageImports_[i]->generate(os);
      os << std::endl;
    }

    os << "//Generate Modules" << std::endl;
    for (size_t i = 0; i < packages_.size(); i++) {
      modules_[i]->generate(os);
      os << std::endl;
    }

    os << "//Generate Relationships" << std::endl;
    for (size_t i = 0; i < packages_.size(); i++) {
      relationships_[i]->generate(os);
      os << std::endl;
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const ModelNode node) {
    os << "Model Name: " << node.name_ << std::endl;
    os << "Model Id: " << node.id_ << std::endl;
    return os;
  }
};
}  // namespace XMR