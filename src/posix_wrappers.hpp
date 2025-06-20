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
} // namespace jsh
