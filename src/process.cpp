#include "process.hpp"
#include "posix_wrappers.hpp"
#include <optional>


namespace jsh {
    process::shell_internal_redirection::shell_internal_redirection(std::optional<file_descriptor_wrapper> stdout, std::optional<file_descriptor_wrapper> stdin, std::optional<file_descriptor_wrapper> stderr, bool restore) : new_stdout{std::move(stdout)}, new_stdin{std::move(stdin)}, new_stderr{std::move(stderr)}, og_stdout{std::nullopt}, og_stdin{std::nullopt}, og_stderr{std::nullopt}, _restore{restore}{
        // check for a different stdout
        if(new_stdout.has_value()){
            // this will essentially make STDOUT_FILENO point to the other file descriptor
            // since according to the man page it will close STDOUT_FILENO since it already exists
            if(_restore){
                og_stdout = syscall_wrapper::dup_wrapper(syscall_wrapper::STDOUT_FILE_DESCRIPTOR);
            }

            // we should have a value at this point
            assert(new_stdout.has_value());
            bool status = syscall_wrapper::dup2_wrapper(new_stdout.value(), syscall_wrapper::STDOUT_FILE_DESCRIPTOR);

            // check to make sure dup succeeded
            if(!status){
                return;
            }

            // close new fd
            new_stdout = std::nullopt;
        }

        // check for a different stdin
        if(new_stdin.has_value()){
            if(_restore){
                og_stdin = syscall_wrapper::dup_wrapper(syscall_wrapper::STDIN_FILE_DESCRIPTOR);
            }

            // we should have a value at this point
            assert(new_stdin.has_value());
            bool status = syscall_wrapper::dup2_wrapper(new_stdin.value(), syscall_wrapper::STDIN_FILE_DESCRIPTOR);

            // check to make sure dup succeeded
            if(!status){
                return;
            }

            // close new fd
            new_stdin = std::nullopt;
        }

        // check for a different stderr
        if(new_stderr.has_value()){
            if(_restore){
                og_stderr = syscall_wrapper::dup_wrapper(syscall_wrapper::STDERR_FILE_DESCRIPTOR);
            }

            // we should have a value at this point
            assert(new_stderr.has_value());
            bool status = syscall_wrapper::dup2_wrapper(new_stderr.value(), syscall_wrapper::STDERR_FILE_DESCRIPTOR);

            // check to make sure dup succeeded
            if(!status){
                return;
            }

            // close new fd
            new_stderr = std::nullopt;
        }
    }

    process::shell_internal_redirection::~shell_internal_redirection(){
        if(_restore){
            // check for a different stdout
            if(og_stdout.has_value()){
                bool status = syscall_wrapper::dup2_wrapper(og_stdout.value(), syscall_wrapper::STDOUT_FILE_DESCRIPTOR);
                // check to make sure dup succeeded
                if(!status){
                    return;
                }
            }

            // check for a different stdin
            if(og_stdin.has_value()){
                bool status = syscall_wrapper::dup2_wrapper(og_stdin.value(), syscall_wrapper::STDIN_FILE_DESCRIPTOR);

                // check to make sure dup succeeded
                if(!status){
                    return;
                }
            }

            // check for a different stderr
            if(new_stderr.has_value()){
                bool status = syscall_wrapper::dup2_wrapper(og_stderr.value(), syscall_wrapper::STDERR_FILE_DESCRIPTOR);

                // check to make sure dup succeeded
                if(!status){
                    return;
                }
            }
        }
    }

