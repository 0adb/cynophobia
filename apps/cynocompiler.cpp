#include <cynophobia/lexer.hpp>
#include <cynophobia/shared.hpp>

#include <fmt/format.h> 
#include <iostream>
#include <unordered_map>


/*
Expects argc at least 2 and up to 4. 
First: name of the executable. 
Second argument: filename for output of C program after GCC preprocessor. 
Optional arguments must appear in this order: 
- [if present] --verbose, which will print intermediate outputs to stdout.
- [if present] --lex | --parse | --codegen, which will halt compilation after lexer, parser, and code generationn

*/
int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 4) {
        return 1; 
    }

    std::string filename(argv[1]);  
    bool verbose = false; 
    Target target = link_stage; 
    
    if (argc > 2) {
        std::string verbose_flag{"--verbose"};
        std::unordered_map<std::string, Target> options = {
            { "--lex", lex_stage},
            { "--parse", parse_stage},
            { "--codegen", codegen_stage}
        }; 

        std::string second(argv[2]);

        int stage_index = 0;
        if (verbose_flag.compare(second) == 0) {
            verbose = true; 
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

    std::unordered_map<Target, std::string> options = {
        { lex_stage, "--lex" },
        { parse_stage, "--parse" },
        { codegen_stage, "--codegen" },
        { link_stage, "[no stage]" }
    };  
    auto flag_read = options.find(target);
    
    if (flag_read == options.end()) {
        printf("stage received: undefined\n");
    } else {
        printf("stage received: %s\n", flag_read->second.c_str());
    }

    const Config& config = { filename, verbose };
    lex(config);
}