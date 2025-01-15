#pragma once 

#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <unordered_set> 
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
        Semicolon,    //  ;
        Identifier,   // [a-zA-Z_]\w*\b
        Constant,     // [0-9]+\b
        Int,          // int\b 
        Void,         // void\b 
        Return,       // return\b
        OpenParen,    // \(
        CloseParen,   // \)
        OpenBrace,    // \{
        CloseBrace,   // \}
    };

    FilePosition position; 
    std::string  text;
    TokenType    token_type;  
    std::string debug_string() const; 
};

const Token DEFAULT_TOKEN = { { 0, 0 }, "", Token::Semicolon }; 

struct UnknownToken {
    FilePosition position; 
    std::string  text; 
    std::string debug_string() const;
};

struct LexerOutput { 
    const std::vector<Token> tokens; 
    const std::vector<UnknownToken> unknown_tokens; 
    const bool read_failed;
    const bool open_failed;
    std::string debug_string() const; 
};
 
//// Parsing-related



namespace parsing {
    struct IntConstant {
        Token value; 
    };

    struct Expression {
        enum Type { IntConstant }; 
        Type type; 
        union {
            parsing::IntConstant int_constant;
        };

        Expression(parsing::IntConstant&& subtree) {
            type = IntConstant;
            new (&int_constant) parsing::IntConstant(std::move(subtree));
        }
        
        ~Expression() {
            switch (type) {
                case IntConstant:
                    int_constant.~IntConstant(); 
            }
        }
    };

    struct ReturnStatement {
        Token return_token; 
        std::unique_ptr<Expression> expression;  
        Token semicolon_token;
    };

    struct Statement {
        enum Type { Return };
        Type type; 
        union {
            ReturnStatement statement_return;
        };

        Statement(ReturnStatement&& subtree) {
            type = Return;
            new (&statement_return) ReturnStatement(std::move(subtree));
        }
 
        ~Statement() {
            switch (type) {
                case Return:
                    statement_return.~ReturnStatement(); 
            }
        }
    }; 
    
    struct Function {
        Token type;
        Token identifier; 
        std::unique_ptr<parsing::Statement> statement;
    }; 

    struct Program {
        Function function;  
    }; 
}
struct ParserOutput { 
    
    struct Error {
        FilePosition position; 
        std::string  message;
    };

    bool is_error; 
    union {
        std::unique_ptr<parsing::Program> program; 
        Error error;
    };

    ParserOutput(ParserOutput::Error parse_error) : is_error(true) {
        error = parse_error;
    }

    ParserOutput(std::unique_ptr<parsing::Program> parsed_program) : is_error(false) {
        program = std::move(parsed_program);
    }

    ~ParserOutput() { 
        if (is_error) {
            error.~Error(); 
        } else {
            program.~unique_ptr(); 
        }
    }

    
};

std::string debug_string(const ParserOutput::Error& error) {
        std::stringstream ss;
        ss << "{'position': " << error.position.debug_string() 
            << ",'message': " << error.message << "}";
        return ss.str(); 
    }

const ParserOutput::Error DEFAULT_PARSER_ERROR = { { 0, 0 }, "internal_compilation_error" };
const ParserOutput DEFAULT_PARSER_OUTPUT =
    { DEFAULT_PARSER_ERROR };

