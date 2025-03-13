/**********************************************************
 *
 * @filename: PapyrusParser.hpp
 * @brief:
 *
 ***********************************************************/
#include <string>
#include <unordered_map>

#include "parsers/IParser.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/XMLChar.hpp"

#define Error nullptr

namespace XMR {

class PapyrusParser : public IParser {
  xercesc::XercesDOMParser* parser_ = nullptr;
  xercesc::ErrorHandler* errHandler_ = nullptr;

  // const char* packageElementTag_ = "packagedElement";
  XMLCh* idKey_;
  XMLCh* typeKey_;
  XMLCh* nameKey_;
  XMLCh* visibilityKey_;
  XMLCh* attributeTypeKey_;
  XMLCh* hrefKey_;
  XMLCh* paramKey_;

  // Various values returned from "xmi:type" key in XML DOM
  std::string packageType_ = "uml:Package";
  std::string packageImportType_ = "uml:PackageImport";
  std::string classType_ = "uml:Class";
  std::string interactionType_ = "uml:Interaction";
  std::string associationType_ = "uml:Association";
  std::string propertyType_ = "uml:Property";
  std::string operationType_ = "uml:Operation";
  std::string primitiveType_ = "uml:PrimitiveType";

  enum UmlType {
    PACKAGE,
    PACKAGE_IMPORT,
    CLASS,
    INTERACTION,
    ASSOCIATION,
    PROPERTY,
    OPERATION,
    PRIMITIVE
  };
  std::unordered_map<std::string, UmlType> umlStringIdMap_ = {
      {packageType_, UmlType::PACKAGE},
      {packageImportType_, UmlType::PACKAGE_IMPORT},
      {classType_, UmlType::CLASS},
      {interactionType_, UmlType::INTERACTION},
      {associationType_, UmlType::ASSOCIATION},
      {propertyType_, UmlType::PROPERTY},
      {operationType_, UmlType::OPERATION},
      {primitiveType_, UmlType::PRIMITIVE}};

  // used to store XMI id to name mapping to be used when
  // user defined modules are also used for attributes and param types.
  std::unordered_map<std::string, std::string> idNameMap_;

  ModelNode* parseModel(xercesc::DOMNode* model);
  Package* parsePackage(xercesc::DOMElement* package);
  ModuleNode* parseModule(xercesc::DOMElement* module);
  Operator* parseOperator(xercesc::DOMElement* op);
  Attribute* parseAttribute(xercesc::DOMElement* attribute);

 public:
  // Constructor
  PapyrusParser();

  // Destructor
  ~PapyrusParser();

  bool setInputFile(const char* fileName) final;

  // Main parse function
  ModelNode* parse() final;
};

}  // namespace XMR
