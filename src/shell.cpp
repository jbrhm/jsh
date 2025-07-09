#include "shell.hpp"

namespace jsh {
    std::shared_ptr<shell> shell::shell_ptr;

    shell::shell(){
        // check to see if jsh is interactive
        is_interactive = syscall_wrapper::isatty_wrapper(syscall_wrapper::STDIN_FILE_DESCRIPTOR);
        std::optional<pid_t> cur_grp_pid = std::nullopt;
        term_if = std::make_shared<termios>(); // structure describing the terminal interface

        // interactive shell setup
        if(is_interactive){
            // loop until jsh is in the foreground
            while(true){
                // get the process group id controlling the terminal
                std::optional<pid_t> tc_grp_pid = syscall_wrapper::tcgetpgrp_wrapper(syscall_wrapper::STDIN_FILE_DESCRIPTOR);

                // get the current process' group id
                cur_grp_pid = syscall_wrapper::getpgrp_wrapper();

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
            std::optional<std::function<void(int)>> sig_status;
            sig_status = syscall_wrapper::signal_wrapper(SIGINT, SIG_IGN); // termination requests

            // error handle
            if(!sig_status.has_value()){
                return;
            }

            sig_status = syscall_wrapper::signal_wrapper(SIGQUIT, SIG_IGN);

            // error handle
            if(!sig_status.has_value()){
                return;
            }

            sig_status = syscall_wrapper::signal_wrapper(SIGTSTP, SIG_IGN);

            // error handle
            if(!sig_status.has_value()){
                return;
            }

            sig_status = syscall_wrapper::signal_wrapper(SIGTTIN, SIG_IGN);

            // error handle
            if(!sig_status.has_value()){
                return;
            }

            sig_status = syscall_wrapper::signal_wrapper(SIGTTOU, SIG_IGN);

            // error handle
            if(!sig_status.has_value()){
                return;
            }


            // create a process group for the shell
            cur_grp_pid = syscall_wrapper::getpid_wrapper();
            assert(cur_grp_pid.has_value()); // this function shouldn't fail

            // if we cant set into our own process group exit
            if(!syscall_wrapper::setpgid_wrapper(cur_grp_pid.value(), cur_grp_pid.value())){
                throw std::runtime_error("Failed to set process group id...");
            }

            // try and grab control of the shell
            if(!syscall_wrapper::tcsetpgrp_wrapper(syscall_wrapper::STDIN_FILE_DESCRIPTOR, cur_grp_pid.value())){
                throw std::runtime_error("Failed to grab control of terminal shell...");
            }

            // get the shell attributes
            if(!syscall_wrapper::tcgetattr_wrapper(syscall_wrapper::STDIN_FILE_DESCRIPTOR, term_if)){
                throw std::runtime_error("Failed to get shell attributes...");
            }
        }else{
            cout_logger.log(LOG_LEVEL::ERROR, "JSH must run interactively...");
            throw std::runtime_error("JSH must run interactively...");
        }
    }

    auto shell::get() -> std::optional<std::shared_ptr<shell>>{
        // creating the shell instance may throw an exception
        try{
            // check to see if we need to make the singleton instance
            if(shell_ptr == nullptr){
                // create the singleton instance
                shell_ptr = std::make_shared<shell>();
            }
        }catch(std::runtime_error const& err){
            // print error
            jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, err.what(), '\n');
            return std::nullopt;
        }

        // we should never return a nullptr, since the shell should always be valid
        assert(shell_ptr != nullptr);
        return std::make_optional<std::shared_ptr<shell>>(shell_ptr);
    }

    auto shell::execute_command() -> bool{
        // stack vars
        std::string input;
        std::string arg;

        // get the command from the user
        jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "prompt:");
        getline(std::cin, input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Raw user input: ", input);
        input = jsh::parsing::variable_substitution(input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Substituted user input: ", input);

        if(input == "exit")
            return false;

        // parse the job
        auto job_data = jsh::job::parse_job(input);

        // execute the job
        jsh::job::execute_job(job_data);

        // success
        return true;
    }

    auto shell::get_term_if() -> std::shared_ptr<termios>{
        return term_if;
    }

    shell::~shell(){

    }
} // namespace jsh
