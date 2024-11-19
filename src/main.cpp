/**********************************************************
 * 
 * @filename: main.cpp
 * @brief:
 * 
 ***********************************************************/
#include <iostream>
#include "parsers/XmiParser.h"

int main(int argc, char* argv[]){
    Parser parser;
    std::cout << parser.parse();
    return 0;
}