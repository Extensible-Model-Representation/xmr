/**********************************************************
 *
 * @filename: PapyrusParser.cpp
 * @brief:
 *
 ***********************************************************/
#include "parsers/PapyrusParser.hpp"

#include <filesystem>
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace xercesc;
using namespace std;

namespace XMR {

// Constructor
PapyrusParser::PapyrusParser() {
  try {
    XMLPlatformUtils::Initialize();

  } catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cerr << "Error during platform init: " << message << endl;
    XMLString::release(&message);
  }

  parser_ = new XercesDOMParser();
  errHandler_ = new HandlerBase();
  parser_->setErrorHandler(errHandler_);

  parser_->setValidationScheme(XercesDOMParser::Val_Always);
  parser_->setDoNamespaces(true);
  parser_->setLoadSchema(true);
  //!@todo: Find elegant way of loading schema file in project
  // assert(parser_->loadGrammar(schemaLocation_.c_str(),
  //                             Grammar::GrammarType::SchemaGrammarType,
  //                             true) != nullptr);
  parser_->setDoSchema(true);

  idKey_ = XMLString::transcode("xmi:id");
  typeKey_ = XMLString::transcode("xmi:type");
  nameKey_ = XMLString::transcode("name");
  visibilityKey_ = XMLString::transcode("visibility");
  attributeTypeKey_ = XMLString::transcode("type");
  hrefKey_ = XMLString::transcode("href");
  paramKey_ = XMLString::transcode("ownedParameter");
}

// Destructor
PapyrusParser::~PapyrusParser() {
  delete parser_;
  delete errHandler_;
  XMLString::release(&idKey_);
  XMLString::release(&typeKey_);

  XMLPlatformUtils::Terminate();
}

bool PapyrusParser::setInputFile(char* fileName) {
  if (!filesystem::exists(fileName)) return false;

  try {
    parser_->parse(fileName);
  } catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cerr << "XML Parser Error. Message: " << message << endl;
    XMLString::release(&message);
    return false;
  }

  catch (const DOMException& toCatch) {
    char* message = XMLString::transcode(toCatch.msg);
    cerr << "XML Parser Error. Message: " << message << endl;
    XMLString::release(&message);
    return false;
  }

  catch (const SAXParseException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cerr << "XML Parser Error. Message: " << message << endl;
    XMLString::release(&message);
    return false;
  }

  catch (...) {
    cerr << "Unexpected Exception When Loading XML File Into DOM \n";
  }

  return true;
}

ModelNode* PapyrusParser::parse() {
  DOMDocument* doc = parser_->getDocument();
  if (doc == nullptr) {
    cerr << "Failed to get DOM doc" << endl;
    return nullptr;
  }

  // List of UML model nodes at the root of the DOM doc
  DOMNodeList* nodes = doc->getChildNodes();
  if (nodes == nullptr) {
    cerr << "No Model in document" << endl;
    return nullptr;
  }

  //!@todo: Can we make this generate multi programs?
  // We only support 1 model at a time as of 2025/02/26
  if (nodes->getLength() < 1) {
    cerr << "Only support for 1 model in an XMI file, num currently in file "
            "is: ";
    cerr << nodes->getLength() << endl;
    return nullptr;
  }

  return parseModel(nodes->item(0));
}

ModelNode* PapyrusParser::parseModel(DOMNode* model) {
  cout << "Model" << endl;
  if (model->getNodeType() != DOMNode::NodeType::ELEMENT_NODE) {
    cerr << "Model must be DOM element" << endl;
    return nullptr;
  }
  DOMElement* modelDomElement = static_cast<DOMElement*>(model);
  char* modelName =
      XMLString::transcode(modelDomElement->getAttribute(nameKey_));
  char* modelId = XMLString::transcode(modelDomElement->getAttribute(idKey_));

  ModelNode* modelNode = new ModelNode(modelName, modelId);

  // Grab children
  DOMNodeList* nodes = modelDomElement->getChildNodes();

  if (nodes != nullptr) {
    // Loop through children of the model
    while (nodes->getLength() > 0) {
      DOMNode* node = nodes->item(0);

      // Unsure why DOM has empty text nodes layered in teh children nodes of
      // the model node?
      if (node->getNodeType() == DOMNode::NodeType::TEXT_NODE) {
        modelDomElement->removeChild(node)->release();
#ifdef DEBUG
        cout << "Skipping Text Node!" << endl;
#endif
        continue;
      }

      if (node->getNodeType() != DOMNode::NodeType::ELEMENT_NODE) {
        cerr << "Model children must be DOM element. Element was of type: ";
        cerr << node->getNodeType() << endl;
        return nullptr;
      }

      DOMElement* domElement = static_cast<DOMElement*>(node);
      char* type = XMLString::transcode(domElement->getAttribute(typeKey_));
      if (type == nullptr) {
        cerr << "Model children nodes must have attributes." << endl;
        return nullptr;
      }

      switch (umlStringIdMap_[type]) {
        case UmlType::CLASS: {
          ModuleNode* moduleNode = parseModule(domElement);
          if (moduleNode == nullptr) {
            cerr << "Failed to parse module" << endl;
            return nullptr;
          }
          modelNode->addModule(moduleNode);

        } break;
        case UmlType::PACKAGE: {
          Package* packageNode = parsePackage(domElement);
          if (packageNode == nullptr) {
            cerr << "Failed to parse package" << endl;
            return nullptr;
          }
          modelNode->addPackage(packageNode);

        } break;

        default:
          cout << "UML Type Unimplemented: " << umlStringIdMap_[type] << endl;
          break;
      }
      modelDomElement->removeChild(node)->release();
      XMLString::release(&type);
    }
  }
  cout << *modelNode;
  return modelNode;
}

