#pragma once

// JSH
#include "macros.hpp"
#include "pch.hpp"

namespace jsh {
    /**
     * The goal of the environment class is to facilitate the shell's interaction with environment variables
     */
    class environment {
    private:
        /**
         * CONSTANT VARIABLES
         */
        static constexpr char const* EMPTY_STRING = "";

        /**
         * get: gets the array of environment strings representing the current process' environment
         */
        [[nodiscard]] static auto get() -> char**;

    public:
        /**
         * CONSTANT VARIABLES
         */
        static constexpr char const* STATUS_STRING = "?";
        static constexpr char const* SUCCESS_STRING = "0";

        /**
         *
         * set_var: sets the environment variable var to value
         *
         * var: the name of the environment variable being altered in c-string format
         *
         * val: the value to which the environment variable should be set
         */
        static void set_var(char const* var, char const* val);

        /**
         * get_var: gets the value of the environment variable var
         *
         * var: the name of the environment variable
         */
        [[nodiscard]] static auto get_var(char const* var) -> char const*;


        /**
         * print: prints the current processes environment
         */
        static void print([[maybe_unused]] logger& log = cout_logger);
    };
} // namespace jsh
