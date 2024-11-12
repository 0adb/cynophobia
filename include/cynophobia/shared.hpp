
#include <string>
#pragma once 

enum Target { lex_stage, parse_stage, codegen_stage, link_stage };

struct Config {
    std::string filename; 
    bool verbose;
};