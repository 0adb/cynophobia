#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> 
#include <cynophobia/lexer.hpp> 

std::vector<Token::TokenType> get_tokentype_sequence 
    (const LexerOutput& lexer_output) {
        std::vector<Token::TokenType> token_types = {};
        for (const Token& token : lexer_output.tokens) {
            token_types.push_back(token.token_type); 
        }
        return token_types;
    }

// Nonconstant as in, given a token type, it's ambiguous what
// the token text would be. (E.g., not "Semicolon", which can only mean ";"
// , but "Identifier", which could be any string matching [a-zA-Z_]\w*\b)
std::vector<std::string> get_tokentext_sequence
    (const LexerOutput& lexer_output) { 
        std::vector<std::string> token_types = {};
        for (const Token& token : lexer_output.tokens) {
            token_types.push_back(token.text); 
        }
        return token_types;
    }

std::vector<std::string> get_unknown_tokens 
    (const LexerOutput& lexer_output) { 
        std::vector<std::string> unknown_tokens = {};
        for (const UnknownToken& unknown_token : lexer_output.unknown_tokens) {
            unknown_tokens.push_back(unknown_token.text); 
        }
        return unknown_tokens;
    }

TEST_CASE( "Lexing a valid chapter 1 parse", "[lexer][chapter1]" ) {
    std::string valid_parse = "int main(void) { return 100; }";
    LexerOutput lexer_output = lex_string(valid_parse, false); 

    const std::vector<Token::TokenType> expected_tokentype_sequence = {
        Token::Int, 
        Token::Identifier, 
        Token::OpenParen,
        Token::Void,
        Token::CloseParen, 
        Token::OpenBrace,
        Token::Return,
        Token::Constant, 
        Token::Semicolon,
        Token::CloseBrace 
    };

    const std::vector<std::string> expected_tokentext_sequence = {
        "int",
        "main",
        "(",
        "void",
        ")",
        "{",
        "return",
        "100",
        ";",
        "}"
    };

    const std::vector<std::string> expected_unknown_tokens = {};


    REQUIRE( get_tokentext_sequence(lexer_output) == expected_tokentext_sequence );  
    REQUIRE( get_tokentype_sequence(lexer_output) == expected_tokentype_sequence );  
    REQUIRE ( get_unknown_tokens(lexer_output) == expected_unknown_tokens ); 
}

TEST_CASE( "Lexing an invalid parse but with valid chapter 1 tokens", "[lexer][chapter1]" ) {
    std::string invalid_parse = "main100 ) 69 int ( void ; return } {";
    LexerOutput lexer_output = lex_string(invalid_parse, false); 

    std::vector<Token::TokenType> expected_tokentype_sequence = {
        Token::Identifier, 
        Token::CloseParen, 
        Token::Constant, 
        Token::Int, 
        Token::OpenParen,
        Token::Void,
        Token::Semicolon,
        Token::Return,
        Token::CloseBrace, 
        Token::OpenBrace,
    };

    std::vector<std::string> expected_tokentext_sequence = {
        "main100", 
        ")",
        "69",
        "int",
        "(",
        "void",
        ";",
        "return",
        "}",
        "{"
    };
    

    const std::vector<std::string> expected_unknown_tokens = {};

    REQUIRE( get_tokentype_sequence(lexer_output) == expected_tokentype_sequence ); 
    REQUIRE( get_tokentext_sequence(lexer_output) == expected_tokentext_sequence );   
    REQUIRE ( get_unknown_tokens(lexer_output) == expected_unknown_tokens ); 
}