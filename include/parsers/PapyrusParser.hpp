/**********************************************************
 * Copyright 2025 Jason Cisneros & Lucas Van Der Heijden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @filename: PapyrusParser.hpp
 * @brief:
 *
 ***********************************************************/
#include <string>
#include <unordered_map>
#include <vector>

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
  XMLCh* generalizationAttrKey_;
  XMLCh* generalKey_;
  XMLCh* lowerValueAttrKey_;
  XMLCh* upperValueAttrKey_;
  XMLCh* valueKey_;
  // Various values returned from "xmi:type" key in XML DOM
  std::string packageType_ = "uml:Package";
  std::string packageImportType_ = "uml:PackageImport";
  std::string classType_ = "uml:Class";
  std::string interactionType_ = "uml:Interaction";
  std::string associationType_ = "uml:Association";
  std::string propertyType_ = "uml:Property";
  std::string operationType_ = "uml:Operation";
  std::string primitiveType_ = "uml:PrimitiveType";
  std::string generalType_ = "uml:Generalization";

  enum UmlType { PACKAGE, PACKAGE_IMPORT, CLASS, INTERACTION, ASSOCIATION, PROPERTY, OPERATION, PRIMITIVE, GENERALIZATION };
  std::unordered_map<std::string, UmlType> umlStringIdMap_ = {{packageType_, UmlType::PACKAGE},         {packageImportType_, UmlType::PACKAGE_IMPORT}, {classType_, UmlType::CLASS},
                                                              {interactionType_, UmlType::INTERACTION}, {associationType_, UmlType::ASSOCIATION},      {propertyType_, UmlType::PROPERTY},
                                                              {operationType_, UmlType::OPERATION},     {primitiveType_, UmlType::PRIMITIVE},          {generalType_, UmlType::GENERALIZATION}};

  // used to store XMI id to name mapping to be used when
  // user defined modules are also used for attributes and param types.
  std::unordered_map<std::string, std::vector<std::string>> idNameMap_;
  std::vector<std::string> currentScope_;

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
