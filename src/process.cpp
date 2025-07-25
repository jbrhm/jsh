#include "process.hpp"
#include "shell.hpp" // required to be here for non-cyclic includes

namespace jsh {
process::shell_internal_redirection::shell_internal_redirection(std::optional<file_descriptor_wrapper> stdout, std::optional<file_descriptor_wrapper> stdin, std::optional<file_descriptor_wrapper> stderr, bool restore) : new_stdout{std::move(stdout)}, new_stdin{std::move(stdin)}, new_stderr{std::move(stderr)}, og_stdout{std::nullopt}, og_stdin{std::nullopt}, og_stderr{std::nullopt}, _restore{restore} {
    // check for a different stdout
    if (new_stdout.has_value()) {
        // this will essentially make STDOUT_FILENO point to the other file descriptor
        // since according to the man page it will close STDOUT_FILENO since it already exists
        if (_restore) {
            og_stdout = syscall_wrapper::dup_wrapper(syscall_wrapper::stdout_file_descriptor);
        }

        // we should have a value at this point
        assert(new_stdout.has_value());
        bool const status = syscall_wrapper::dup2_wrapper(new_stdout.value(), syscall_wrapper::stdout_file_descriptor);

        // check to make sure dup succeeded
        if (!status) {
            return;
        }

        // close new fd
        new_stdout = std::nullopt;
    }

    // check for a different stdin
    if (new_stdin.has_value()) {
        if (_restore) {
            og_stdin = syscall_wrapper::dup_wrapper(syscall_wrapper::stdin_file_descriptor);
        }

        // we should have a value at this point
        assert(new_stdin.has_value());
        bool const status = syscall_wrapper::dup2_wrapper(new_stdin.value(), syscall_wrapper::stdin_file_descriptor);

        // check to make sure dup succeeded
        if (!status) {
            return;
        }

        // close new fd
        new_stdin = std::nullopt;
    }

    // check for a different stderr
    if (new_stderr.has_value()) {
        if (_restore) {
            og_stderr = syscall_wrapper::dup_wrapper(syscall_wrapper::stderr_file_descriptor);
        }

        // we should have a value at this point
        assert(new_stderr.has_value());
        bool const status = syscall_wrapper::dup2_wrapper(new_stderr.value(), syscall_wrapper::stderr_file_descriptor);

        // check to make sure dup succeeded
        if (!status) {
            return;
        }

        // close new fd
        new_stderr = std::nullopt;
    }
}

process::shell_internal_redirection::~shell_internal_redirection() {
    if (_restore) {
        // check for a different stdout
        if (og_stdout.has_value()) {
            bool const status = syscall_wrapper::dup2_wrapper(og_stdout.value(), syscall_wrapper::stdout_file_descriptor);
            // check to make sure dup succeeded
            if (!status) {
                return;
            }
        }

        // check for a different stdin
        if (og_stdin.has_value()) {
            bool const status = syscall_wrapper::dup2_wrapper(og_stdin.value(), syscall_wrapper::stdin_file_descriptor);

            // check to make sure dup succeeded
            if (!status) {
                return;
            }
        }

        // check for a different stderr
        if (og_stderr.has_value()) {
            bool const status = syscall_wrapper::dup2_wrapper(og_stderr.value(), syscall_wrapper::stderr_file_descriptor);

            // check to make sure dup succeeded
            if (!status) {
                return;
            }
        }
    }
}

auto process::parse_process(std::string const& input) -> std::optional<std::unique_ptr<process_data>> {
    auto proc_data = std::make_unique<process_data>();

    // stack allocated variables
    std::string arg;
    std::vector<std::string> args;

    // file descriptors
    std::optional<file_descriptor_wrapper> proc_stdout = std::nullopt;
    std::optional<file_descriptor_wrapper> proc_stdin = std::nullopt;
    std::optional<file_descriptor_wrapper> proc_stderr = std::nullopt;

    // parse the command into different components
    static constexpr mode_t FILE_MODE = 0777;

    // parsing state
    std::stack<PARSE_STATE> curr_state{};
    // default behavior should be REGULAR mode
    curr_state.push(PARSE_STATE::REGULAR);
    for(char chr : input) {
        assert(!curr_state.empty()); // this should be true since we dont pop in REGULAR
        assert(static_cast<char>(curr_state.top()) < PARSE_STATE::COUNT);

        auto add_current_arg = [&](){
            if(!arg.empty()){
                args.emplace_back(std::move(arg));
                arg = std::string{};
            }
        };
        switch(curr_state.top()){
            case PARSE_STATE::REGULAR:{
                // if we encounter white space we want to move push back out new argument and clear it
                if(static_cast<bool>(std::isspace(chr))){
                    add_current_arg();
                }else if(chr == INPUT_REDIRECTION){ // transition to input filename
                    add_current_arg(); // the redirection characters will act like whitespace
                    curr_state.push(PARSE_STATE::INPUT_FILENAME);
                }else if(chr == OUTPUT_REDIRECTION){ // transition to output filename
                    add_current_arg(); // the redirection characters will act like whitespace
                    curr_state.push(PARSE_STATE::OUTPUT_FILENAME);
                }else if(chr == SINGLE_QUOTE){ // transition to single quote state
                    curr_state.push(PARSE_STATE::QUOTE);
                }else if(chr == DOUBLE_QUOTE){ // transition to double quote state
                    curr_state.push(PARSE_STATE::DBL_QUOTE);
                }else{ // append the character
                    arg += chr;
                }

                break;
            }
            case PARSE_STATE::INPUT_FILENAME:{
                // if we encounter a boundary we want create the filename
                if(static_cast<bool>(std::isspace(chr)) || chr == INPUT_REDIRECTION || chr == OUTPUT_REDIRECTION){
                    if (!arg.empty()) {
                        cout_logger.log(LOG_LEVEL::DEBUG, "Attempting to open file ", arg, " for reading...");
                        proc_stdin = syscall_wrapper::open_wrapper(arg, O_RDONLY, FILE_MODE);

                        if (!proc_stdin.has_value()) {
                            return std::nullopt;
                        }
                        curr_state.pop();
                    }

                    // if we are IO redirection then push another state
                    if(chr == INPUT_REDIRECTION){
                        curr_state.push(PARSE_STATE::INPUT_FILENAME);
                    }else if(chr == OUTPUT_REDIRECTION){
                        curr_state.push(PARSE_STATE::OUTPUT_FILENAME);
                    }
                    arg.clear();
                }else if(chr == SINGLE_QUOTE){ // transition to single quote state
                    curr_state.push(PARSE_STATE::QUOTE);
                }else if(chr == DOUBLE_QUOTE){ // transition to double quote state
                    curr_state.push(PARSE_STATE::DBL_QUOTE);
                }else{ // append the character
                    arg += chr;
                }
                break;
            }
            case PARSE_STATE::OUTPUT_FILENAME:{
                // if we encounter a boundary character we want create the filename
                if(static_cast<bool>(std::isspace(chr)) || chr == INPUT_REDIRECTION || chr == OUTPUT_REDIRECTION){
                    if (!arg.empty()) {
                        cout_logger.log(LOG_LEVEL::DEBUG, "Attempting to open file ", arg, " for writing...");
                        proc_stdout = syscall_wrapper::open_wrapper(arg, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);

                        if (!proc_stdout.has_value()) {
                            return std::nullopt;
                        }
                        curr_state.pop();
                    }

                    // if we are IO redirection then push another state
                    if(chr == INPUT_REDIRECTION){
                        curr_state.push(PARSE_STATE::INPUT_FILENAME);
                    }else if(chr == OUTPUT_REDIRECTION){
                        curr_state.push(PARSE_STATE::OUTPUT_FILENAME);
                    }
                    arg.clear();
                }else if(chr == SINGLE_QUOTE){ // transition to single quote state
                    curr_state.push(PARSE_STATE::QUOTE);
                }else if(chr == DOUBLE_QUOTE){ // transition to double quote state
                    curr_state.push(PARSE_STATE::DBL_QUOTE);
                }else{ // append the character
                    arg += chr;
                }
                break;
            }
            case PARSE_STATE::QUOTE:{
                if(chr == SINGLE_QUOTE){ // close single quote
                    curr_state.pop();
                }else{ // append the character
                    arg += chr;
                }
                break;
            }
            case PARSE_STATE::DBL_QUOTE:{
                if(chr == DOUBLE_QUOTE){ // close double quote
                    curr_state.pop();
                }else{ // append the character
                    arg += chr;
                }
                break;
            }
            case PARSE_STATE::COUNT:{
                // we should never get here
                assert(false);
                break;
            }
        }
    }

    // check to make sure for errors
    if(curr_state.top() == PARSE_STATE::QUOTE || curr_state.top() == PARSE_STATE::DBL_QUOTE){
        cout_logger.log(LOG_LEVEL::ERROR, "Invalid process input, unclosed quotation...");
        return std::nullopt;
    }
    if((curr_state.top() == PARSE_STATE::OUTPUT_FILENAME || curr_state.top() == PARSE_STATE::INPUT_FILENAME) && arg.empty()){
        cout_logger.log(LOG_LEVEL::ERROR, "No filename provided...");
        return std::nullopt;
    }

    // create leftover filenames
    if(curr_state.top() == PARSE_STATE::INPUT_FILENAME && !arg.empty()){
        proc_stdin = syscall_wrapper::open_wrapper(arg, O_RDONLY, FILE_MODE);

        if (!proc_stdin.has_value()) {
            return std::nullopt;
        }

        arg.clear();
    }

    if(curr_state.top() == PARSE_STATE::OUTPUT_FILENAME && !arg.empty()){
        proc_stdout = syscall_wrapper::open_wrapper(arg, O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE);

        if (!proc_stdout.has_value()) {
            return std::nullopt;
        }

        arg.clear();
    }

    // add any leftover arguments
    if(!arg.empty()){
        args.push_back(std::move(arg));
    }

    // search for shell built-ins
    // otherwise, treat as binary
    if (!args.empty() && args[0] == EXPORT_BUILTIN) { // export
        *proc_data = export_data{};

        assert(std::holds_alternative<export_data>(*proc_data));

        auto& data = std::get<export_data>(*proc_data);

        // set IO redirection
        data.stdout = std::move(proc_stdout);
        data.stdin = std::move(proc_stdin);
        data.stderr = std::move(proc_stderr);

        // find the variable name and value from the input
        // we should only have two items in the args list export and [variable name]=[value]
        cout_logger.log(LOG_LEVEL::DEBUG, "Args size: ", args.size());
        for(auto const& argument : args){
            cout_logger.log(LOG_LEVEL::DEBUG, "Export arg: ", argument);
        }

        if (args.size() < 2) {
            return std::nullopt;
        }


        // find the equals
        // indexing into this at 1 is fine since we know the size is at least 2
        std::size_t const idx = args[1].find(EQUALS);

        // ensure there is an = in the string
        if (idx == std::string::npos || idx + 1 >= args[1].size() || idx == 0) {
            return std::nullopt;
        }

        // get the variable name
        data.name = args[1].substr(0, idx);
        data.val = args[1].substr(idx + 1, args[1].size());
    } else { // regular binary
        *proc_data = binary_data{};

        assert(std::holds_alternative<binary_data>(*proc_data));

        auto& data = std::get<binary_data>(*proc_data);

        // set IO redirection
        data.stdout = std::move(proc_stdout);
        data.stdin = std::move(proc_stdin);
        data.stderr = std::move(proc_stderr);

        // give the variant ownership of the arguments
        data.args = std::move(args);
    }

    return proc_data;
}

void process::execute_process(binary_data& data) {
    // we should not accept any nullptrs to this function
    assert(data.pgid != nullptr);

    // the array of pointers to the command line arguments
    std::vector<char*> args_ptr;

    // push back all of the pointers to the arguments
    args_ptr.reserve(data.args.size());
    for (auto& arg : data.args) {
        args_ptr.push_back(arg.data());
    }

    // add the sentinal to the end of the arguments
    args_ptr.push_back(nullptr);

    // fork into another subprocess to execute the binary
    std::optional<pid_t> pid_op = syscall_wrapper::fork_wrapper();
    if (!pid_op.has_value()) {
        return;
    }
    pid_t const& pid = pid_op.value();

    if (static_cast<bool>(pid)) { // parent
        // update the process id for the new process if it is the first one/its pointer is nullptr
        if (*data.pgid == -1) {
            *data.pgid = pid;
        }

        // set the child process' process group id in parent to prevent race conditions
        bool status = syscall_wrapper::setpgid_wrapper(pid, *data.pgid);

        // error handle
        if (!status) {
            return;
        }

        // set the child process to control the terminal
        if (data.is_foreground) {
            status = syscall_wrapper::tcsetpgrp_wrapper(syscall_wrapper::stdin_file_descriptor, *data.pgid);

            if (!status) {
                return;
            }
        }

        // send continue signal to child process group
        status = syscall_wrapper::kill_wrapper(-*data.pgid, SIGCONT);

        if (!status) {
            return;
        }

        // wait for the process to complete
        // TODO (john): I cannot wait everytime there is a command executed if we are going to support background tasks
        int wait_status = 0;
        pid_t const exit_code = waitpid(pid, &wait_status, WUNTRACED);

        // close all of the older file descriptors
        data.stdout = std::nullopt;
        data.stdin = std::nullopt;
        data.stderr = std::nullopt;

        // check for errors
        if (exit_code != pid) {
            jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Waiting on PID ", pid, " failed: ", syscall_wrapper::strerror_wrapper(errno), '\n');
        } else {
            // save the exit status
            if (WIFEXITED(wait_status)) {
                std::string const exit_status = std::to_string(WEXITSTATUS(wait_status));
                environment::set_var(environment::STATUS_STRING, exit_status.c_str());
            }
        }

        // restore terminal control to the shell
        // since the shell is its process leader, we can just use getpid
        if (data.is_foreground) {
            std::optional<pid_t> cur_pgid = syscall_wrapper::getpid_wrapper();
            assert(cur_pgid.has_value());
            status = syscall_wrapper::tcsetpgrp_wrapper(syscall_wrapper::stdin_file_descriptor, cur_pgid.value());

            // error handle
            if (!status) {
                return;
            }
        }

        // get the jsh shell singleton
        std::optional<std::shared_ptr<shell>> shll_ptr = shell::get();

        // error handle
        if (!shll_ptr.has_value()) {
            return;
        }

        // send job cont signal
        assert(shll_ptr.has_value());
        status = syscall_wrapper::tcsetattr_wrapper(syscall_wrapper::stdin_file_descriptor, TCSADRAIN, shll_ptr.value()->get_term_if());

        if (!status) {
            return;
        }
    } else { // child
        // get the current PID
        std::optional<pid_t> cur_pid = syscall_wrapper::getpid_wrapper();
        assert(cur_pid.has_value()); // this should always pass since it getpid shouldn't fail

        // if the pgid is nullptr, then this is the first process and we should make its PID the
        // PGID since it will be the process leader
        if (*data.pgid == -1) {
            *data.pgid = cur_pid.value();
        }

        // set the current process' process group id
        // if this is the first process being launched then it will be the process leader and cur_pid.value() == data.pgid
        bool status = syscall_wrapper::setpgid_wrapper(cur_pid.value(), *data.pgid);

        // error handle
        if (!status) {
            return;
        }

        // if the process is running in the foreground, then it gets access to the terminal
        if (data.is_foreground) {
            status = syscall_wrapper::tcsetpgrp_wrapper(syscall_wrapper::stdin_file_descriptor, *data.pgid);

            // error handle
            if (!status) {
                return;
            }
        }

        // listen to job control signals
        std::optional<std::function<void(int)>> sig_status;
        sig_status = syscall_wrapper::signal_wrapper(SIGINT, SIG_DFL); // termination requests

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        sig_status = syscall_wrapper::signal_wrapper(SIGQUIT, SIG_DFL);

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        sig_status = syscall_wrapper::signal_wrapper(SIGTSTP, SIG_DFL);

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        sig_status = syscall_wrapper::signal_wrapper(SIGTTIN, SIG_DFL);

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        sig_status = syscall_wrapper::signal_wrapper(SIGTTOU, SIG_DFL);

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        { // sir scope
            shell_internal_redirection const sir(std::move(data.stdout), std::move(data.stdin), std::move(data.stderr), false);

            int const exit_code = execvp(args_ptr[0], args_ptr.data());

            // execvp only returns if there was an error
            assert(exit_code == -1);
        } // sir scope

        // log that this command was invalid
        jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Executing command failed...");
    }
}

void process::execute_process(export_data& data) {
    { // sir scope
        shell_internal_redirection const sir(std::move(data.stdout), std::move(data.stdin), std::move(data.stderr));

        // perform the export
        environment::set_var(data.name.c_str(), data.val.c_str());

        // set return env var to 0
        environment::set_var(environment::STATUS_STRING, environment::SUCCESS_STRING);
    } // sir scope
}

void process::execute(std::unique_ptr<process_data>& data) {
    // execute on the given data
    std::visit([](auto&& var) {
        jsh::process::execute_process(var);
    },
               *data);
}
} // namespace jsh