Package* PapyrusParser::parsePackage(xercesc::DOMElement* package) {
  cout << "Package" << endl;
  char* packageName = XMLString::transcode(package->getAttribute(nameKey_));
  char* packageId = XMLString::transcode(package->getAttribute(idKey_));

  Package* packageNode = new Package(packageName, packageId);

  // Grab children
  DOMNodeList* nodes = package->getChildNodes();

  if (nodes != nullptr) {
    // Loop through children of the package
    while (nodes->getLength() > 0) {
      DOMNode* node = nodes->item(0);

      // Unsure why DOM has empty text nodes layered in the children nodes?
      if (node->getNodeType() == DOMNode::NodeType::TEXT_NODE) {
        package->removeChild(node)->release();
#ifdef DEBUG
        cout << "Skipping Text Node!" << endl;
#endif
        continue;
      }

      if (node->getNodeType() != DOMNode::NodeType::ELEMENT_NODE) {
        cerr << "Package children must be DOM element. Element was of type: ";
        cerr << node->getNodeType() << endl;
        return nullptr;
      }

      DOMElement* domElement = static_cast<DOMElement*>(node);
      char* type = XMLString::transcode(domElement->getAttribute(typeKey_));
      if (type == nullptr) {
        cerr << "Package children nodes must have attributes." << endl;
        return nullptr;
      }

      switch (umlStringIdMap_[type]) {
        case UmlType::CLASS: {
          ModuleNode* moduleNode = parseModule(domElement);
          if (moduleNode == nullptr) {
            cerr << "Failed to parse module" << endl;
            return nullptr;
          }
          packageNode->addModule(moduleNode);
        } break;
        case UmlType::PACKAGE: {
          Package* packageNode = parsePackage(domElement);
          if (packageNode == nullptr) {
            cerr << "Failed to parse package" << endl;
            return nullptr;
          }
          packageNode->addPackage(packageNode);

        } break;

        default:
          cout << "UML Type Unimplemented: " << umlStringIdMap_[type] << endl;
          break;
      }
      package->removeChild(node)->release();
      XMLString::release(&type);
    }
  }
  cout << *packageNode;
  return packageNode;
}

ModuleNode* PapyrusParser::parseModule(xercesc::DOMElement* mod) {
  cout << "Module" << endl;
  char* moduleName = XMLString::transcode(mod->getAttribute(nameKey_));
  char* moduleId = XMLString::transcode(mod->getAttribute(idKey_));
  const XMLCh* visAtt = mod->getAttribute(visibilityKey_);

  char* visibility;
  if (visAtt != nullptr) {
    visibility = XMLString::transcode(visAtt);
  }

  ModuleNode* moduleNode;

  if (visibility == nullptr)
    moduleNode = new ModuleNode(moduleName, moduleId);
  else {
    // Double check its private first
    //!@todo: Do we handle protected?
    if (strcmp(visibility, "private") == 0) {
      moduleNode = new ModuleNode(moduleName, moduleId, Visibility::PRIVATE);
    } else {
      moduleNode = new ModuleNode(moduleName, moduleId);
    }
  }
  XMLString::release(&visibility);

  // Grab children
  DOMNodeList* nodes = mod->getChildNodes();

  if (nodes != nullptr) {
    // Loop through children of the package
    while (nodes->getLength() > 0) {
      DOMNode* node = nodes->item(0);

      // Unsure why DOM has empty text nodes layered in teh children nodes?
      if (node->getNodeType() == DOMNode::NodeType::TEXT_NODE) {
        mod->removeChild(node)->release();
#ifdef DEBUG
        cout << "Skipping Text Node!" << endl;
#endif
        continue;
      }

      if (node->getNodeType() != DOMNode::NodeType::ELEMENT_NODE) {
        cerr << "Module children must be DOM element. Element was of type : ";
        cerr << node->getNodeType() << endl;
        return nullptr;
      }

      DOMElement* domElement = static_cast<DOMElement*>(node);
      char* type = XMLString::transcode(domElement->getAttribute(typeKey_));
      if (type == nullptr) {
        cerr << "Module children nodes must have attributes." << endl;
        return nullptr;
      }

      switch (umlStringIdMap_[type]) {
        case UmlType::OPERATION: {
          Operator* operatorNode = parseOperator(domElement);
          if (operatorNode == nullptr) {
            cerr << "Failed to parse operator" << endl;
            return nullptr;
          }
          moduleNode->addOperator(operatorNode);
        } break;
        case UmlType::PROPERTY: {
          Attribute* attributeNode = parseAttribute(domElement);
          if (attributeNode == nullptr) {
            cerr << "Failed to parse attribute" << endl;
            return nullptr;
          }
          moduleNode->addAttribute(attributeNode);

        } break;

        default:
          cout << "UML Type Unimplemented: " << umlStringIdMap_[type] << endl;
          break;
      }
      mod->removeChild(node)->release();
      XMLString::release(&type);
    }
  }
  cout << *moduleNode;
  return moduleNode;
}

