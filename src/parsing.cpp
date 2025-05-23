#include "parsing.hpp"

namespace jsh {
    auto parsing::peek_char(std::string const& str, std::size_t index) -> std::optional<char> {
        // check conditions for invalidity
        if(index == std::string::npos || index >= str.size()){
            return std::nullopt;
        }

        // retrieve the character
        return std::make_optional<char>(str[index]);
    }

    auto parsing::variable_substitution(std::string const& input) -> std::string {
        // create a copy of the input 
        std::string output = input;

        // while there exists a $ in the string perform substitutions
        std::size_t it;
        while((it = output.find('$')) != std::string::npos){
            
        }

    }
} // namespace jsh
