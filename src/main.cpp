#include "pch.hpp"

// JSH
#include "environment.hpp"
#include "parsing.hpp"
#include "process.hpp"

int main(int argc, char* argv[], char* envp[]){
    jsh::set_log_level(1);

    jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "Welcome to John's Shell\n");

    std::string input;
    std::string arg;
    while(true){
        // get the command from the user
        jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "prompt:");
        getline(std::cin, input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Raw user input: ", input);
        input = jsh::parsing::variable_substitution(input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Substituted user input: ", input);

        // parse the users input into a data describing a specific process
        std::optional<std::unique_ptr<jsh::process_data>> data = jsh::process::parse_process(input);

        // check the to see if the input was valid
        if(!data.has_value()){ // invalid
            continue;
        }

        // perform the process' execution
        std::visit([](auto&& var){
                    jsh::process::execute_process(var);
                }, *data.value());
    }
}
