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
  generalizationAttrKey_ = XMLString::transcode("generalization");
  generalKey_ = XMLString::transcode("general");
  lowerValueAttrKey_ = XMLString::transcode("lowerValue");
  upperValueAttrKey_ = XMLString::transcode("upperValue");
  valueKey_ = XMLString::transcode("value");
}

// Destructor
PapyrusParser::~PapyrusParser() {
  delete parser_;
  delete errHandler_;
  XMLString::release(&idKey_);
  XMLString::release(&typeKey_);

  XMLPlatformUtils::Terminate();
}

bool PapyrusParser::setInputFile(const char* fileName) {
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
  if (model->getNodeType() != DOMNode::NodeType::ELEMENT_NODE) {
    cerr << "Model must be DOM element" << endl;
    return nullptr;
  }
  DOMElement* modelDomElement = static_cast<DOMElement*>(model);
  char* modelName = XMLString::transcode(modelDomElement->getAttribute(nameKey_));
  char* modelId = XMLString::transcode(modelDomElement->getAttribute(idKey_));
  currentScope_.push_back(modelName);
  ModelNode* modelNode = new ModelNode(modelName, modelId, currentScope_);

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
          idNameMap_[moduleNode->id_] = moduleNode->fullyQualified_;

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
  modelNode->idNameMap_ = this->idNameMap_;

  currentScope_.pop_back();

  return modelNode;
}

//
Package* PapyrusParser::parsePackage(xercesc::DOMElement* package) {
  char* packageName = XMLString::transcode(package->getAttribute(nameKey_));
  char* packageId = XMLString::transcode(package->getAttribute(idKey_));
  currentScope_.push_back(packageName);
  Package* packageNode = new Package(packageName, packageId, currentScope_);

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
          idNameMap_[moduleNode->id_] = moduleNode->fullyQualified_;
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
  currentScope_.pop_back();

  return packageNode;
}

ModuleNode* PapyrusParser::parseModule(xercesc::DOMElement* mod) {
  char* moduleName = XMLString::transcode(mod->getAttribute(nameKey_));
  char* moduleId = XMLString::transcode(mod->getAttribute(idKey_));
  const XMLCh* visAtt = mod->getAttribute(visibilityKey_);
  char* visibility;
  if (visAtt != nullptr) {
    visibility = XMLString::transcode(visAtt);
  }
  currentScope_.push_back(moduleName);
  ModuleNode* moduleNode;
  if (visibility == nullptr)
    moduleNode = new ModuleNode(moduleName, moduleId, currentScope_);
  else {
    // Double check its private first
    //!@todo: Do we handle protected?
    if (strcmp(visibility, "private") == 0) {
      moduleNode = new ModuleNode(moduleName, moduleId, currentScope_, Visibility::PRIVATE);
    } else {
      moduleNode = new ModuleNode(moduleName, moduleId, currentScope_);
    }
  }
  moduleNode->qualifiedName_ = currentScope_;  // save path to module
  XMLString::release(&visibility);

  // Grab children
  DOMNodeList* nodes = mod->getChildNodes();

  if (nodes != nullptr) {
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

        case UmlType::GENERALIZATION: {
          char* generalType = XMLString::transcode(domElement->getAttribute(generalKey_));
          moduleNode->addGeneralization(generalType);
        } break;

        default:
          cout << "UML Type Unimplemented: " << umlStringIdMap_[type] << endl;
          break;
      }
      mod->removeChild(node)->release();
      XMLString::release(&type);
    }
  }
  currentScope_.pop_back();

  return moduleNode;
}

