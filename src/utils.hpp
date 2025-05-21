#pragma once

// STL
#include <string>
#include <istream>
#include <variant>
#include <optional>

namespace jsh {
    // < character
    struct file_input_redirection {};

    // > character
    struct file_output_redirection {};

    using token = std::variant<std::string, file_input_redirection, file_output_redirection>;

    /**
     * The goal of this function is to parse out all special characters from the input, otherwise return a std::string
     * Special Characters:
     * - < file input redirection
     * - > file output redirection
     */
    inline std::optional<token> next_token(std::istream& input_stream){
        // string to hold next token
        std::string _token;

        // read in the next token
        input_stream >> _token;

        if(_token == "<"){ // file input redirection
            return std::make_optional<token>(file_input_redirection());
        }else if(_token == ">"){ // file output redirection
            return std::make_optional<token>(file_output_redirection());
        }else{ // string
            return std::make_optional<token>(_token);
        }
    }
}
