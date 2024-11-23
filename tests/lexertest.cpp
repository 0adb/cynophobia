#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> 
#include <cynophobia/lexer.hpp>
/*
struct LexerOutput {
    const std::vector<std::string> texts; 
    const std::vector<Token> tokens; 
    const std::vector<UnknownToken> unknown_tokens; 
    const bool read_failed;
    const bool open_failed;
    std::string debug_string() const; 
};
*/

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
std::vector<std::string> get_nonconstant_texts
    (const LexerOutput& lexer_output) {
        return lexer_output.texts; 
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

    const std::vector<std::string> expected_texts = {
        "main", 
        "100"
    };

    const std::vector<std::string> expected_unknown_tokens = {};

    REQUIRE( get_tokentype_sequence(lexer_output) == expected_tokentype_sequence ); 
    REQUIRE( get_nonconstant_texts(lexer_output) == expected_texts );
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

    std::vector<std::string> expected_texts = {
        "main100", 
        "69"
    };
    

    const std::vector<std::string> expected_unknown_tokens = {};

    REQUIRE( get_tokentype_sequence(lexer_output) == expected_tokentype_sequence ); 
    REQUIRE( get_nonconstant_texts(lexer_output) == expected_texts );
    REQUIRE ( get_unknown_tokens(lexer_output) == expected_unknown_tokens ); 
}