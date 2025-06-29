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

    file_descriptor_wrapper::file_descriptor_wrapper(int fides) : _fides{fides}{}

    file_descriptor_wrapper::~file_descriptor_wrapper(){
        // close the file descriptor
        int status = close(_fides);

        // check the status on close
        if(status == -1){
            cout_logger.log(LOG_LEVEL::ERROR, "Error occurred while closing file: ", strerror(errno));
        }
    }
    ///// FILE DESCRIPTOR WRAPPER /////

    auto syscall_wrapper::open_wrapper(std::string const& file, int flags, mode_t perms) -> std::optional<file_descriptor_wrapper>{
        // open the file
        int fides = open(file.c_str(), flags, perms);

        // check for errors
        if(fides == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Error occurred while opening file ", file, ": ", strerror(errno));
            return std::nullopt;
        }

        // create the file descriptor wrapper using the valid file descriptor
        file_descriptor_wrapper fdw(fides);

        // create RAII wrapper
        return std::make_optional<file_descriptor_wrapper>(std::move(fdw));

    }
} // namespace jsh
