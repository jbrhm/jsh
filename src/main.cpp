#include "pch.hpp"

// JSH
#include "environment.hpp"
#include "parsing.hpp"
#include "process.hpp"
#include "job.hpp"

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

        // parse the job
        jsh::job::parse_job(input);

    }
}
