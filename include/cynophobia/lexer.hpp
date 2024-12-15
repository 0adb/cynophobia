#pragma once
#include <cynophobia/shared.hpp>

#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <utility>

LexerOutput lex_file(
    const Config& config
);

LexerOutput lex_string(
    std::string program_string,
    bool debug
);

