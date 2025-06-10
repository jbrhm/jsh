#pragma once

#include "pch.hpp"

// JSH
#include "environment.hpp"

namespace jsh {
    /**
     * structure to define process agnostic information
     *
     * stdout: the file descriptor which standard out will be directed towards
     *
     * stdin: the file descriptor which standard in will be directed towards
     *
     * stderr: the file descriptor which standard error will be directed towards
     */
    struct default_data {
        int stdout = STDOUT_FILENO;
        int stdin = STDIN_FILENO;
        int stderr = STDERR_FILENO;
    };

    /**
     * structure to wrap all data necessary to run a process
     *
     * args: the arguments provided to the shell
     */
    struct binary_data : default_data {
        std::vector<std::string> args;
    };

    /**
     * structure to wrap all of the necessary data to perform an export operation
     *
     * name: the name of the variable which will be exported
     *
     * val: the value that will be associated with this environment variable
     */
    struct export_data : default_data {
        std::string name;
        std::string val;
    };

    // typedef for a one command the user runs
    using process_data = std::variant<binary_data, export_data>;

    class process {
    private:
        /**
         * CONSTANTS
         */
        static constexpr char const* EXPORT_BUILTIN = "export";
        static constexpr char EQUALS = '=';
        static constexpr char INPUT_REDIRECTION = '<';
        static constexpr char OUTPUT_REDIRECTION = '>';

        /**
         * populate the binary with its data
         */
        static void populate_process_data(binary_data& data);

        /**
         * shell_internal_redirection: RAII wrapper around setting stdout, stdin, and stderr for a given shell internal
         */
        class shell_internal_redirection {
        private:
            /**
             * new_stdout: stdout fd for shell internal
             */
            int new_stdout;

            /**
             * new_stdin: stdin fd for shell internal
             */
            int new_stdin;

            /**
             * new_stderr: stderr fd for shell internal
             */
            int new_stderr;

            /**
             * og_stdout: stdout fd for shell internal
             */
            int og_stdout;

            /**
             * og_stdin: stdin fd for shell internal
             */
            int og_stdin;

            /**
             * og_stderr: stderr fd for shell internal
             */
            int og_stderr;

            /**
             * restore: indicates whether the file descriptors will be restored in the destructor
             */
            bool _restore;
        public:
            shell_internal_redirection(int stdout, int stdin, int stderr, [[maybe_unused]] bool restore = true);
            ~shell_internal_redirection();
        };

    public:
        /**
         * parse_process: parse an input into a process_data structure, or if a shell internal was called return the appropriate type
         *
         * input: the input command provided by the user to start the process
         */
        [[nodiscard]] static auto parse_process(std::string const& input) -> std::optional<std::unique_ptr<process_data>>;

        /**
         * execute_binary: performs the execution for binary
         *
         * data: the parsed input command from the user which is used to execute the binary
         */
        static void execute_process(binary_data& data);

        /**
         * execute_export: performs the execution for an export shell intrinsic
         *
         * data: the information necessary to perform the export
         */
        static void execute_process(export_data& data);

        /**
         * execute: determines which type of process should be executed
         *
         * data: variant of data to be executed upon
         */
        static void execute(std::unique_ptr<process_data>& data);
    };
} // namespace jsh
