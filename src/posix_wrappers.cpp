#include "posix_wrappers.hpp"

namespace jsh {
    named_pipe_wrapper::named_pipe_wrapper(std::string pipe_name) : success{true}, name{std::move(pipe_name)}{
        // create the pipe
        if(mkfifo(name.c_str(), PIPE_PERMS) == -1){
            std::cout << "Error making pipe: " << strerror(errno) << '\n';
            success = false;
        }
    }
    named_pipe_wrapper::~named_pipe_wrapper(){
        // if pipe was created delete it
        if(success){
            if(unlink(name.c_str()) == -1){
                std::cout << "Error removing pipe: " << strerror(errno) << '\n';
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
            _fides = other._fides;
            other._fides = -1;
        }
        
        // return current object
        return *this;
    }

    file_descriptor_wrapper::file_descriptor_wrapper(int fides) : _fides{fides}{

    }

    ~file_descriptor_wrapper();

    ///// FILE DESCRIPTOR WRAPPER /////
} // namespace jsh
