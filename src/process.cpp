#include "process.hpp"

namespace jsh {

    auto process::parse_process(std::string const& input) -> std::unique_ptr<process_data>{
        // TODO: find a way to prevent this from defaulting to export
        auto proc_data = std::make_unique<process_data>();
        
        // THIS IS NOT THREAD SAFE
        // so it only is initialized once
        static std::string arg;
        static std::stringstream ss(input);

        // do not statically allocate becasue this one will be moved later
        std::vector<std::string> args;

        // parse the command into different components
        while(ss >> arg){
            args.push_back(arg);
        }

        // search for shell built-ins
        // otherwise, treat as binary
        if(!args.empty() && args[0] == EXPORT_BUILTIN){ // export
            *proc_data = export_data{};
        }else{ // regular binary

        }
    }
} // namespace jsh
