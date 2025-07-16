#include "shell.hpp"

namespace jsh {
    std::optional<std::shared_ptr<shell>> shell::shell_ptr = std::nullopt;

shell::shell() : is_interactive{syscall_wrapper::isatty_wrapper(syscall_wrapper::stdin_file_descriptor)} {
    // check to see if jsh is interactive
    std::optional<pid_t> cur_grp_pid = std::nullopt;
    term_if = std::make_shared<termios>(); // structure describing the terminal interface

    // set the previous exit status to be zero
    environment::set_var(environment::STATUS_STRING, environment::SUCCESS_STRING);

    // interactive shell setup
    if (is_interactive) {
        // loop until jsh is in the foreground
        while (true) {
            // get the process group id controlling the terminal
            std::optional<pid_t> tc_grp_pid = syscall_wrapper::tcgetpgrp_wrapper(syscall_wrapper::stdin_file_descriptor);

            // get the current process' group id
            cur_grp_pid = syscall_wrapper::getpgrp_wrapper();

            // error handle
            if (!tc_grp_pid.has_value() || !cur_grp_pid.has_value()) {
                continue;
            }

            // no more errors
            assert(tc_grp_pid.has_value());
            assert(cur_grp_pid.has_value());

            // we are the process in the foreground of the terminal
            if (tc_grp_pid.value() == cur_grp_pid.value()) {
                break;
            }

            // try and kill the process controlling the terminal
            kill(-cur_grp_pid.value(), SIGTTIN);
        }

        // ignore all of the job control signals
        std::optional<std::function<void(int)>> sig_status;
        sig_status = syscall_wrapper::signal_wrapper(SIGINT, &shell::ctrl_c_signal_handler); // termination requests

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        sig_status = syscall_wrapper::signal_wrapper(SIGQUIT, SIG_IGN);

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        sig_status = syscall_wrapper::signal_wrapper(SIGTSTP, SIG_IGN);

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        sig_status = syscall_wrapper::signal_wrapper(SIGTTIN, SIG_IGN);

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        sig_status = syscall_wrapper::signal_wrapper(SIGTTOU, SIG_IGN);

        // error handle
        if (!sig_status.has_value()) {
            return;
        }

        // create a process group for the shell
        cur_grp_pid = syscall_wrapper::getpid_wrapper();
        assert(cur_grp_pid.has_value()); // this function shouldn't fail

        // if we cant set into our own process group exit
        if (!syscall_wrapper::setpgid_wrapper(cur_grp_pid.value(), cur_grp_pid.value())) {
            throw std::runtime_error("Failed to set process group id...");
        }

        // try and grab control of the shell
        if (!syscall_wrapper::tcsetpgrp_wrapper(syscall_wrapper::stdin_file_descriptor, cur_grp_pid.value())) {
            throw std::runtime_error("Failed to grab control of terminal shell...");
        }

        // get the shell attributes
        if (!syscall_wrapper::tcgetattr_wrapper(syscall_wrapper::stdin_file_descriptor, term_if)) {
            throw std::runtime_error("Failed to get shell attributes...");
        }
    } else {
        cout_logger.log(LOG_LEVEL::ERROR, "JSH must run interactively...");
    }
}

auto shell::get() -> std::optional<std::shared_ptr<shell>> {
    // creating the shell instance may throw an exception
    try {
        // check to see if we need to make the singleton instance
        if (!shell_ptr.has_value()) {
            // create the singleton instance
            shell_ptr = std::make_optional<std::shared_ptr<shell>>(std::make_shared<shell>());
        }
    } catch (std::runtime_error const& err) {
        // print error
        jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, err.what(), '\n');
        return std::nullopt;
    }

    // we should never return a nullptr, since the shell should always be valid
    assert(shell_ptr.has_value());
    return shell_ptr;
}

auto shell::execute_command() -> bool {
    // stack vars
    std::string input;

    // get the command from the user
    jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, PROMPT_MESSAGE);

    // create the signal file descriptor for SIGINT
    sigset_t sigs;

    bool const sigset_status = syscall_wrapper::sigemptyset_wrapper(sigs);
    if (!sigset_status) {
        return false;
    }

    bool const sigadd_status = syscall_wrapper::sigaddset_wrapper(sigs, SIGINT);
    if (!sigadd_status) {
        return false;
    }

    // block the signal handlers
    bool const sigprocmask_status = syscall_wrapper::sigprocmask_wrapper(SIG_BLOCK, sigs);
    if (!sigprocmask_status) {
        return false;
    }

    // create signal fd
    std::optional<file_descriptor_wrapper> sigfd_op = syscall_wrapper::signalfd_wrapper(syscall_wrapper::invalid_file_descriptor, sigs, 0);
    if (!sigfd_op.has_value()) {
        return false;
    }
    file_descriptor_wrapper const sigfd = std::move(sigfd_op.value());

    // poll the file descriptors
    std::vector<syscall_wrapper::pollfd_wrapper> pfds;
    pfds.emplace_back(sigfd, POLLIN, 0);
    pfds.emplace_back(syscall_wrapper::stdin_file_descriptor, POLLIN, 0);

    // poll the fds
    std::optional<int> const num_fds = syscall_wrapper::poll_wrapper(pfds, -1);
    if (!num_fds.has_value()) {
        return false;
    }

    if (pfds[0].revents != 0) {
        signalfd_siginfo sigfdinfo{};

        ssize_t total_read = 0;

        while (total_read != sizeof(sigfdinfo)) {
            std::optional<ssize_t> num_read = syscall_wrapper::read_wrapper(sigfd, &sigfdinfo, sizeof(sigfdinfo));

            // err handle
            if (!num_read.has_value()) {
                return false;
            }

            // increment
            total_read = total_read + num_read.value();
        }

        // we should only ever recieve a SIGINT
        assert(sigfdinfo.ssi_signo == SIGINT);

        // add new return
        jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, '\n');
    } else if (pfds[1].revents != 0) {
        std::getline(std::cin, input);
    }

    jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Raw user input: ", input);
    input = jsh::parsing::variable_substitution(input);
    jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Substituted user input: ", input);

    // exit jsh on exit keyword
    if (input == "exit" || input == "\nexit" || input == "exit\n" || input == "\nexit\n") {
        return false;
    }

    // parse the job
    auto job_data = jsh::job::parse_job(input);

    // execute the job
    jsh::job::execute_job(job_data);

    // success
    return true;
}

auto shell::get_term_if() -> std::shared_ptr<termios> {
    return term_if;
}

void shell::ctrl_c_signal_handler(int sig) {
    // this handler should only ever run for ctrl+c
    assert(sig == SIGINT);

    // reprint the prompt message
    std::cout << PROMPT_MESSAGE;
}

shell::~shell() = default;
} // namespace jsh
