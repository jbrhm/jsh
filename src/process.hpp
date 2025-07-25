#pragma once

#include "pch.hpp"

// JSH
#include "environment.hpp"
#include "macros.hpp"
#include "posix_wrappers.hpp"

namespace jsh {
/**
 * structure to define process agnostic information
 *
 * stdout: the file descriptor which standard out will be directed towards
 *
 * stdin: the file descriptor which standard in will be directed towards
 *
 * stderr: the file descriptor which standard error will be directed towards
 *
 * pgid: the current process group id the process should be put in, note: this information is redundant to the pgid in the job structure, however instead of passing on the stack, we add a member to this structure
 *
 * is_foreground: indicates whether the process will run in the foreground of the shell
 */
static constexpr std::size_t DEFAULT_DATA_ALIGNMENT = 64;
struct __attribute__((packed)) __attribute__((aligned(DEFAULT_DATA_ALIGNMENT))) default_data {
    std::optional<file_descriptor_wrapper> stdout = std::nullopt;
    std::optional<file_descriptor_wrapper> stdin = std::nullopt;
    std::optional<file_descriptor_wrapper> stderr = std::nullopt;

    std::shared_ptr<pid_t> pgid;

    bool is_foreground;
};

/**
 * structure to wrap all data necessary to run a process
 *
 * args: the arguments provided to the shell
 */
struct __attribute__((packed)) binary_data : default_data { // NOLINT this complains about being 64 byte aligned
    std::vector<std::string> args;
};

/**
 * structure to wrap all of the necessary data to perform an export operation
 *
 * name: the name of the variable which will be exported
 *
 * val: the value that will be associated with this environment variable
 */
struct __attribute__((packed)) export_data : default_data { // NOLINT this complains about being 64 byte aligned
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
    static constexpr char SINGLE_QUOTE = '\'';
    static constexpr char DOUBLE_QUOTE = '\"';

    /**
     * populate the binary with its data
     */
    static void populate_process_data(binary_data& data);

    /**
     * parse_state: the current state of the parsing algorithm
     */
    enum PARSE_STATE : char {
        REGULAR = 0,
        QUOTE = 1,
        DBL_QUOTE = 2,
        INPUT_FILENAME = 3,
        OUTPUT_FILENAME = 4,
        COUNT = 5
    };

    /**
     * shell_internal_redirection: RAII wrapper around setting stdout, stdin, and stderr for a given shell internal
     */
    class shell_internal_redirection {
      private:
        /**
         * new_stdout: stdout fd for shell internal
         */
        std::optional<file_descriptor_wrapper> new_stdout;

        /**
         * new_stdin: stdin fd for shell internal
         */
        std::optional<file_descriptor_wrapper> new_stdin;

        /**
         * new_stderr: stderr fd for shell internal
         */
        std::optional<file_descriptor_wrapper> new_stderr;

        /**
         * og_stdout: stdout fd for shell internal
         */
        std::optional<file_descriptor_wrapper> og_stdout;

        /**
         * og_stdin: stdin fd for shell internal
         */
        std::optional<file_descriptor_wrapper> og_stdin;

        /**
         * og_stderr: stderr fd for shell internal
         */
        std::optional<file_descriptor_wrapper> og_stderr;

        /**
         * restore: indicates whether the file descriptors will be restored in the destructor
         */
        bool _restore;

      public:
        /**
         * constructor to wrap all fd redirection
         */
        shell_internal_redirection(std::optional<file_descriptor_wrapper> stdout, std::optional<file_descriptor_wrapper> stdin, std::optional<file_descriptor_wrapper> stderr, [[maybe_unused]] bool restore = true);

        /**
         * delete other constructors
         */
        shell_internal_redirection(shell_internal_redirection const& other) = delete;
        shell_internal_redirection(shell_internal_redirection&& other) = delete;
        auto operator=(shell_internal_redirection const& other) -> shell_internal_redirection& = delete;
        auto operator=(shell_internal_redirection&& other) -> shell_internal_redirection& = delete;

        /**
         * destructor to close all fds
         */
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
