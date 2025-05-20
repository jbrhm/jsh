#include "environment.hpp"

namespace jsh {
    environment::environment(char* envp[]){
        // loop through the existing environment created and create the data structure
        for(std::size_t i = 0; envp[i] != nullptr; ++i){
            assert(envp[i] != nullptr);

            // find the location of the equals
            char* equals_index = std::strstr(envp[i], EQUALS);

            // create a string over the key
            std::size_t key_length = equals_index - envp[i];
            std::string key(envp[i], key_length);

            // create a string over the value
            std::string value(equals_index + 1);

            // create the new index in the variables map
            variables[key] = value;
        }
    }

    auto environment::create_environment() -> std::vector<char*>{
        // reserve spaces in internal data structures
        std::vector<char*> new_environment(variables.size());
        envp.clear();
        envp.reserve(variables.size());

        // loop over all variables and add their pointers to the vector
        for(auto const& [key, value] : variables){
            envp.push_back(key + EQUALS + value);
            new_environment.push_back(envp.back().data());
        }

        return new_environment;
    }

    void environment::print(){
        // print all of the variables and their values
        for(auto const& [key, value] : variables){
            log(LOG_LEVEL::STATUS, key, EQUALS, value);
        }
    }
}
