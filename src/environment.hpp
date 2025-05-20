#pragma once

// STL
#include <unordered_map>
#include <string>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <vector>
#include <utility>

// JSH
#include "macros.hpp"

namespace jsh{
    class environment {
    private:
        
        // CONSTANTS
        static constexpr char const* EQUALS = "=";

        // environment variables
        // mapping key strings to value strings
        std::unordered_map<std::string, std::string> variables;

        // fully mapped environment variables
        // should only ever be accessed immediately after callind craete_environment
        std::vector<std::string> envp;

    public:
        // remove all implicitly defined constructors
        environment() = delete;
        environment(environment const& other) = delete;
        environment(environment&& other) = delete;
        auto operator=(environment const& other) -> environment& = delete;
        auto operator=(environment&& other) -> environment& = delete;

        // create an environment from the envp* passed into the shell
        environment(char* envp[]);

        // create an environment of the form char* envp[] so it can be passed to subprocesses
        [[nodiscard]] auto create_environment() -> std::vector<char*>;

        // print out the environment in a nice visual way
        void print();
    };
}
