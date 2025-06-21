#pragma once

// UNIX
#include <sys/stat.h>
#include <unistd.h>

// STL
#include <string>
#include <string.h>
#include <iostream>

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
        explicit file_descriptor_wrapper(int fides);

    public:
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
    };
} // namespace jsh
