/**********************************************************
 * 
 * @filename: XmiParser.h
 * @brief:
 * 
 ***********************************************************/
#include <string>
#include <iostream>
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace xercesc;

class Parser {
        const char * schemaLocation = "xmi.xsd";
    public:
        std::string parse(char *file){
            try{
                XMLPlatformUtils::Initialize();

            }catch(const XMLException& toCatch){
                char* message = XMLString::transcode(toCatch.getMessage());
                std::cout << "Error during platform init: " << message << std::endl;
                XMLString::release(&message);
            }

            XercesDOMParser* parser = new XercesDOMParser();
            parser->setValidationScheme(XercesDOMParser::Val_Always);
            parser->setDoNamespaces(true);

            ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
            parser->setErrorHandler(errHandler);

            char* xmiFile = file;

            try{
                parser->parse(xmiFile);
            }
            catch(const XMLException& toCatch){
                char* message = XMLString::transcode(toCatch.getMessage());
                std::cout << "XML Parser Error. Message: " << message << std::endl;
                XMLString::release(&message);
            }

            catch(const DOMException& toCatch){
                char* message = XMLString::transcode(toCatch.msg);
                std::cout << "XML Parser Error. Message: " << message << std::endl;
                XMLString::release(&message);
            }

            catch(...){
                std::cout << "Unexpected Exception \n";
            }

            DOMDocument* doc = parser->getDocument();
            assert(doc != nullptr);
            // DOMNodeIterator* iterator =  doc->createNodeIterator(doc, DOMNodeFilter::SHOW_ALL, nullptr, true);
            // assert(iterator != nullptr);
            // DOMNode* node = iterator->getRoot();
            // while (node != nullptr){
            //     std::cout << XMLString::transcode(node->getNodeName()) << std::endl;
            //     node = iterator->nextNode();
            // }
            DOMTreeWalker* iterator = doc->createTreeWalker(doc, DOMNodeFilter::SHOW_ALL, nullptr, true);
            DOMNode* node = iterator->getRoot();
            while (node != nullptr){
                // std::cout << "Name: " << XMLString::transcode(node->getNodeName()) << " Type:" << node->getNodeType() << std::endl;
                // node = iterator->nextNode();
                switch(node->getNodeType()){
                    case DOMNode::NodeType::ELEMENT_NODE: {
                        DOMElement* elementNode = static_cast<DOMElement*>(node);
                        std::cout << "Tag Value: " << XMLString::transcode(elementNode->getTagName()) << std::endl;
                        break;
                    }
                    default:
                        break;
                }
                node = iterator->nextNode();
            }
            iterator->release();
            // DOMNamedNodeMap* attributes =  doc->getAttributes();
            // assert(attributes != nullptr);
            // for(size_t i = 0; i < attributes->getLength(); i++){
            //     DOMNode* node = attributes->item(i);
            //     assert(node != nullptr);
            // }

            delete parser;
            delete errHandler;

            XMLPlatformUtils::Terminate();

            return "true";
        }

};

