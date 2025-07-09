#pragma once

// JSH
#include "macros.hpp"
#include <unistd.h>

namespace jsh {
    /**
     * named_pipe_wrapper uses RAII to ensure that a created pipe is deleted
     */
    class named_pipe_wrapper {
    private:
        /**
         * constants
         */
        static constexpr mode_t PIPE_PERMS = 0666;
        
        /**
         * indicates whether the pipe was successfully create or not
         */
        bool success;
        
        /**
         * name of the pipe
         */
        std::string name;

    public:

        /**
         * creates and destroys a named pipe along with the lifetime of this object
         *
         * pipe_name: the filepath for the new pipe
         */
        explicit named_pipe_wrapper(std::string pipe_name);

        ~named_pipe_wrapper();
    };

    /**
     * file_descriptor_wrapper uses RAII to ensure that all file descriptors are closed
     */
    class syscall_wrapper;
    class file_descriptor_wrapper {
    private:
        /**
         * fides: the file descriptor which this class is wrapping
         *
         * note: a file descriptor of -1 means that it is invalid
         */
        int _fides;

        /**
         * creates and destroys a named pipe along with the lifetime of this object
         *
         * pipe_name: the filepath for the new pipe
         *
         * note: the reason this constructor was made private was to prevent non-friend classes from creating file descriptor wrappers
         */
        explicit file_descriptor_wrapper(int fides) noexcept;

    public:

        /**
         * equality operators for assertions
         */
        auto operator!=(file_descriptor_wrapper const& other) const -> bool;
        auto operator==(file_descriptor_wrapper const& other) const -> bool;

        /**
         * delete copy and assignment operator
         */
        file_descriptor_wrapper(file_descriptor_wrapper const& other) = delete;
        file_descriptor_wrapper& operator=(file_descriptor_wrapper const& other) = delete;

        /**
         * define move semantics
         */
        file_descriptor_wrapper(file_descriptor_wrapper&& other) noexcept;
        auto operator=(file_descriptor_wrapper&& other) noexcept -> file_descriptor_wrapper&;

        /**
         * close file descriptors
         */
        ~file_descriptor_wrapper();


        friend syscall_wrapper;
    };

    /**
     * syscall_wrapper: a class full of static functions which safely wrap all the C-style syscalls
     */
    class syscall_wrapper {
    public:
        /**
         * definitions for stdout, stdin, stderr file_descriptor_wrappers
         */
        inline static file_descriptor_wrapper STDOUT_FILE_DESCRIPTOR = file_descriptor_wrapper(STDOUT_FILENO);
        inline static file_descriptor_wrapper STDIN_FILE_DESCRIPTOR = file_descriptor_wrapper(STDIN_FILENO);
        inline static file_descriptor_wrapper STDERR_FILE_DESCRIPTOR = file_descriptor_wrapper(STDERR_FILENO);

        /**
         * open_wrapper: a wrapper around the open syscall in order to interface properly with the file_descriptor_wrapper
         */
        [[nodiscard]] static auto open_wrapper(std::string const& file, int flags, mode_t perms) -> std::optional<file_descriptor_wrapper>;

        /**
         * dup_wrapper: a wrapper around the dup syscall in order to interface properly with the file_descriptor_wrapper
         */
        [[nodiscard]] static auto dup_wrapper(file_descriptor_wrapper const& fides) -> std::optional<file_descriptor_wrapper>;

        /**
         * dup2_wrapper: a wrapper around the dup2 syscall in order to interface properly with the file_descriptor_wrapper
         */
        [[nodiscard]] static auto dup2_wrapper(file_descriptor_wrapper const& fides1, file_descriptor_wrapper const& fides2) -> bool;

        /**
         * pipe_wrapper: a wrapper around the pipe syscall in order to interface properly with the file_descriptor_wrapper
         */
        [[nodiscard]] static auto pipe_wrapper() -> std::optional<std::vector<file_descriptor_wrapper>>;

        /**
         * read: a wrapper around the read syscall in order to interface properly with the file_descriptor_wrapper
         */
        [[nodiscard]] static auto read_wrapper(file_descriptor_wrapper const& fides, void* buf, std::size_t count) -> std::optional<ssize_t>;

        /**
         * isatty_wrapper: a wrapper which allows for error handling on the isatty sycall
         */
        [[nodiscard]] static auto isatty_wrapper(file_descriptor_wrapper const& fides) -> bool;

        /**
         * tcgetpgrp: a wrapper which safely handles errors for the tgetpgrp function
         */
        [[nodiscard]] static auto tcgetpgrp_wrapper(file_descriptor_wrapper const& fides) -> std::optional<pid_t>;

        /**
         * getpgrp_wrapper: a wrapper around getpgrp which handles errors and interfaces with the file descriptor wrapper properly
         */
        [[nodiscard]] static auto getpgrp_wrapper() -> std::optional<pid_t>;

        /**
         * getpid: a wrapper around the getpid syscall
         */
        [[nodiscard]] static auto getpid_wrapper() -> std::optional<pid_t>;

        /**
         * setpgid_wrapper: a wrapper around the setpgid syscall in order to handle errors properly
         */
        [[nodiscard]] static auto setpgid_wrapper(pid_t pid, pid_t pgid)-> bool;

        /**
         * tcsetpgrp_wrapper: wrapper around the tcsetpgrp syscall in order to interface with wrappers while grabbing control of the terminal
         */
        [[nodiscard]] static auto tcsetpgrp_wrapper(file_descriptor_wrapper const& term_fides, pid_t shell_pid) -> bool;

        /**
         * tcgetattr_wrapper: wrapper around getting the terminal interface
         */
        [[nodiscard]] static auto tcgetattr_wrapper(file_descriptor_wrapper const& term_fides, std::shared_ptr<termios> term_if) -> bool;

        /**
         * tcsetattr_wrapper: wrapper around setting the terminal interace
         */
        [[nodiscard]] static auto tcsetattr_wrapper(file_descriptor_wrapper const& term_fides, int opts, std::shared_ptr<termios> const& term_if) -> bool;

        /**
         * kill_wrapper: wrapper around the kill syscall
         */
        [[nodiscard]] static auto kill_wrapper(pid_t pid, int sig) -> bool;

        /**
         * signal_wrapper: wrapper around the signal syscall
         */
        [[nodiscard]] static auto signal_wrapper(int sig, void(*func)(int)) -> std::optional<std::function<void(int)>>;
    };
} // namespace jsh
