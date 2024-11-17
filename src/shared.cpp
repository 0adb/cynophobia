#include <cynophobia/shared.hpp>
#include <unordered_map> 
#include <string> 

std::string FilePosition::debug_string() const {
    // auto fmt = "line: %d, column: %d";
    // int sz = std::snprintf(nullptr, 0, fmt, line, column);
    // std::vector<char> buf(sz + 1); // note +1 for null terminator
    // std::sprintf(buf.data(), fmt, line, column);
    // std::string s(buf.begin(), buf.end());
    // return s; 
    std::ostringstream oss;
    oss << line << ":" << column;
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
    oss << "position: " << position.debug_string() << "\n";
    oss << "text: " << text << "\n";
    oss << "token_type: ";
    std::unordered_map<Token::TokenType, std::string> lookup = 
        {
            {Identifier, "Identifier"},
            {Constant, "Constant"},
            {Int, "Int"},
            {Void, "Void"},
            {Return, "Return"},
            {OpenParen, "OpenParen"},
            {CloseParen, "CloseParen"},
            {OpenBrace, "OpenBrace"},
            {CloseBrace, "CloseBrace"},
            {Semicolon, "Semicolon"},
        };
    auto it = lookup.find(token_type);
    if (it == lookup.end()) {
        oss << "<unknown>";
    } else {
        oss << it->second; 
    }
    oss << "\n";
    return oss.str();
}


std::string UnknownToken::debug_string() const {
    std::ostringstream oss;
    oss << "position: " << position.debug_string() << "\n";
    oss << "text: " << text << "\n";
    oss << "token_type: unknown\n";
    return oss.str();
}

std::string LexerOutput::debug_string() const {
    std::ostringstream oss;
    oss << "open_failed: " << (open_failed ? "true" : "false") << "\n";
    oss << "read_failed: " << (read_failed ? "true" : "false") << "\n";
    oss << "tokens: \n";
    for (const Token& t : tokens) {
        oss << t.debug_string();
    }
    oss << "unknown tokens: \n";
    for (const UnknownToken& u : unknown_tokens) {
        oss << u.debug_string();
    }
    return oss.str();
}