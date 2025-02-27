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

#define Error nullptr

namespace XMR {

class PapyrusParser : public IParser {
  xercesc::XercesDOMParser* parser_ = nullptr;
  xercesc::ErrorHandler* errHandler_ = nullptr;

  std::string schemaLocation_;
  // const char* packageElementTag_ = "packagedElement";
  const char* idKey_ = "xmi:id";
  const char* typeKey_ = "xmi:type";

  // Various values returned from "xmi:type" key in XML DOM
  std::string packageType_ = "uml:Package";
  std::string packageImportType_ = "uml:PackageImport";
  std::string classType_ = "uml:Class";
  std::string interactionType_ = "uml:Interaction";
  std::string associationType_ = "uml:Association";
  std::string propertyType_ = "uml:Property";
  std::string operationType_ = "uml:Operation";

  enum UmlType {
    PACKAGE,
    PACKAGE_IMPORT,
    CLASS,
    INTERACTION,
    ASSOCIATION,
    PROPERTY,
    OPERATION
  };
  std::unordered_map<std::string, UmlType> umlStringIdMap_ = {
      {packageType_, UmlType::PACKAGE},
      {packageImportType_, UmlType::PACKAGE_IMPORT},
      {classType_, UmlType::CLASS},
      {interactionType_, UmlType::INTERACTION},
      {associationType_, UmlType::ASSOCIATION},
      {propertyType_, UmlType::PROPERTY},
      {operationType_, UmlType::OPERATION}};

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

  bool setInputFile(char* fileName) final;

  // Main parse function
  ModelNode* parse() final;
  // // DOMNodeIterator* iterator =  doc->createNodeIterator(doc,
  // // DOMNodeFilter::SHOW_ALL, nullptr, true); assert(iterator !=
  // nullptr);
  // // DOMNode* node = iterator->getRoot();
  // // while (node != nullptr){
  // //     std::cout << XMLString::transcode(node->getNodeName()) <<
  // std::endl;
  // //     node = iterator->nextNode();
  // // }
  // DOMTreeWalker* iterator =
  //     doc->createTreeWalker(doc, DOMNodeFilter::SHOW_ALL, nullptr, true);
  // DOMNode* node = iterator->getRoot();
  // while (node != nullptr) {
  //   // std::cout << "Name: " << XMLString::transcode(node->getNodeName())
  //   << "
  //   // Type:" << node->getNodeType() << std::endl; node =
  //   // iterator->nextNode();
  //   switch (node->getNodeType()) {
  //     case DOMNode::NodeType::ELEMENT_NODE: {
  //       DOMElement* elementNode = static_cast<DOMElement*>(node);
  //       const XMLCh* xmi_type =
  //           elementNode->getAttribute(XMLString::transcode(typeKey));
  //       const XMLCh* xmi_id =
  //           elementNode->getAttribute(XMLString::transcode(idKey));
  //       if (XMLString::equals(
  //               packageElementTag,
  //               XMLString::transcode(elementNode->getTagName()))) {
  //         if (xmi_id == nullptr) {
  //           std::cerr << "XMI Packaged Elements Require an Id" <<
  //           std::endl; return (Error);
  //         }

  //         if (xmi_type == nullptr) {
  //           std::cerr << "XMI Packaged Elements Require a type" <<
  //           std::endl; return (Error);
  //         }

  //         std::cout << "Packaged Element: XMI Id: "
  //                   << XMLString::transcode(xmi_id)
  //                   << " XMI Type: " << XMLString::transcode(xmi_type)
  //                   << std::endl;
  //       } else {
  //         std::cout << "Non-Packaged Element XMI Id: "
  //                   << XMLString::transcode(xmi_id)
  //                   << " XMI Type: " << XMLString::transcode(xmi_type)
  //                   << std::endl;
  //       }
  //       break;
  //     }
  //     default:
  //       std::cout << "Node Name: "
  //                 << XMLString::transcode(node->getNodeName())
  //                 << " Node Type: " << node->getNodeType() << std::endl;
  //       break;
  //   }
  //   node = iterator->nextNode();
  // }
  // iterator->release();
  // // DOMNamedNodeMap* attributes =  doc->getAttributes();
  // // assert(attributes != nullptr);
  // // for(size_t i = 0; i < attributes->getLength(); i++){
  // //     DOMNode* node = attributes->item(i);
  // //     assert(node != nullptr);
  // // }
};

}  // namespace XMR
