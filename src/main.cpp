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
    std::optional<pid_t> cur_grp_pid = std::nullopt;
    std::shared_ptr<termios> term_if = std::make_shared<termios>(); // structure describing the terminal interface

    // interactive shell setup
    if(is_interactive){
        // loop until jsh is in the foreground
        while(true){
            // get the process group id controlling the terminal
            std::optional<pid_t> tc_grp_pid = jsh::syscall_wrapper::tcgetpgrp_wrapper(jsh::syscall_wrapper::STDOUT_FILE_DESCRIPTOR);

            // get the current process' group id
            cur_grp_pid = jsh::syscall_wrapper::getpgrp_wrapper();

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

        // ignore all of the job control signals
        signal(SIGINT, SIG_IGN); // termination requests
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);

        // create a process group for the shell
        cur_grp_pid = jsh::syscall_wrapper::getpid_wrapper();
        assert(cur_grp_pid.has_value()); // this function shouldn't fail

        // if we cant set into our own process group exit
        if(!jsh::syscall_wrapper::setpgid_wrapper(cur_grp_pid.value(), cur_grp_pid.value())){
            return 1;
        }

        // try and grab control of the shell
        if(!jsh::syscall_wrapper::tcsetpgrp_wrapper(jsh::syscall_wrapper::STDOUT_FILE_DESCRIPTOR, cur_grp_pid.value())){
            return 1;
        }

        // get the shell attributes
        if(!jsh::syscall_wrapper::tcgetattr_wrapper(jsh::syscall_wrapper::STDOUT_FILE_DESCRIPTOR, term_if)){
            return 1;
        }
    }else{
        jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "JSH must run interactively...");
        return 1;
    }

    while(true){
        // get the command from the user
        jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "prompt:");
        getline(std::cin, input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Raw user input: ", input);
        input = jsh::parsing::variable_substitution(input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Substituted user input: ", input);

        if(input == "exit")
            return 0;

        // parse the job
        auto job_data = jsh::job::parse_job(input);

        // execute the job
        jsh::job::execute_job(job_data);
    }
}
