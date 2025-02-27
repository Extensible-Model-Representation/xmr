/**********************************************************
 *
 * @filename: Node.hpp
 * @brief:
 *
 ***********************************************************/
#include <cstddef>
#include <cstring>
#include <ostream>
#include <string>
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

class PackageImport : public Node {};

class Relationship : public Node {};

class Operator : public Node {
  char name_[MAX_STRING_SIZE];
  char id_[MAX_STRING_SIZE];
  Visibility visibility_;

 public:
  Operator(char* name, char* id, Visibility visibility = Visibility::PUBLIC)
      : visibility_(visibility) {
    strncpy(name_, name, MAX_STRING_SIZE);
    strncpy(id_, id, MAX_STRING_SIZE);
  }

  void generate(std::ostream& os) final {
    os << "Called Operator Generate for Operator Node: " << std::endl;
    os << *this << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& os, const Operator node) {
    os << "Operator Name: " << node.name_ << std::endl;
    os << "Operator Id: " << node.id_ << std::endl;
    os << "Operator Visiblity: " << node.visibility_ << std::endl;
    return os;
  }
};

class Attribute : public Node {
  char name_[MAX_STRING_SIZE];
  char id_[MAX_STRING_SIZE];
  char type_[MAX_STRING_SIZE];
  Visibility visibility_;

 public:
  Attribute(char* name, char* id, char* type,
            Visibility visibility = Visibility::PUBLIC)
      : visibility_(visibility) {
    strncpy(name_, name, MAX_STRING_SIZE);
    strncpy(id_, id, MAX_STRING_SIZE);
    strncpy(type_, type, MAX_STRING_SIZE);
  }

  void generate(std::ostream& os) final {
    os << "Called Attribute Generate for Attribute Node: " << std::endl;
    os << *this << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& os, const Attribute node) {
    os << "Attribute Name: " << node.name_ << std::endl;
    os << "Attribute Id: " << node.id_ << std::endl;
    os << "Attribute Type: " << node.type_ << std::endl;
    os << "Attribute Visiblity: " << node.visibility_ << std::endl;
    return os;
  }
};

class ModuleNode : public Node {
  char name_[MAX_STRING_SIZE];
  char id_[MAX_STRING_SIZE];
  Visibility visibility_;

  //!@todo: May need to store these in two different vectors based on visiblity?
  std::vector<Operator*> operators_;
  std::vector<Attribute*> attributes_;

 public:
  //!@todo: Do we want to default visibility if not set? Will it never be not
  //! set in the metadata?
  ModuleNode(char* name, char* id, Visibility visibility = Visibility::PUBLIC)
      : visibility_(visibility) {
    strncpy(name_, name, MAX_STRING_SIZE);
    strncpy(id_, id, MAX_STRING_SIZE);
  }

  void addOperator(Operator* op) { operators_.push_back(op); }

  void addAttribute(Attribute* attribute) { attributes_.push_back(attribute); }

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
  char name_[MAX_STRING_SIZE];
  char id_[MAX_STRING_SIZE];
  std::vector<Package*> packages_;
  std::vector<ModuleNode*> modules_;
  std::vector<Relationship*> relationships_;

 public:
  Package(char* name, char* id) {
    strncpy(name_, name, MAX_STRING_SIZE);
    strncpy(id_, id, MAX_STRING_SIZE);
  }

  inline void addPackage(Package* package) { packages_.push_back(package); }

  inline void addModule(ModuleNode* module) { modules_.push_back(module); }

  inline void addRelationship(Relationship* relationship) {
    relationships_.push_back(relationship);
  }

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
  char name_[MAX_STRING_SIZE];
  char id_[MAX_STRING_SIZE];
  std::vector<PackageImport*> packageImports_;
  std::vector<Package*> packages_;
  std::vector<ModuleNode*> modules_;
  std::vector<Relationship*> relationships_;

 public:
  ModelNode(char* name, char* id) {
    strncpy(name_, name, MAX_STRING_SIZE);
    strncpy(id_, id, MAX_STRING_SIZE);
  }

  inline void addPackageImport(PackageImport* packageImport) {
    packageImports_.push_back(packageImport);
  }

  inline void addPackage(Package* package) { packages_.push_back(package); }

  inline void addModule(ModuleNode* module) { modules_.push_back(module); }

  inline void addRelationship(Relationship* relationship) {
    relationships_.push_back(relationship);
  }

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