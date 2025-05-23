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

        std::size_t last_dollar_sign_location = 0;

        // while there exists a $ in the string perform substitutions
        for(std::size_t sub = 0; sub < MAX_SUBSTITUTIONS; ++sub){
            // location of the desired character
            std::size_t dollar_sign_location = std::string::npos;
            std::size_t open_brace_location = std::string::npos;
            std::size_t closed_brace_location = std::string::npos;

            // find the first instance of the $
            for(std::size_t i = last_dollar_sign_location; i < output.size(); ++i){
                if(auto c = peek_char(output, i); c && c.value() == '$'){
                    dollar_sign_location = i;
                    break;
                }
            }

            // if we didn't find a $ break out of the loop
            if(dollar_sign_location == std::string::npos){
                break;
            }

            // find the first instance of the {
            for(std::size_t i = dollar_sign_location; i < output.size(); ++i){
                if(auto c = peek_char(output, i); c && c.value() == '{'){
                    open_brace_location = i;
                    break;
                }
            }

            // the addition here is not going to overflow since dollar_sign_location cannot be std::string::npos
            assert(dollar_sign_location != std::string::npos);

            // if we didn't find a { continue to the next substitution
            // beyond the current one
            if(open_brace_location == std::string::npos || dollar_sign_location + 1 != open_brace_location){
                last_dollar_sign_location = dollar_sign_location + 1;
                continue;
            }

            // find the last instance of the }
            for(std::size_t i = open_brace_location; i < output.size(); ++i){
                if(auto c = peek_char(output, i); c && c.value() == '}'){
                    closed_brace_location = i;
                    break;
                }
            }

            // if we failed this substitution continue to the next one
            if(closed_brace_location == std::string::npos){
                last_dollar_sign_location = dollar_sign_location + 1;
                continue;
            }

            // make sure the characters are in the correct order
            // $
            assert(dollar_sign_location != std::string::npos);
            assert(dollar_sign_location + 1 == open_brace_location);
            assert(dollar_sign_location < closed_brace_location);

            // {
            assert(open_brace_location != std::string::npos);
            assert(open_brace_location < closed_brace_location);

            // }
            assert(closed_brace_location != std::string::npos);

            // make the substitution
            std::string prefix = output.substr(0, dollar_sign_location);
            std::string suffix = output.substr(std::min(closed_brace_location + 1, output.size()), output.size());
            std::string var = output.substr(open_brace_location + 1, closed_brace_location - open_brace_location - 1);

            output = prefix + environment::get_var(var.c_str()) + suffix;
        }

        return output;

    }
} // namespace jsh
