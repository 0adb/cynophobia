#pragma once
#include <cstring>
#include <sstream>
#include <tuple>
#include <utility>
#include <cynophobia/shared.hpp>

ParserOutput parse_program(
    const std::vector<std::string> texts,
    const std::vector<Token> tokens
);