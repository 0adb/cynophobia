#pragma once
#include <cynophobia/shared.hpp>

#include <fstream>
#include <unordered_set>

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