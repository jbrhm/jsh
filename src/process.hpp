#pragma once

#include "pch.hpp"

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
        std::vector<std::string> args;
    };

    // typedef for a one command the user runs
    using process_data = std::variant<binary_data, export_data>;

    class process {
    private:
        /**
         * CONSTANTS
         */
        static constexpr char const* EXPORT_BUILTIN = "export";

    public:
        /**
         * parse_process: parse an input into a process_data structure, or if a shell internal was called return the appropriate type
         *
         * input: the input command provided by the user to start the process
         */
        [[nodiscard]] static auto parse_process(std::string const& input) -> std::unique_ptr<process_data>;
    };
} // namespace jsh
