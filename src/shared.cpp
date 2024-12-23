#include <cynophobia/shared.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <unordered_set> 
#include <unordered_map>
#include <vector>

std::string FilePosition::debug_string() const {
    std::ostringstream oss;
    oss << "{'line': " << line << ", 'column': " << column << "}";
    return oss.str();
}

FilePosition FilePosition::next_column() {
    return { line, column + 1 };
}

FilePosition FilePosition::start_next_line() {
    return { line + 1, 0 }; 
}

std::string Token::debug_string() const  {
    std::ostringstream oss;
    oss << "{'position': " << position.debug_string();
    oss << ", 'text': \"" << text << "\"," ;
    oss << "'token_type': ";



    switch (token_type) { 
#define SELF_PRINT(x)            \
case x:                          \
    oss << "'" << #x << "'";     \
    break;
        SELF_PRINT(Semicolon)
        SELF_PRINT(Identifier)
        SELF_PRINT(Constant) 
        SELF_PRINT(Int)
        SELF_PRINT(Void) 
        SELF_PRINT(Return)
        SELF_PRINT(OpenParen)    
        SELF_PRINT(CloseParen)  
        SELF_PRINT(OpenBrace)  
        SELF_PRINT(CloseBrace)
#undef SELF_PRINT
    }  
    
    oss << "}";
    return oss.str();
}


std::string UnknownToken::debug_string() const {
    std::ostringstream oss;
    oss << "{'position': " << position.debug_string();
    oss << ",'text': '" << text << "'}"; 
    return oss.str();
}

std::string LexerOutput::debug_string() const {
    std::ostringstream oss;
    oss << "{'open_failed': " << (open_failed ? "true" : "false");
    oss << ",'read_failed': " << (read_failed ? "true" : "false");
    oss << ",'tokens':[";
    {
        bool first = true; 
        for (const Token& t : tokens) {
            if (!first) {
                oss << ",";
            }
            oss << t.debug_string();
            first = false; 
        }
    }

    oss << "],'unknown_tokens':[";
    {
        bool first = true;
        for (const UnknownToken& u : unknown_tokens) {
            if (!first) {
                oss << ",";
            }
            oss << u.debug_string();
            first = false;
        }
    }
    oss << "]}";
    return oss.str();
}