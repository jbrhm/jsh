#include "posix_wrappers.hpp"


namespace jsh {
    named_pipe_wrapper::named_pipe_wrapper(std::string pipe_name) : success{true}, name{std::move(pipe_name)}{
        // create the pipe
        if(mkfifo(name.c_str(), PIPE_PERMS) == -1){
            std::cout << "Error making pipe: " << syscall_wrapper::strerror_wrapper(errno) << '\n';
            success = false;
        }
    }
    named_pipe_wrapper::~named_pipe_wrapper(){
        // if pipe was created delete it
        if(success){
            if(unlink(name.c_str()) == -1){
                std::cout << "Error removing pipe: " << syscall_wrapper::strerror_wrapper(errno) << '\n';
            }
        }
    }

    ///// FILE DESCRIPTOR WRAPPER /////

    file_descriptor_wrapper::file_descriptor_wrapper(file_descriptor_wrapper&& other) noexcept : _fides{other._fides}{
        // set the other fdw to invalid
        other._fides = -1;
    }
    auto file_descriptor_wrapper::operator=(file_descriptor_wrapper&& other) noexcept -> file_descriptor_wrapper&{
        // ensure we are nove being moved into ourselves
        if(this != &other){
            // we need to close any currently open file descriptors
            if(_fides != -1){
                close(_fides);
            }
            _fides = other._fides;
            other._fides = -1;
        }
        
        // return current object
        return *this;
    }

    file_descriptor_wrapper::file_descriptor_wrapper(int fides) noexcept : _fides{fides}{
        //std::cout << "Creating: " << _fides << '\n';
    }

    file_descriptor_wrapper::~file_descriptor_wrapper(){
        // close the file descriptor (not stdout, stdin, or stderr)
        if(_fides != -1){
	    //std::cout << "Closing: " << _fides << '\n';
            int const status = close(_fides);
            // check the status on close
            if(status == -1){
                cout_logger.log(LOG_LEVEL::ERROR, "Error occurred while closing file: ", syscall_wrapper::strerror_wrapper(errno));
            }
        }
    }

    auto file_descriptor_wrapper::operator!=(file_descriptor_wrapper const& other) const -> bool{
        return _fides != other._fides;
    }

    auto file_descriptor_wrapper::operator==(file_descriptor_wrapper const& other) const -> bool{
        return _fides == other._fides;
    }

    ///// FILE DESCRIPTOR WRAPPER /////

    //// SYSCALL WRAPPER /////

    std::array<char, ERR_BUF_SIZE> syscall_wrapper::err_buf{};

    auto syscall_wrapper::open_wrapper(std::string const& file, int flags, mode_t perms) -> std::optional<file_descriptor_wrapper>{
        // open the file
        int const fides = open(file.c_str(), flags, perms);

        // check for errors
        if(fides == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Error occurred while opening file ", file, ": ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // create the file descriptor wrapper using the valid file descriptor
        file_descriptor_wrapper fdw(fides);

        // create RAII wrapper
        return std::make_optional<file_descriptor_wrapper>(std::move(fdw));
    }

    auto syscall_wrapper::dup_wrapper(file_descriptor_wrapper const& fides) -> std::optional<file_descriptor_wrapper>{
        // call dup on the current file descriptor
        int const new_fides = fcntl(fides._fides, F_DUPFD_CLOEXEC);

        // check to see if there was an error
        if(new_fides == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Error occurred while duplicating a file descriptor: ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // create the new file descriptor
        return std::make_optional<file_descriptor_wrapper>(file_descriptor_wrapper(new_fides));
    }

    auto syscall_wrapper::dup2_wrapper(file_descriptor_wrapper const& fides1, file_descriptor_wrapper const& fides2) -> bool{
        // make the dup2 syscall
        int const status = dup2(fides1._fides, fides2._fides);

        // check to see if there was an error
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Error occurred while duplicating2 a file descriptor: ", strerror_wrapper(errno));
            return false;
        }
        
        // success
        return true;
    }

    auto syscall_wrapper::pipe_wrapper() -> std::optional<std::vector<file_descriptor_wrapper>>{
        // pipe fds
        std::array<int, 2> fides{};
        int const status = pipe2(fides.data(), O_CLOEXEC);

        // check for errors
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Creating a anonymous pipe: ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // create the fides wrappers
        std::vector<file_descriptor_wrapper> fides_wrappers;
	fides_wrappers.reserve(2);
	fides_wrappers.emplace_back(file_descriptor_wrapper(fides[0]));
	fides_wrappers.emplace_back(file_descriptor_wrapper(fides[1]));

        return std::make_optional<std::vector<file_descriptor_wrapper>>(std::move(fides_wrappers));
    }
    
    auto syscall_wrapper::read_wrapper(file_descriptor_wrapper const& fides, void* buf, std::size_t count) -> std::optional<ssize_t>{
        // perform the read
        ssize_t status = read(fides._fides, buf, count);

        // error handle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Error while reading file descriptor: ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // success
        return std::make_optional<ssize_t>(status);
    }

    auto syscall_wrapper::isatty_wrapper(file_descriptor_wrapper const& fides) -> bool{
        // check if jsh is a terminal
        int const status = isatty(fides._fides);

        // error handle
        if(status != 1){
            cout_logger.log(jsh::LOG_LEVEL::WARN, "JSH is not running as a terminal: ", strerror_wrapper(errno));
            return false;
        }

        // success
        return true;
    }

    auto syscall_wrapper::tcgetpgrp_wrapper(file_descriptor_wrapper const& fides) -> std::optional<pid_t>{
        // run
        pid_t tc_grp_pid = tcgetpgrp(fides._fides);

        // check for errors
        if(tc_grp_pid == -1){
            cout_logger.log(jsh::LOG_LEVEL::WARN, "Failed to get the process group controlling the terminal: ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // success
        return std::make_optional<pid_t>(tc_grp_pid);
    }

    auto syscall_wrapper::getpgrp_wrapper() -> std::optional<pid_t>{
        // run function
        pid_t pgrp_id = getpgrp();

        // error handle
        if(pgrp_id == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to get the process group id: ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // success
        return std::make_optional<pid_t>(pgrp_id);
    }
    
    auto syscall_wrapper::getpid_wrapper() -> std::optional<pid_t>{
        // according to the man pages this function never failes
        // I just want a common interface
        return std::make_optional<pid_t>(getpid());
    }

    auto syscall_wrapper::setpgid_wrapper(pid_t pid, pid_t pgid)-> bool{
        // try and set pgid
        int const status = setpgid(pid, pgid);

        // error handle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to set the process' process group id: ", strerror_wrapper(errno));
            return false;
        }

        // success
        return true;
    }

    auto syscall_wrapper::tcsetpgrp_wrapper(file_descriptor_wrapper const& term_fides, pid_t shell_pid) -> bool{
        // get control over the terminal
        int const status = tcsetpgrp(term_fides._fides, shell_pid);

        // error hangle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to set the terminal's controlling process group: ", strerror_wrapper(errno));
            return false;
        }

        // success
        return true;
    }

    auto syscall_wrapper::tcgetattr_wrapper(file_descriptor_wrapper const& term_fides, std::shared_ptr<termios> const& term_if) -> bool{
        // get the interface
        int const status = tcgetattr(term_fides._fides, term_if.get());

        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to get the terminal's interface attribute: ", strerror_wrapper(errno));
            return false;
        }

        // success
        return true;
    }

    auto syscall_wrapper::tcsetattr_wrapper(file_descriptor_wrapper const& term_fides, int opts, std::shared_ptr<termios> const& term_if) -> bool{
        // set the interface
        int const status = tcsetattr(term_fides._fides, opts, term_if.get());

        // error handle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to set the terminal's interface attribute: ", strerror_wrapper(errno));
            return false;
        }

        // success
        return true;
    }