Operator* PapyrusParser::parseOperator(xercesc::DOMElement* op) {
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
      operatorNode = new Operator(operatorName, operatorId, Visibility::PRIVATE);
    } else if (strcmp(visibility, "protected") == 0) {
      operatorNode = new Operator(operatorName, operatorId, Visibility::PROTECTED);
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
      Param* returnNode = nullptr;
      if (!param->hasAttribute(attributeTypeKey_)) {
        // Check worst case no type associated return nullptr!
        if (param->getElementsByTagName(attributeTypeKey_)->getLength() <= 0) {
          cerr << "No type associated with operator parameter: " << (name == nullptr ? "return" : name) << " param Id: " << id << " for operator: " << operatorName << endl;
          return nullptr;
        }
        // primitive type
        DOMElement* typeDomElement = (DOMElement*)param->getElementsByTagName(attributeTypeKey_)->item(0);
        typeNode = new Type(XMLString::transcode(typeDomElement->getAttribute(hrefKey_)), true);
      } else {
        typeNode = new Type(XMLString::transcode(param->getAttribute(attributeTypeKey_)));
      }
      if (std::strcmp(direction, "return") == 0) {
        returnNode = new Param(name, id, typeNode);
      } else if (std::strcmp(direction, "out") == 0) {
        // reference/pointer
        paramNode = new Param(name, id, typeNode, Direction::OUT);
      } else {
        // By copy
        paramNode = new Param(name, id, typeNode);
      }

      // Check if its a param node as these have multiplicity tags!
      if (paramNode) {
        DOMNodeList* lowerBound = param->getElementsByTagName(lowerValueAttrKey_);

        // Check for lower bounds
        if (lowerBound && lowerBound->getLength() > 0) {
          if (lowerBound->getLength() != 1) {
            cerr << "Params can only support 1 lower bound!";
            return nullptr;
          }

          DOMElement* lowerBoundNode = (DOMElement*)lowerBound->item(0);
          char* lowerValue = XMLString::transcode(lowerBoundNode->getAttribute(valueKey_));
          string lowerValueString = lowerValue;
          if (!lowerValueString.empty()) {
            paramNode->nilable_ = false;
          } else {
            paramNode->nilable_ = true;
          }
          XMLString::release(&lowerValue);
        } else {
          paramNode->nilable_ = false;
        }

        // Check upper bound
        DOMNodeList* upperBound = param->getElementsByTagName(upperValueAttrKey_);
        if (upperBound && upperBound->getLength() > 0) {
          if (upperBound->getLength() != 1) {
            cerr << "Params  can only support 1 upper bound!";
            return nullptr;
          }

          DOMElement* upperBoundNode = (DOMElement*)upperBound->item(0);
          char* upperValue = XMLString::transcode(upperBoundNode->getAttribute(valueKey_));
          string value = upperValue;
          if (value == "*") {
            paramNode->unlimited_ = true;
          } else {
            paramNode->unlimited_ = false;
            paramNode->multiplicity_ = atoi(upperValue);
          }
          XMLString::release(&upperValue);
        } else {
          paramNode->unlimited_ = false;
        }

        operatorNode->addParam(paramNode);
      }

      // Check if its a return node as these have multiplicity tags!
      if (returnNode) {
        DOMNodeList* lowerBound = param->getElementsByTagName(lowerValueAttrKey_);

        // Check for lower bounds
        if (lowerBound && lowerBound->getLength() > 0) {
          if (lowerBound->getLength() != 1) {
            cerr << "Return node can only support 1 lower bound!";
            return nullptr;
          }

          DOMElement* lowerBoundNode = (DOMElement*)lowerBound->item(0);
          char* lowerValue = XMLString::transcode(lowerBoundNode->getAttribute(valueKey_));
          string lowerValueString = lowerValue;
          if (!lowerValueString.empty()) {
            returnNode->nilable_ = false;
          } else {
            returnNode->nilable_ = true;
          }
          XMLString::release(&lowerValue);
        } else {
          returnNode->nilable_ = false;
        }

        // Check upper bound
        DOMNodeList* upperBound = param->getElementsByTagName(upperValueAttrKey_);
        if (upperBound && upperBound->getLength() > 0) {
          if (upperBound->getLength() != 1) {
            cerr << "Return node  can only support 1 upper bound!";
            return nullptr;
          }

          DOMElement* upperBoundNode = (DOMElement*)upperBound->item(0);
          char* upperValue = XMLString::transcode(upperBoundNode->getAttribute(valueKey_));
          string value = upperValue;
          if (value == "*") {
            returnNode->unlimited_ = true;
          } else {
            returnNode->unlimited_ = false;
            returnNode->multiplicity_ = atoi(upperValue);
          }
          XMLString::release(&upperValue);
        } else {
          returnNode->unlimited_ = false;
        }

        operatorNode->addReturnType(returnNode);
      }
    }

    XMLString::release(&directionKey);
  }

  return operatorNode;
}