Operator* PapyrusParser::parseOperator(xercesc::DOMElement* op) {
  cout << "Operator" << endl;
  char* operatorName = XMLString::transcode(op->getAttribute(nameKey_));
  char* operatorId = XMLString::transcode(op->getAttribute(idKey_));
  char* visibility = XMLString::transcode(op->getAttribute(visibilityKey_));
  Operator* operatorNode;
  if (visibility == nullptr)
    operatorNode = new Operator(operatorName, operatorId);
  else {
    // Double check its private first, don't need to assign public as
    // public
    // is set by default.
    //!@todo: Do we handle protected?
    if (strcmp(visibility, "private") == 0) {
      operatorNode =
          new Operator(operatorName, operatorId, Visibility::PRIVATE);
    } else {
      operatorNode = new Operator(operatorName, operatorId);
    }
  }
  XMLString::release(&visibility);

  DOMNodeList* params = op->getElementsByTagName(paramKey_);
  if (params->getLength() != 0) {
    XMLCh* directionKey = XMLString::transcode("direction");
    for (size_t i = 0; i < params->getLength(); i++) {
      DOMElement* param = (DOMElement*)params->item(i);
      char* direction = XMLString::transcode(param->getAttribute(directionKey));
      char* id = XMLString::transcode(param->getAttribute(idKey_));
      char* name = XMLString::transcode(param->getAttribute(nameKey_));
      Type* typeNode = nullptr;
      Param* paramNode = nullptr;
      if (!param->hasAttribute(attributeTypeKey_)) {
        // primitive type
        DOMElement* typeDomElement =
            (DOMElement*)param->getElementsByTagName(attributeTypeKey_)
                ->item(0);
        typeNode = new Type(
            XMLString::transcode(typeDomElement->getAttribute(hrefKey_)), true);
      } else {
        typeNode = new Type(
            XMLString::transcode(param->getAttribute(attributeTypeKey_)));
      }
      if (std::strcmp(direction, "return") == 0) {
        operatorNode->addReturnType(typeNode);

      } else if (std::strcmp(direction, "out") == 0) {
        // reference/pointer
        paramNode = new Param(name, id, typeNode, Direction::OUT);
        operatorNode->addParam(paramNode);
      } else {
        // By copy
        paramNode = new Param(name, id, typeNode);
        operatorNode->addParam(paramNode);
      }
    }
    XMLString::release(&directionKey);
  }

  cout << *operatorNode;
  return operatorNode;
}

Attribute* PapyrusParser::parseAttribute(xercesc::DOMElement* attribute) {
  cout << "Attribute" << endl;
  char* attributeName = XMLString::transcode(attribute->getAttribute(nameKey_));
  char* attributeId = XMLString::transcode(attribute->getAttribute(idKey_));
  char* visibility =
      XMLString::transcode(attribute->getAttribute(visibilityKey_));
  char* type = XMLString::transcode(attribute->getAttribute(attributeTypeKey_));
  Type* typeNode = nullptr;
  // If fail means primitive type
  if (!attribute->hasAttribute(attributeTypeKey_)) {
    // primitive type
    DOMElement* typeDomElement =
        (DOMElement*)attribute->getElementsByTagName(attributeTypeKey_)
            ->item(0);
    typeNode = new Type(
        XMLString::transcode(typeDomElement->getAttribute(hrefKey_)), true);

  } else {
    typeNode = new Type(
        XMLString::transcode(attribute->getAttribute(attributeTypeKey_)));
  }

  Attribute* attributeNode;
  if (visibility == nullptr)
    attributeNode = new Attribute(attributeName, attributeId, typeNode);
  else {
    // Double check its private first, don't need to assign public as
    // public
    // is set by default.
    //!@todo: Do we handle protected?
    if (strcmp(visibility, "private") == 0) {
      attributeNode = new Attribute(attributeName, attributeId, typeNode,
                                    Visibility::PRIVATE);
    } else {
      attributeNode = new Attribute(attributeName, attributeId, typeNode);
    }
  }
  XMLString::release(&visibility);
  cout << *attributeNode;
  return attributeNode;
}
extern "C" IParser* create_parser() { return new PapyrusParser; }
extern "C" void destroy_parser(IParser* parser) { delete parser; }

}  // namespace XMR