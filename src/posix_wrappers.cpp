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

    file_descriptor_wrapper::file_descriptor_wrapper(int fides) noexcept : _fides{fides}{}

    file_descriptor_wrapper::~file_descriptor_wrapper(){
        // close the file descriptor (not stdout, stdin, or stderr)
        if(_fides != STDOUT_FILENO && _fides != STDIN_FILENO && _fides != STDERR_FILENO){
            int status = close(_fides);
            // check the status on close
            if(status == -1){
                cout_logger.log(LOG_LEVEL::ERROR, "Error occurred while closing file: ", strerror(errno));
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

    auto syscall_wrapper::dup_wrapper(file_descriptor_wrapper const& fides) -> std::optional<file_descriptor_wrapper>{
        // call dup on the current file descriptor
        int new_fides = dup(fides._fides);

        // check to see if there was an error
        if(new_fides == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Error occurred while duplicating a file descriptor: ", strerror(errno));
            return std::nullopt;
        }

        // create the new file descriptor
        return std::make_optional<file_descriptor_wrapper>(file_descriptor_wrapper(new_fides));
    }

    auto syscall_wrapper::dup2_wrapper(file_descriptor_wrapper const& fides1, file_descriptor_wrapper const& fides2) -> bool{
        // make the dup2 syscall
        int status = dup2(fides1._fides, fides2._fides);

        // check to see if there was an error
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Error occurred while duplicating2 a file descriptor: ", strerror(errno));
            return false;
        }
        
        // success
        return true;
    }

    auto syscall_wrapper::pipe_wrapper() -> std::optional<std::array<file_descriptor_wrapper, 2>>{
        // pipe fds
        std::array<int, 2> fides{};
        int status = pipe(fides.data());

        // check for errors
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Creating a anonymous pipe: ", strerror(errno));
            return std::nullopt;
        }

        // create the fides wrappers
        std::array<file_descriptor_wrapper, 2> fides_wrappers{file_descriptor_wrapper(fides[0]), file_descriptor_wrapper(fides[1])};

        return std::make_optional<std::array<file_descriptor_wrapper, 2>>(std::move(fides_wrappers));
    }
    
    auto syscall_wrapper::read_wrapper(file_descriptor_wrapper const& fides, void* buf, std::size_t count) -> bool{
        // perform the read
        ssize_t status = read(fides._fides, buf, count);

        // error handle
        if(status == -1){
            cout_logger.log(jsh::LOG_LEVEL::ERROR, "Error while reading file descriptor: ", strerror(errno));
            return false;
        }

        // success
        return true;
    }
} // namespace jsh
