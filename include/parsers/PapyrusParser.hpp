/**********************************************************
 *
 * @filename: PapyrusParser.hpp
 * @brief:
 *
 ***********************************************************/
#include <string>

#include "parsers/IParser.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"

#define Error nullptr

namespace XMR {

class PapyrusParser : public IParser {
  std::string schemaLocation_;
  const char* packageElementTag_ = "packagedElement";
  const char* idKey_ = "xmi:id";
  const char* typeKey_ = "xmi:type";

  xercesc::XercesDOMParser* parser_ = nullptr;
  xercesc::ErrorHandler* errHandler_ = nullptr;

  ModelNode* parseModel(xercesc::DOMNode* model);

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