    auto process::parse_process(std::string const& input) -> std::optional<std::unique_ptr<process_data>>{
        // TODO: find a way to prevent this from defaulting to export
        auto proc_data = std::make_unique<process_data>();

        // stack allocated variables
        std::string arg;
        std::stringstream ss(input);
        std::vector<std::string> args;

        // file descriptors
        std::optional<file_descriptor_wrapper> proc_stdout = std::nullopt;
        std::optional<file_descriptor_wrapper> proc_stdin = std::nullopt;
        std::optional<file_descriptor_wrapper> proc_stderr = std::nullopt;

        // parse the command into different components
        while(ss >> arg){
            // search for input and output redirection
            std::string filename;
            if(arg.size() == 1 && arg[0] == INPUT_REDIRECTION){
                // get the input file descriptor
                if(!(ss >> filename)){
                    cout_logger.log(LOG_LEVEL::ERROR, "Input file not specified...");
                    return std::nullopt;
                }
                proc_stdin = syscall_wrapper::open_wrapper(filename, O_RDWR, 0777);

                if(!proc_stdin.has_value()){
                    return std::nullopt;
                }
                
                continue;
            }

            if(arg.size() == 1 && arg[0] == OUTPUT_REDIRECTION){
                // get the output file descriptor
                if(!(ss >> filename)){
                    cout_logger.log(LOG_LEVEL::ERROR, "Output file not specified...");
                    return std::nullopt;
                }
                proc_stdout = syscall_wrapper::open_wrapper(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);

                if(!proc_stdout.has_value()){
                    return std::nullopt;
                }

                continue;
            }

            // if not IO redirection push back as command line arg
            args.push_back(arg);
        }

        // search for shell built-ins
        // otherwise, treat as binary
        if(!args.empty() && args[0] == EXPORT_BUILTIN){ // export
            *proc_data = export_data{};

            assert(std::holds_alternative<export_data>(*proc_data));

            export_data& data = std::get<export_data>(*proc_data);

            // set IO redirection
            data.stdout = std::move(proc_stdout);
            data.stdin = std::move(proc_stdin);
            data.stderr = std::move(proc_stderr);

            // find the variable name and value from the input
            // we should only have two items in the args list export and [variable name]=[value]
            if(args.size() < 2){
                return std::nullopt;
            }

            // find the equals
            // indexing into this at 1 is fine since we know the size is at least 2
            std::size_t it = args[1].find(EQUALS);

            // ensure there is an = in the string
            if(it == std::string::npos || it + 1 >= args[1].size() || it == 0){
                return std::nullopt;
            }

            // get the variable name
            data.name = args[1].substr(0, it);
            data.val = args[1].substr(it + 1, args[1].size());
        }else{ // regular binary
            *proc_data = binary_data{};

            assert(std::holds_alternative<binary_data>(*proc_data));

            binary_data& data = std::get<binary_data>(*proc_data);

            // set IO redirection
            data.stdout = std::move(proc_stdout);
            data.stdin = std::move(proc_stdin);
            data.stderr = std::move(proc_stderr);

            // give the variant ownership of the arguments
            data.args = std::move(args);
        }   

        return proc_data;
    }
    
    void process::execute_process(binary_data& data){
        // the array of pointers to the command line arguments
        std::vector<char*> args_ptr;

        // push back all of the pointers to the arguments
        for(auto& arg : data.args){
            args_ptr.push_back(arg.data());
        }

        // add the sentinal to the end of the arguments
        args_ptr.push_back(nullptr);

        // fork into another subprocess to execute the binary
        pid_t pid = fork();
        int status;
        if(pid){ // parent
            pid_t exit_code = waitpid(pid, &status, 0);

            // close all of the older file descriptors
            data.stdout = std::nullopt;
            data.stdin = std::nullopt;
            data.stderr = std::nullopt;

            // check for errors
            if(exit_code != pid){
                jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Waiting on PID ", pid, " failed...");
            }
        }else{ // child
            { // sir scope
                shell_internal_redirection sir(std::move(data.stdout), std::move(data.stdin), std::move(data.stderr), false);

                int exit_code = execvp(args_ptr[0], args_ptr.data());

                // execvp only returns if there was an error
                assert(exit_code == -1);
            } // sir scope

            // log that this command was invalid
            jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Executing command failed...");
        }
    }


    void process::execute_process(export_data& data){
        { // sir scope
            shell_internal_redirection sir(std::move(data.stdout), std::move(data.stdin), std::move(data.stderr));

            // perform the export
            environment::set_var(data.name.c_str(), data.val.c_str());
        } // sir scope
    }

    void process::execute(std::unique_ptr<process_data>& data){
        // execute on the given data
        std::visit([](auto&& var){
                    jsh::process::execute_process(var);
                }, *data);
    }
} // namespace jsh
