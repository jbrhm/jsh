#include "pch.hpp"

// JSH
#include "shell.hpp"

int main(){
    jsh::set_log_level(jsh::LOG_LEVEL::WARN);

    jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "Welcome to John's Shell\n");

    // this can throw an exception so it must be wrapped in a try and catch
    std::optional<std::shared_ptr<jsh::shell>> jsh_op = jsh::shell::get();

    // error handle
    if(!jsh_op.has_value()){
        jsh::cout_logger.log(jsh::LOG_LEVEL::ERROR, "Failed to create shell singleton...\n");
        return 1;
    }

    // grab the singleton pointer
    assert(jsh_op.has_value());
    std::shared_ptr<jsh::shell> jsh = jsh_op.value();

    // loop through user input
    while(true){
        // execute the user input
        bool status = jsh->execute_command();

        if(!status){
            return 0;
        }
    }

    return 1;
}
