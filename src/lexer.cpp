#include <cynophobia/charstream.hpp>
#include <cynophobia/lexer.hpp>
#include <cynophobia/shared.hpp>
 
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <utility>



std::unordered_set<char> charset_digits() {
    std::unordered_set<char> 
        res{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    return res; 
}

bool all_digits(std::string s) {
    auto digit_chars = charset_digits(); 
    for (const char& c : s) {
        if (digit_chars.find(c) == digit_chars.end()) {
            return false;
        }
    }
    return true; 
}

std::unordered_set<char> charset_wordchars() {
    auto base = charset_digits();
    base.insert('_');
    for (char c = 'a'; c <= 'z'; c++) {
        base.insert(c);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        base.insert(c);
    }
    return base; 
}



LexerOutput lex(FallibleCharStream& fcs, bool debug) {
    bool was_open = fcs.was_opened();
    PositionedStream pfs(fcs); 
  
    std::vector<Token> tokens = {};
    std::vector<UnknownToken> unknown_tokens = {};

    const std::unordered_set<char> wordchars = charset_wordchars();
    const std::unordered_set<char> digitchars = charset_digits();

    bool read_failed = false;
    if (was_open) { 
        while (true) {  
            bool extra_getchars_exit = false; 
            FallibleCharStream::StreamStatus next_status;
            char next_char; 
            FilePosition next_position = pfs.get_next_position();
            std::tie(next_char, next_status) = pfs.get_next_char();  
            
            if (next_status != FallibleCharStream::STREAM_GOOD) {
                read_failed = (next_status == FallibleCharStream::STREAM_ERROR); 
                break; 
            } 

            switch (next_char) {
                // non-alphabetic characters 
#define TOKEN_ONE_CHAR(x,y)                                     \
    case x: {                                                   \
        tokens.push_back({next_position, std::string(1,x), y}); \
        break;                                                  \
    }
                TOKEN_ONE_CHAR('(', Token::OpenParen)
                TOKEN_ONE_CHAR(')', Token::CloseParen)
                TOKEN_ONE_CHAR('{', Token::OpenBrace) 
                TOKEN_ONE_CHAR('}', Token::CloseBrace) 
                TOKEN_ONE_CHAR(';', Token::Semicolon)  
#undef TOKEN_ONE_CHAR
                // whitespace characters 
                case '\n': break;
                case '\r': break;
                case '\v': break;
                case '\f': break;
                case ' ':  break;
                case '\t': break; 
                    

                // non-range-check alphabetic characters
                case '_': {
                    FallibleCharStream::StreamStatus take_wordchars_result; 
                    std::string taken_wordchars; 
                    std::tie(taken_wordchars, take_wordchars_result) 
                        = pfs.get_while_in(wordchars);
                    if (take_wordchars_result != FallibleCharStream::STREAM_GOOD) {
                        read_failed = (take_wordchars_result ==
                        FallibleCharStream::STREAM_ERROR); 
                        extra_getchars_exit = true; 
                    } else {
                        taken_wordchars.insert(taken_wordchars.begin(), next_char);  
                        tokens.push_back({ next_position, taken_wordchars, 
                        Token::Identifier });
                    }
                    break; 
                }
                // maybe-alphabetic characters
                default: {
                    // [a-zA-Z]
                    if (('a' <= next_char && next_char <= 'z') || 
                        ('A' <= next_char && next_char <= 'Z')) { 
                        FallibleCharStream::StreamStatus take_wordchars_result; 
                        std::string taken_wordchars; 
                        std::tie(taken_wordchars, take_wordchars_result) 
                            = pfs.get_while_in(wordchars);
                        if (take_wordchars_result !=
                        FallibleCharStream::STREAM_GOOD) {
                            read_failed = (take_wordchars_result ==
                                FallibleCharStream::STREAM_ERROR); 
                            extra_getchars_exit = true; 
                        }
                        taken_wordchars.insert(taken_wordchars.begin(), next_char); 

                        Token::TokenType token_type = Token::Identifier; 

                        std::unordered_map<std::string, Token::TokenType> keyword_tokens =
                        { { "int", Token::Int }, 
                        { "void",  Token::Void }, 
                        { "return", Token::Return }, }; 

                        auto keyword_it = keyword_tokens.find(taken_wordchars);
                        if (keyword_it != keyword_tokens.end()) {
                            token_type = keyword_it->second; 
                        } 

                        tokens.push_back( { next_position, taken_wordchars, token_type }); 
                        break; 
                    } else if ( '0' <= next_char && next_char <= '9') {
                        FallibleCharStream::StreamStatus take_wordchars_result; 
                        std::string taken_wordchars; 
                        std::tie(taken_wordchars, take_wordchars_result) 
                            = pfs.get_while_in(wordchars);
                        if (take_wordchars_result !=
                        FallibleCharStream::STREAM_GOOD) {
                            read_failed = (take_wordchars_result ==
                                FallibleCharStream::STREAM_ERROR); 
                            extra_getchars_exit = true; 
                        }
                        taken_wordchars.insert(
                            taken_wordchars.begin(), next_char);  
                        if (all_digits(taken_wordchars)) {   
                            tokens.push_back({next_position, taken_wordchars,
                            Token::Constant});
                        } else {
                            unknown_tokens.push_back({next_position, taken_wordchars
                            });
                        }
                        break; 
                    } else {

                        std::string token_text(1, next_char);
                        unknown_tokens.push_back({next_position, token_text});
                        // characters we don't recognize yet
                    }
                    break; 
                }
                
            }
            if (extra_getchars_exit) {
                break; 
            }
        }
    }

    LexerOutput output = { tokens, unknown_tokens, read_failed, !was_open };
    if (debug) { 
        printf("%s", output.debug_string().c_str());
    }

    return output;

}
 

LexerOutput lex_file(const Config& config) { 
    FileCharStream file_fcs(config.filename);
    FallibleCharStream& fcs = file_fcs; 

    return lex(fcs, config.debug); 
}

LexerOutput lex_string(std::string program_string, bool debug) {
    StringCharStream string_fcs(program_string);
    FallibleCharStream& fcs = string_fcs; 
    return lex(fcs, debug); 
}