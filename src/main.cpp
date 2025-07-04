#include "pch.hpp"

// JSH
#include "shell.hpp"

int main(int argc, char* argv[], char* envp[]){
    jsh::set_log_level(1);

    jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "Welcome to John's Shell\n");

    std::string input;
    std::string arg;

    try{
        // this can throw an exception so it must be wrapped in a try and catch
        jsh::shell jsh;

        while(true){
            // execute the user input
            bool status = jsh.execute_command();

            if(!status){
                return 0;
            }
        }
    }catch(std::runtime_error const& err){
        // print error
        jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, err.what(), '\n');
    }

    // if we reach this point there was an error
    return 1;
}
