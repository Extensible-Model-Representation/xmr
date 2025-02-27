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
}

// Destructor
PapyrusParser::~PapyrusParser() {
  delete parser_;
  delete errHandler_;

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
  if (model->getNodeType() != DOMNode::NodeType::ELEMENT_NODE) {
    cerr << "Model must be DOM element" << endl;
    return nullptr;
  }
  DOMElement* modelDomElement = static_cast<DOMElement*>(model);
  std::string modelName = XMLString::transcode(
      modelDomElement->getAttribute(XMLString::transcode("name")));
  std::string modelId = XMLString::transcode(
      modelDomElement->getAttribute(XMLString::transcode(idKey_)));

  ModelNode* modelNode = new ModelNode(modelName, modelId);
  return modelNode;
}

}  // namespace XMR