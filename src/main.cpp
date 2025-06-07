#include "pch.hpp"

// JSH
#include "environment.hpp"
#include "parsing.hpp"
#include "process.hpp"

int main(int argc, char* argv[], char* envp[]){
    // make a fifo to store the output from standard out
    int fds[2];
    char const* fifo_path = ".fifo";
    if(pipe(fds)){
        char const* msg = strerror(errno);
        jsh::cout_logger.log(jsh::LOG_LEVEL::FATAL, msg);
        assert(false);
    }

    // create the command for the binary data
    std::optional<std::unique_ptr<jsh::process_data>> binary_var = std::make_optional<std::unique_ptr<jsh::process_data>>(std::make_unique<jsh::process_data>(jsh::binary_data{}));

    assert(std::holds_alternative<jsh::binary_data>(*binary_var.value()));

    jsh::binary_data& binary = std::get<jsh::binary_data>(*binary_var.value());

    binary.args = {"echo", "hi"};

    // direct the stdout to be the fifo
    binary.stdout = fds[1];

    // run the binary
    jsh::process::execute(binary_var);

    // read the contents of the pipe
    char data[2];
    read(fds[0], data, sizeof(data));

    std::cout << data[0] << " " << data[1];

    // close the read fd for the pipe
    close(fds[0]);
    jsh::set_log_level(1);

    jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "Welcome to John's Shell\n");

    std::string input;
    std::string arg;
    while(true){
        // get the command from the user
        jsh::cout_logger.log(jsh::LOG_LEVEL::SILENT, "prompt:");
        getline(std::cin, input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Raw user input: ", input);
        input = jsh::parsing::variable_substitution(input);
        jsh::cout_logger.log(jsh::LOG_LEVEL::DEBUG, "Substituted user input: ", input);

        // parse the users input into a data describing a specific process
        std::optional<std::unique_ptr<jsh::process_data>> data = jsh::process::parse_process(input);

        // check the to see if the input was valid
        if(!data.has_value()){ // invalid
            continue;
        }

        // perform the process' execution
    }
}
