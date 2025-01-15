#include <cynophobia/shared.hpp>


template<typename T>
class ParseResult {
    public: 
    size_t next_index;
    bool is_error; 
    union {
        std::unique_ptr<T> result;
        ParserOutput::Error error;
    };
    
    // These constructors are defined mostly for brace-initializers to be
    // possible.
        ParseResult(size_t next_index, ParserOutput::Error parse_error) : 
            next_index(next_index), is_error(true) {
            error = parse_error;
        }

        ParseResult(size_t next_index, std::unique_ptr<T> parse_result) :
            next_index(next_index), is_error(false) {
            result = std::move(parse_result);
        } 

    // This constructor is here so I can soundly return a ParseResult from a
    // function and then store it in a variable.
        ParseResult(ParseResult&& other) noexcept : 
            next_index(other.next_index), is_error(other.is_error) {
            if (other.is_error) {
                error = other.error;
            } else {
                result = std::move(other.result); 
            }
        }

        ~ParseResult() {
            if (is_error) {
                error.~ParserOutput::Error();
            } else {
                result.~unique_ptr();
            }
        }
};

FilePosition next_char_position(FilePosition current_position, 
    char next_char, char next_next_char) {
        switch (next_char) {
            case '\n':
                return current_position.start_next_line();
            case '\v':
                return current_position.start_next_line();
            case '\f':
                return current_position.start_next_line();
            case '\r':
                if (next_next_char == (char)0 || (next_next_char != '\n')) {
                    return current_position.next_column();
                } else {
                    return current_position.start_next_line();
                }
            default:
                return current_position.next_column();
        }
    }

// If current_index is past the end of the tokens vector,
// returns end of file token. 
// Possible tradeoff: this implementation will require
// callers to be aware that this function will not catch
// the 'out of bounds' argument.
FilePosition get_current_position(const std::vector<Token> tokens,
  size_t current_index) {
    if (current_index >= tokens.size()) {
        if (tokens.size() == 0) {
            return { 0, 0 };
        }
        
        Token last_token = tokens[tokens.size() - 1];
        FilePosition end = last_token.position;
        std::string text = last_token.text;
        for (size_t i = 0; i < text.size(); i++) {
            end = next_char_position(end, text[i], (i + 1 >= text.size()) ? (char)0 : text[i + 1]); 
        }
        return end;
    } else {
        return tokens[current_index].position;   
    }
  }


// If starting_index is beyond tokens, will return an error.
//  
ParseResult<parsing::Expression> parse_expression(
    const std::vector<Token> tokens,
    size_t starting_index
) {
    if (starting_index >= tokens.size()) {
        return { starting_index, { get_current_position(tokens, starting_index), "reached end of file, expected token"}};
    } 
    Token at_position = tokens[starting_index];
    switch (at_position.token_type) {
        case Token::Constant: {
                std::unique_ptr<parsing::Expression> expression(new parsing::Expression {{ tokens[starting_index] } });
                return { (size_t)(starting_index + 1), std::move(expression) };
            }
        default:    
            return { starting_index, { get_current_position(tokens, starting_index), "expected constant token, found other token \"" + at_position.text + "\""}};
    }
}

ParseResult<parsing::Statement> parse_statement(
    const std::vector<Token> tokens,
    size_t starting_index
) {
    if (starting_index >= tokens.size()) {
        return { starting_index, { get_current_position(tokens, starting_index), "reached end of file, expected token"}};
    } 
    Token at_position = tokens[starting_index];
    switch (at_position.token_type) {
        case Token::Return: { 
                ParseResult<parsing::Expression> return_value = parse_expression(tokens, (size_t) (starting_index + 1));
                if (return_value.is_error) {
                    return { starting_index, return_value.error };
                } else {
                    if (return_value.next_index >= tokens.size()) {
                        return { starting_index, { get_current_position(tokens, starting_index), "reached end of file, after expression expected token"}};
                    } 
                    Token after_expression = tokens[return_value.next_index];
                    switch (after_expression.token_type) {
                        case Token::Semicolon: {
                            parsing::ReturnStatement return_statement { at_position, std::move(return_value.result) , after_expression };
                            std::unique_ptr<parsing::Statement> statement{new parsing::Statement { std::move(return_statement)}};
                            return { return_value.next_index + 1, std::move(statement) };
                        }
                        default: 
                            return { starting_index, { get_current_position(tokens, starting_index), "reached end of file, after expression expected semicolon"}};
                    
                    }
                }
            }
        default:    
            return { starting_index, { get_current_position(tokens, starting_index), "expected constant token, found other token with text: \"" + at_position.text + "\""}};
    }
}



