/**********************************************************
 * 
 * @filename: XmiParser.h
 * @brief:
 * 
 ***********************************************************/
#include <string>
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"

using namespace xercesc;

class Parser {
        const char * schemaLocation = "xmi.xsd";
    public:
        std::string parse(){
            XMLPlatformUtils::Initialize();

            XercesDOMParser* parser = new xercesc::XercesDOMParser();
            parser->setExternalSchemaLocation(schemaLocation);

            XMLPlatformUtils::Terminate();

            return "true";
        }

}

