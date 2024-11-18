#include <cynophobia/lexer.hpp>
#include <cynophobia/shared.hpp>

#include <fstream>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <utility>

class FallibleCharStream {
    public:
        virtual ~FallibleCharStream() {}
        enum StreamStatus {
            STREAM_END,
            STREAM_ERROR,
            STREAM_GOOD
        };
        virtual std::tuple<char, StreamStatus> peek() = 0; 
        virtual std::tuple<char, StreamStatus> get() = 0; 
        virtual bool was_opened() const = 0; 
};


class FileCharStream : public FallibleCharStream {
    public:
        FileCharStream(std::string filename) {
            istream.open(filename); 
            input_was_opened = istream.is_open();
        }


        bool was_opened() const override {
            return input_was_opened;
        }

        std::tuple<char, FallibleCharStream::StreamStatus> get() {
            char next_char = istream.get();  
            if (istream.eof()) {
                return { (char)0, FallibleCharStream::STREAM_END };
            } else if (istream.bad() || istream.fail()) {
                return { (char)0, FallibleCharStream::STREAM_ERROR }; 
            }
            return { next_char, FallibleCharStream::STREAM_GOOD };
        }

        std::tuple<char, FallibleCharStream::StreamStatus> peek() {
            char next_char = istream.peek();  
            if (istream.eof()) {
                return { (char)0, FallibleCharStream::STREAM_END };
            } else if (istream.bad() || istream.fail()) {
                return { (char)0, FallibleCharStream::STREAM_ERROR }; 
            }
            return { next_char, FallibleCharStream::STREAM_GOOD };
        }

    private:
        std::ifstream istream;
        bool input_was_opened; 
};

class StringCharStream : public FallibleCharStream {
    public:
        StringCharStream(const std::string input_string) : 
          text(input_string), index(0), length(input_string.size())
        {} 
             

        bool was_opened() const override {
            // If a string manages to fail I will be gobsmacked. 
            return true; 
        }

        std::tuple<char,  FallibleCharStream::StreamStatus> get() override {
            if (index >= length) {
                return { (char)0, FallibleCharStream::STREAM_END };
            } 
            std::tuple<char, StreamStatus> result =
              { text[index], FallibleCharStream::STREAM_GOOD }; 
            index += 1;
            return result; 
        }

        std::tuple<char,  FallibleCharStream::StreamStatus> peek() override {
            if (index >= length) {
                return { (char)0, FallibleCharStream::STREAM_END };
            } 
            std::tuple<char, StreamStatus> result =
              { text[index], FallibleCharStream::STREAM_GOOD };  
            return result; 
        }

    private:
        const std::string text;
        std::size_t index; 
        std::size_t length; 
};

class PositionedStream {
    public: 
        PositionedStream(FallibleCharStream& char_stream) : 
            fcstream(char_stream), 
            next_char_position({0,0}),
            read_has_failed(false) {} 

        FilePosition get_next_position() const {
            return next_char_position;
        }


        std::tuple<char, FallibleCharStream::StreamStatus> get_next_char() {
            char next_char; 
            FallibleCharStream::StreamStatus next_status; 
            auto next_result = fcstream.get(); 
            std::tie(next_char, next_status) = next_result;

            if (next_status == FallibleCharStream::STREAM_ERROR) {
                read_has_failed = true; 
                return { char(0), FallibleCharStream::STREAM_ERROR }; 
            }

            update_next_char_position(next_char);
             
            if (read_has_failed) {
                return { char(0), FallibleCharStream::STREAM_ERROR };
            }
            return next_result; 
        }

        std::tuple<char, FallibleCharStream::StreamStatus>
          peek_next_char() { 
            FallibleCharStream::StreamStatus next_status; 
            auto next_result = fcstream.peek(); 
            std::tie(std::ignore, next_status) = next_result;

            if (next_status == FallibleCharStream::STREAM_ERROR) {
                read_has_failed = true; 
            }
            return next_result; 
        }

        std::tuple<std::string, FallibleCharStream::StreamStatus> 
          get_while_in(const std::unordered_set<char> &charset) {
            std::string built = ""; 
            while (true) {  
                char peek_char;
                 FallibleCharStream::StreamStatus peek_result; 
                std::tie(peek_char, peek_result) = peek_next_char();
                switch (peek_result) {
                    case FallibleCharStream::STREAM_ERROR:
                        return { built, peek_result }; 
                    case FallibleCharStream::STREAM_END:
                        return { built, peek_result }; 
                    case FallibleCharStream::STREAM_GOOD:
                        if (charset.find(peek_char) == charset.end()) {
                            return { built, peek_result }; 
                        }
                        break; 
                }
                char take_char;
                 FallibleCharStream::StreamStatus take_result;
                std::tie(take_char, take_result) = get_next_char(); 
                if (take_char != peek_char) {
                    // The file changed from under us.
                    read_has_failed = true; 
                    return { built, FallibleCharStream::STREAM_ERROR };
                }
                // We know that the char is in charset from the earlier peek.
                built.push_back(take_char);  
            }
          }

    private: 
        FallibleCharStream& fcstream;
        FilePosition next_char_position;
        bool read_has_failed; 
        
        void update_next_char_position(char next_char) {
            switch (next_char) {
                case '\n': 
                case '\v':
                case '\f':
                    next_char_position = next_char_position.start_next_line(); 
                    break; 
                case '\r': { //  error: jump to case label
                    char follow_char; 
                    FallibleCharStream::StreamStatus follow_result; 
                    std::tie(follow_char, follow_result) = fcstream.peek();  
                    if (follow_result != FallibleCharStream::STREAM_GOOD
                        || (follow_char != '\n')) {
                        next_char_position = 
                          next_char_position.start_next_line(); 
                    } else {
                        next_char_position = next_char_position.next_column();
                    }

                    if (follow_result == FallibleCharStream::STREAM_ERROR) {
                        read_has_failed = true; 
                    }
                    break; 
                }
                default:
                    next_char_position = next_char_position.next_column();
            }
        }

};

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
                    }
                    taken_wordchars.insert(taken_wordchars.begin(), next_char); 
                    tokens.push_back({ next_position, taken_wordchars,
                    Token::Identifier });
                    break; 
                }
                // maybe-alphabetic characters
                default: {
                    // [a-zA-Z]
                    if ('a' <= next_char && next_char <= 'z' || 
                        'A' <= next_char && next_char <= 'Z') { 
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

                        tokens.push_back({ next_position, taken_wordchars, token_type });
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