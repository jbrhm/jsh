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
            assert(args.size() == 2);

            // find the equals
            // indexing into this at 1 is fine since we know the size is at least 2
            std::size_t it = args[1].find(EQUALS);

            // ensure there is an = in the string
            if(it == std::string::npos || it + 1 >= args[1].size()){
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
} // namespace jsh
