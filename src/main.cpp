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

    // check to see if jsh is interactive
    bool is_interactive = jsh::syscall_wrapper::isatty_wrapper(jsh::syscall_wrapper::STDOUT_FILE_DESCRIPTOR);

    // interactive shell setup
    if(is_interactive){
        // loop until jsh is in the foreground
        while(true){
            // get the process group id controlling the terminal
            std::optional<pid_t> tc_grp_pid = jsh::syscall_wrapper::tcgetpgrp_wrapper(jsh::syscall_wrapper::STDOUT_FILE_DESCRIPTOR);

            // get the current process' group id
            std::optional<pid_t> cur_grp_pid = jsh::syscall_wrapper::getpgrp_wrapper();

            // error handle
            if(!tc_grp_pid.has_value() || !cur_grp_pid.has_value()){
                continue;
            }

            // no more errors
            assert(tc_grp_pid.has_value());
            assert(cur_grp_pid.has_value());

            // we are the process in the foreground of the terminal
            if(tc_grp_pid.value() == cur_grp_pid.value()){
                break;
            }

            // try and kill the process controlling the terminal
            kill(-cur_grp_pid.value(), SIGTTIN);
        }
    }

    std::cout << is_interactive << '\n';

    while(true){
        // get the command from the user
        jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "prompt:");
        getline(std::cin, input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Raw user input: ", input);
        input = jsh::parsing::variable_substitution(input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Substituted user input: ", input);

        // parse the job
        auto job_data = jsh::job::parse_job(input);

        // execute the job
        jsh::job::execute_job(job_data);
    }
}