    auto syscall_wrapper::kill_wrapper(pid_t pid, int sig) -> bool{
        // send signal
        int const status = kill(pid, sig);

        // error handle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to send signal: ", strerror_wrapper(errno));
            return false;
        }

        return true;
    }

    auto syscall_wrapper::signal_wrapper(int sig, void(*func)(int)) -> std::optional<std::function<void(int)>>{
        // set the signal handler
        void(*ret)(int) = signal(sig, func);

        // error handle
        if(ret == SIG_ERR){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to set signal handler: ", strerror_wrapper(errno));
            return std::nullopt;
        }
        return std::make_optional<std::function<void(int)>>(ret);
    }
    
    auto syscall_wrapper::sigemptyset_wrapper(sigset_t& sigset) -> bool{
        // empty the signal set
        int const status = sigemptyset(&sigset);

        // error handle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to empty signal set: ", strerror_wrapper(errno));
            return false;
        }

        // success
        return true;
    }

    auto syscall_wrapper::sigaddset_wrapper(sigset_t& sigset, int signo) -> bool{
        // add the signal
        int const status = sigaddset(&sigset, signo);

        // error handle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to add signal to set: ", strerror_wrapper(errno));
            return false;
        }

        // success
        return true;
    }

    auto syscall_wrapper::sigprocmask_wrapper(int how, sigset_t& set) -> bool{
        // add signal process mask
        int const status = pthread_sigmask(how, &set, nullptr);

        // error handle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to create process signal mask: ", strerror_wrapper(errno));
            return false;
        }

        // success
        return true;
    }

    auto syscall_wrapper::signalfd_wrapper(file_descriptor_wrapper const& fides, sigset_t& set, int flags) -> std::optional<file_descriptor_wrapper>{
        // create the signal file
        int const new_signal_fd = signalfd(fides._fides, &set, flags);

        // error handle
        if(new_signal_fd == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to create signal file descriptor: ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // create the new file descriptor
        return std::make_optional<file_descriptor_wrapper>(file_descriptor_wrapper(new_signal_fd));
    }

    auto syscall_wrapper::poll_wrapper(std::vector<pollfd_wrapper>& fds, int timeout) -> std::optional<int>{
        // create the poll fds
        std::vector<pollfd> poll_fds;
        poll_fds.reserve(fds.size());
        for(auto const& fdw : fds){
            poll_fds.emplace_back(fdw.fides._fides, fdw.events, 0);
        }

        // poll the fds
        int num_fds = poll(poll_fds.data(), fds.size(), timeout);

        // error handle
        if(num_fds == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to poll file descriptors: ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // copy over the revents
        for(std::size_t i = 0; i < fds.size(); ++i){
            fds[i].revents = poll_fds[i].revents;
        }

        // create the new file descriptor
        return std::make_optional<int>(num_fds);
    }

    auto syscall_wrapper::fork_wrapper() -> std::optional<pid_t>{
        // fork
        pid_t const pid = fork();

        // error handle
        if(pid == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to fork file descriptors: ", strerror_wrapper(errno));
            return std::nullopt;
        }

        // success
        return std::make_optional<pid_t>(pid);
    }

    auto syscall_wrapper::strerror_wrapper(int err) -> bool{
        // call strerror_r
        char const* status = strerror_r(err, err_buf.data(), err_buf.size());

        // error handle
        if(status == nullptr){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed convert error to string: ", strerror_r(err, err_buf.data(), err_buf.size()));
            return false;
        }

        // success
        return true;
    }
    ///// SYSCALL WRAPPER /////
} // namespace jsh
