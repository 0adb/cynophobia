#pragma once 
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

//// Cross-cutting

enum Target { LexStage, ParseStage, CodegenStage, LinkStage };

struct Config {
    std::string filename; 
    bool debug;
};

//// Lexing-related 
struct FilePosition {
    unsigned int line; 
    unsigned int column;
    std::string debug_string() const;
    FilePosition next_column();
    FilePosition start_next_line();
};

struct Token {
        // [a-zA-Z_] means any a to z character, A to Z character, or an underscore 
    // \w is [a-zA-Z0-9_]
    // \b is a word boundary, i.e. between a \w and a not \w 
    // \( and \) are literal parentheses
    enum TokenType {
        Identifier,   // [a-zA-Z_]\w*\b
        Constant,     // [0-9]+\b
        Int,          // int\b 
        Void,         // void\b 
        Return,       // return\b
        OpenParen,    // \(
        CloseParen,   // \)
        OpenBrace,    // \{
        CloseBrace,   // \}
        Semicolon,    //  ;
    };

    FilePosition position;
    size_t       text_index; 
    TokenType    token_type;  
    std::string debug_string(const std::vector<std::string> &texts) const;
    const std::unordered_map<Token::TokenType, const char*> sole_value =
    { { Token::Int, "int" }, 
    { Token::Void, "void" }, 
    { Token::Return, "return" }, 
    { Token::OpenParen, "("}, 
    { Token::CloseParen, ")"}, 
    { Token::OpenBrace, "{" },
    { Token::CloseBrace, "}" }, 
    { Token::Semicolon, ";" } };
};



struct UnknownToken {
    FilePosition position; 
    std::string  text; 
    std::string debug_string() const;
};

struct LexerOutput {
    const std::vector<std::string> texts; 
    const std::vector<Token> tokens; 
    const std::vector<UnknownToken> unknown_tokens; 
    const bool read_failed;
    const bool open_failed;
    std::string debug_string() const; 
};
