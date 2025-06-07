#include "process.hpp"

namespace jsh {

    auto process::parse_process(std::string const& input) -> std::optional<std::unique_ptr<process_data>>{
        // TODO: find a way to prevent this from defaulting to export
        auto proc_data = std::make_unique<process_data>();

        // stack allocated variables
        std::string arg;
        std::stringstream ss(input);
        std::vector<std::string> args;

        // parse the command into different components
        while(ss >> arg){
            args.push_back(arg);
        }

        // search for shell built-ins
        // otherwise, treat as binary
        if(!args.empty() && args[0] == EXPORT_BUILTIN){ // export
            *proc_data = export_data{};

            assert(std::holds_alternative<export_data>(*proc_data));

            export_data& data = std::get<export_data>(*proc_data);

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

            if(exit_code != pid){
                jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Waiting on PID ", pid, " failed...");
            }
        }else{ // child
            // check for a different stdout
            if(data.stdout != STDOUT_FILENO){
                // this will essentially make STDOUT_FILENO point to the other file descriptor
                // since according to the man page it will close STDOUT_FILENO since it already exists
                dup2(data.stdout, STDOUT_FILENO);
                close(data.stdout);
            }

            // check for a different stdin
            if(data.stdin != STDIN_FILENO){
                dup2(data.stdin, STDIN_FILENO);
                close(data.stdin);
            }

            // check for a different stderr
            if(data.stderr != STDERR_FILENO){
                dup2(data.stderr, STDERR_FILENO);
                close(data.stderr);
            }
                int exit_code = execvp(args_ptr[0], args_ptr.data());

            // execvp only returns if there was an error
            assert(exit_code == -1);

            // log that this command was invalid
            jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Executing command failed...");
        }
    }


    void process::execute_process(export_data& data){
        // perform the export
        environment::set_var(data.name.c_str(), data.val.c_str());
    }

    void process::execute(std::optional<std::unique_ptr<process_data>>& data){
        // execute on the given data
        std::visit([](auto&& var){
                    jsh::process::execute_process(var);
                }, *data.value());
    }
} // namespace jsh
