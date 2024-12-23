#pragma once
#include <cynophobia/shared.hpp>


LexerOutput lex_file(
    const Config& config
);

LexerOutput lex_string(
    std::string program_string,
    bool debug
);

