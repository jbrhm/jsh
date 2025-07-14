#pragma once

#include "pch.hpp"

// JSH
#include "environment.hpp"
#include "macros.hpp"

namespace jsh {
    class parsing {
    private:
        /**
         * CONSTANT VARIABLES
         */
        static constexpr int MAX_SUBSTITUTIONS = 1000000;
    public:
        /**
         * peek_char: returns a the character of a string at a given index or std::nullopt if it is invalid (index is npos or index is out of bounds)
         *
         * str: the string which we will retrieve the character from
         *
         * index: the index of the desired character
         */
        static auto peek_char(std::string const& str, std::size_t index) -> std::optional<char>;

        /**
         * variable_substition will perform any environment variable substitutions in a given string
         *
         * input: the input which will have substitutions performed on it
         */
        static auto variable_substitution(std::string const& input) -> std::string;
    };
} // namespace jsh

