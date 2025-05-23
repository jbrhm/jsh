#include "pch.hpp"

// JSH
#include "environment.hpp"
#include "parsing.hpp"

int main(int argc, char* argv[], char* envp[]){
    jsh::set_log_level(1);

    jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "Welcome to John's Shell\n");

    std::string input;
    std::string arg;
    std::vector<std::string> args_string;
    std::vector<char*> args_ptr;
    while(true){
        // get the command from the user
        jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "prompt:");
        getline(std::cin, input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Raw user input: ", input);
        input = jsh::parsing::variable_substitution(input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Substituted user input: ", input);

        // parse the command into different components
        std::stringstream ss(input);
        args_string.clear();
        args_ptr.clear();
        while(ss >> arg){
            args_string.push_back(arg);
        }

        // convert the args from std::string to char*
        for(auto const& a : args_string){
            args_ptr.push_back(const_cast<char*>(a.c_str()));
        }
        args_ptr.push_back(nullptr);

        pid_t pid = fork();
        int status;
        if(pid){ // parent
            pid_t exit_code = waitpid(pid, &status, 0);

            if(exit_code != pid){
                jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Waiting on PID ", pid, " failed...");
            }
        }else{ // child
            int exit_code = execvp(args_ptr[0], args_ptr.data());

            // execvp only returns if there was an error
            assert(exit_code == -1);

            // log that this command was invalid
            jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Executing command failed...");
        }
    }
}
