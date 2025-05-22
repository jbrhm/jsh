#pragma once

#include "pch.hpp"

namespace jsh {
    /**
     * The goal of the environment class is to facilitate the shell's interaction with environment variables
     */
    class environment {
    public:
        /**
         *
         * set_var: sets the environment variable var to value
         *
         * var: the name of the environment variable being altered in c-string format
         *
         * value: the value to which the environment variable should be set
         */
        static void set_var(char const* var, char const* value);

        /**
         * get_var: gets the value of the environment variable var
         *
         * var: the name of the environment variable
         */
        inline static void get_var(char const* var);

        /**
         * get: gets the array of environment strings representing the current process' environment
         */
        [[nodiscard]] inline static auto get() -> char**;

<<<<<<< Updated upstream
        // print out the environment in a nice visual way
        void print([[maybe_unused]] logger& log = cout_logger);
=======
        /**
         * print: 
         */
        inline static void print([[maybe_unused]] logger& log = cout_logger){

        }
>>>>>>> Stashed changes
    };
} // namespace jsh
