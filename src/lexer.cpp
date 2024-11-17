#include <cynophobia/lexer.hpp>
#include <cynophobia/shared.hpp>

#include <fstream>
#include <tuple> 
#include <unordered_set>
#include <unordered_map>


class PositionedFileStream {
    public:
    // rah: basic_ifstream(const basic_ifstream&) = delete;
        PositionedFileStream(std::string filename) : istream(filename), 
            next_char_position({0,0}) {} 

        enum FileReadResult {
            END_OF_FILE,
            READ_ERROR,
            GOOD_READ,
        };

        bool is_open() const {
            return istream.is_open();
        }

        std::tuple<PositionedFileStream::FileReadResult, char, FilePosition>
          get_next_char() { 
            char next_char = istream.get();  
            if (istream.eof()) {
                return { END_OF_FILE,  (char)0, { 0, 0 }}; 
            } 
            if (istream.bad() || istream.fail()) {
                return { READ_ERROR,  (char)0, { 0, 0 }};  
            }
            FilePosition this_position = next_char_position; 
            switch (next_char) {
                case '\n': 
                case '\v':
                case '\f':
                    next_char_position = next_char_position.start_next_line(); 
                    break; 
                case '\r': { //  error: jump to case label
                    char follow = istream.peek();
                    if (istream.eof() || istream.bad() || istream.fail()
                        || (follow != '\n')) {
                        next_char_position = 
                          next_char_position.start_next_line(); 
                    } else {
                        next_char_position = next_char_position.next_column();
                    }
                    break; 
                }
                default:
                    next_char_position = next_char_position.next_column();
            }
            return { GOOD_READ,  next_char, this_position }; 
        }

        std::tuple<PositionedFileStream::FileReadResult, char>
          peek_next_char() { 
            char next_char = istream.peek();  
            if (istream.eof()) {
                return { END_OF_FILE, (char) 0 };    
            } else if (istream.bad() || istream.fail()) {
                return { READ_ERROR, (char) 0 }; 
            }
            return { GOOD_READ, next_char }; 
        }

        std::tuple<PositionedFileStream::FileReadResult, std::string> 
          get_while_in(const std::unordered_set<char> &charset) {
            std::string built = ""; 
            while (true) { 
                char peek_char;
                FileReadResult peek_result; 
                std::tie(peek_result, peek_char) = peek_next_char();
                auto next_peek = peek_next_char();  
                switch (peek_result) {
                    case READ_ERROR:
                        return { READ_ERROR, built }; 
                    case END_OF_FILE:
                        return { END_OF_FILE, built }; 
                    case GOOD_READ:
                        if (charset.find(peek_char) == charset.end()) {
                            return { GOOD_READ, built }; 
                        }
                }
                char take_char;
                FileReadResult take_result;
                std::tie(take_result, take_char, std::ignore) = get_next_char(); 
                if (take_char != peek_char) {
                    // The file changed from under us.
                    return { READ_ERROR, built };
                }
                // We know that the char is in charset from the earlier peek.
                built.push_back(take_char);  
            }
          }

    private: 
        std::ifstream istream;  
        FilePosition next_char_position;
};

std::unordered_set<char> charset_digits()
{
    std::unordered_set<char> 
        res{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    return res; 
}

std::unordered_set<char> charset_wordchars() 
{
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

LexerOutput lex(const Config& config) { 


    PositionedFileStream pfs(config.filename);
    bool open_failed = !(pfs.is_open());

    std::vector<Token> tokens = {};
    std::vector<UnknownToken> unknown_tokens = {};

    const std::unordered_set<char> wordchars = charset_wordchars();
    const std::unordered_set<char> digitchars = charset_digits();

    bool read_failed = false;
    while (!open_failed) {  
        bool extra_getchars_exit = false; 
        PositionedFileStream::FileReadResult read_result;
        char next_char; 
        FilePosition next_position;
        std::tie(read_result, next_char, next_position) = pfs.get_next_char();  
        if (read_result != PositionedFileStream::GOOD_READ) {
            read_failed = (read_result ==
             PositionedFileStream::READ_ERROR); 
            break; 
        } 

        switch (next_char) {
            // non-alphabetic characters
            case '(': {
                std::string token_text(1, next_char);
                tokens.push_back( {next_position, token_text, Token::OpenParen});
                break; 
            }
            case ')': {
                std::string token_text(1, next_char);
                tokens.push_back( {next_position, token_text, Token::CloseParen});
                break; 
            }
            case '{': {
                std::string token_text(1, next_char);
                tokens.push_back( {next_position, token_text, Token::OpenBrace});
                break; 
            }
            case '}': {
                std::string token_text(1, next_char);
                tokens.push_back( {next_position, token_text, Token::CloseBrace});
                break;
            }
            case ';': {
                std::string token_text(1, next_char);
                tokens.push_back( {next_position, token_text, Token::Semicolon });
                break; 
            }

            // whitespace characters 
            case '\n':
            case '\r': 
            case '\v':
            case '\f':
            case ' ':
            case '\t':
                // Intentionally do nothing. Whitespace isn't
                // a recognized token but is allowed (to separate 
                // tokens, etc.). 
                break;

            // non-range-check alphabetic characters
            case '_': {
                PositionedFileStream::FileReadResult take_wordchars_result; 
                std::string taken_wordchars; 
                std::tie(take_wordchars_result, taken_wordchars) 
                    = pfs.get_while_in(wordchars);
                if (take_wordchars_result != PositionedFileStream::GOOD_READ) {
                    read_failed = (take_wordchars_result ==
                    PositionedFileStream::READ_ERROR); 
                    extra_getchars_exit = true; 
                }
                taken_wordchars.insert(taken_wordchars.begin(), next_char); 
                tokens.push_back( { next_position, taken_wordchars, Token::Identifier });
                break; 
            }
            // maybe-alphabetic characters
            default: {
                // [a-zA-Z]
                if ('a' <= next_char && next_char <= 'z' || 
                    'A' <= next_char && next_char <= 'Z') { 
                    PositionedFileStream::FileReadResult take_wordchars_result; 
                    std::string taken_wordchars; 
                    std::tie(take_wordchars_result, taken_wordchars) 
                        = pfs.get_while_in(wordchars);
                    if (take_wordchars_result !=
                      PositionedFileStream::GOOD_READ) {
                        read_failed = (take_wordchars_result ==
                        PositionedFileStream::READ_ERROR); 
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

                    tokens.push_back({ next_position, taken_wordchars, token_type });
                    break; 
                } else if ( '0' <= next_char && next_char <= '9') {
                    PositionedFileStream::FileReadResult take_digitchars_result; 
                    std::string taken_digitchars; 
                    std::tie(take_digitchars_result, taken_digitchars) 
                        = pfs.get_while_in(digitchars);
                    if (take_digitchars_result !=
                      PositionedFileStream::GOOD_READ) {
                        read_failed = (take_digitchars_result ==
                        PositionedFileStream::READ_ERROR); 
                        extra_getchars_exit = true; 
                    }
                    taken_digitchars.insert(
                        taken_digitchars.begin(), next_char);  
                    tokens.push_back({next_position, taken_digitchars, Token::Constant});
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

    LexerOutput output = { tokens, unknown_tokens, read_failed, open_failed };
    if (config.debug) { 
        printf("%s", output.debug_string().c_str());
    }

    return output;

}