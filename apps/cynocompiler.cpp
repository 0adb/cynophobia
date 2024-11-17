#include <cynophobia/lexer.hpp>
#include <cynophobia/shared.hpp> 

#include <iostream>
#include <unordered_map>


/*
Expects argc at least 2 and up to 4. 
First: name of the executable. 
Second argument: filename for output of C program after GCC preprocessor. 
Optional arguments must appear in this order: 
- [if present] --debug, which will print intermediate outputs to stdout.
- [if present] --lex | --parse | --codegen, which will halt compilation after lexer, parser, and code generationn

*/
int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 4) {
        return 1; 
    }

    std::string filename(argv[1]);  
    bool debug = false; 
    Target target = LinkStage; 
    
    if (argc > 2) {
        std::string debug_flag{"--debug"};
        std::unordered_map<std::string, Target> options = {
            { "--lex", LexStage},
            { "--parse", ParseStage},
            { "--codegen", CodegenStage}
        }; 

        std::string second(argv[2]);

        int stage_index = 0;
        if (debug_flag.compare(second) == 0) {
            debug = true; 
            if (argc == 4) { stage_index = 3; }
        } else {
            stage_index = 2;
        }

        if (stage_index != 0) {
            std::string flag(argv[stage_index]); 
            auto entry = options.find(flag); 
            if (entry != options.end()) {
                target = entry->second; 
            }
        }
    }

    const Config& config = { filename, debug };
    LexerOutput lexer_output = lex(config);
    if (lexer_output.open_failed) {
        if (debug) {
            printf("%s: error: file open failed\n", config.filename.c_str());
        } 
        return 1;
    } else if (lexer_output.read_failed) {
        if (debug) {
            printf("%s: error: reading file filed\n", config.filename.c_str());
        } 
        return 1;
    } else if (lexer_output.unknown_tokens.size() != 0) {
        if (debug) {
            for (const UnknownToken& u: lexer_output.unknown_tokens) {
                printf("%s:%s: error: unrecognized token %s\n", 
                  config.filename.c_str(),
                  u.position.debug_string().c_str(),
                  u.text.c_str());
            }   
        }
        return 1;
    }
    return 0;
}