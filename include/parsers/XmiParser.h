/**********************************************************
 * 
 * @filename: XmiParser.h
 * @brief:
 * 
 ***********************************************************/
#include <string>
#include <iostream>
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"

using namespace xercesc;

class Parser {
        const char * schemaLocation = "xmi.xsd";
    public:
        std::string parse(){
            XMLPlatformUtils::Initialize();


            XMLPlatformUtils::Terminate();

            return "true";
        }

};

