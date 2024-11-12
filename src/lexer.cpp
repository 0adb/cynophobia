#include <cynophobia/lexer.hpp>
#include <cynophobia/shared.hpp>

void lex(const Config& config) { 
    printf("filename: %s\n", config.filename.c_str()); 
    printf("verbose: %s\n", config.verbose ? "yes" : "no");
}