#pragma once

#include "pch.hpp"

// JSH
#include "environment.hpp"
#include "job.hpp"
#include "macros.hpp"
#include "parsing.hpp"
#include "posix_wrappers.hpp"
#include "process.hpp"

namespace jsh {
class shell {
  private:
    /**
     *
     */
    static constexpr char const* PROMPT_MESSAGE = "prompt:";
    /**
     * is_interactice: indicates whether the shell is running in interactive mode
     */
    bool is_interactive;

    /**
     * term_if: describes the current interface/configuration for the terminal
     */
    std::shared_ptr<termios> term_if;

    /**
     * shell_ptr: a singleton pointing to the only install of the jsh shell
     */
    static std::optional<std::shared_ptr<shell>> shell_ptr;

  public:
    /**
     * constructor
     */
    shell();

    /**
     * delete other constructors
     */
    shell(shell const& other) = delete;
    shell(shell&& other) = delete;
    auto operator=(shell const& other) -> shell& = delete;
    auto operator=(shell&& other) -> shell& = delete;

    /**
     * destructor
     */
    ~shell();

    /**
     * get: returns the pointer to the singleton jsh instance
     */
    [[nodiscard]] static auto get() -> std::optional<std::shared_ptr<shell>>;

    /**
     * execute_command: recieves user input and executes their command, returns false if we are to exit
     */
    [[nodiscard]] static auto execute_command() -> bool;

    /**
     * get_term_if: returns the terminal interface pointer
     */
    [[nodiscard]] auto get_term_if() -> std::shared_ptr<termios>;

    /**
     * ctrl_c_signal_handler: handles the ctrl+c input to print a new line
     */
    static void ctrl_c_signal_handler(int sig);
};
} // namespace jsh
