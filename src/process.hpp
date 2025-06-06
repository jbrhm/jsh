#pragma once

#include "pch.hpp"

// JSH
#include "environment.hpp"

namespace jsh {
    /**
     * structure to wrap all data necessary to run a process
     *
     * args: the arguments provided to the shell
     */
    struct binary_data{
        std::vector<std::string> args;
    };

    /**
     * structure to wrap all of the necessary data to perform an export operation
     *
     * args: the arguments provided to the shell
     */
    struct export_data{
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

        /**
         * populate the binary with its data
         */
        static void populate_process_data(binary_data& data);

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
    };
} // namespace jsh