Attribute* PapyrusParser::parseAttribute(xercesc::DOMElement* attribute) {
  char* attributeName = XMLString::transcode(attribute->getAttribute(nameKey_));
  char* attributeId = XMLString::transcode(attribute->getAttribute(idKey_));
  char* visibility = XMLString::transcode(attribute->getAttribute(visibilityKey_));
  char* type = XMLString::transcode(attribute->getAttribute(attributeTypeKey_));
  Type* typeNode = nullptr;
  // If fail means primitive type
  if (!attribute->hasAttribute(attributeTypeKey_)) {
    // Check worst case no type associated return nullptr!
    if (attribute->getElementsByTagName(attributeTypeKey_)->getLength() <= 0) {
      cerr << "No type associated with property: " << attributeName << " property Id: " << attributeId << endl;
      return nullptr;
    }
    // primitive type
    DOMElement* typeDomElement = (DOMElement*)attribute->getElementsByTagName(attributeTypeKey_)->item(0);
    typeNode = new Type(XMLString::transcode(typeDomElement->getAttribute(hrefKey_)), true);

  } else {
    typeNode = new Type(XMLString::transcode(attribute->getAttribute(attributeTypeKey_)));
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
      attributeNode = new Attribute(attributeName, attributeId, typeNode, Visibility::PRIVATE);
    } else if (strcmp(visibility, "protected") == 0) {
      attributeNode = new Attribute(attributeName, attributeId, typeNode, Visibility::PROTECTED);
    } else {
      attributeNode = new Attribute(attributeName, attributeId, typeNode);
    }
  }

  DOMNodeList* lowerBound = attribute->getElementsByTagName(lowerValueAttrKey_);

  // Check for lower bounds
  if (lowerBound && lowerBound->getLength() > 0) {
    if (lowerBound->getLength() != 1) {
      cerr << "Attributes can only support 1 lower bound!";
      return nullptr;
    }

    DOMElement* lowerBoundNode = (DOMElement*)lowerBound->item(0);
    char* lowerValue = XMLString::transcode(lowerBoundNode->getAttribute(valueKey_));
    string lowerValueString = lowerValue;
    if (!lowerValueString.empty()) {
      attributeNode->nilable_ = false;
    } else {
      attributeNode->nilable_ = true;
    }
    XMLString::release(&lowerValue);
  } else {
    attributeNode->nilable_ = false;
  }

  // Check upper bound
  DOMNodeList* upperBound = attribute->getElementsByTagName(upperValueAttrKey_);
  if (upperBound && upperBound->getLength() > 0) {
    if (upperBound->getLength() != 1) {
      cerr << "Attributes can only support 1 upper bound!";
      return nullptr;
    }

    DOMElement* upperBoundNode = (DOMElement*)upperBound->item(0);
    char* upperValue = XMLString::transcode(upperBoundNode->getAttribute(valueKey_));
    string value = upperValue;
    if (value == "*") {
      attributeNode->unlimited_ = true;
    } else {
      attributeNode->unlimited_ = false;
      attributeNode->multiplicity_ = atoi(upperValue);
    }
    XMLString::release(&upperValue);
  } else {
    attributeNode->unlimited_ = false;
  }

  XMLString::release(&visibility);
  return attributeNode;
}

extern "C" IParser* create_parser() { return new PapyrusParser; }
extern "C" void destroy_parser(IParser* parser) { delete parser; }

}  // namespace XMR