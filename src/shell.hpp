#pragma once

#include "pch.hpp"

// JSH
#include "posix_wrappers.hpp"
#include "environment.hpp"
#include "parsing.hpp"
#include "process.hpp"
#include "job.hpp"

namespace jsh {
    class shell{
    private:
        /**
         * is_interactice: indicates whether the shell is running in interactive mode
         */
        bool is_interactive;

        /**
         * term_if: describes the current interface/configuration for the terminal
         */
        std::shared_ptr<termios> term_if;

    public:
        /**
         * constructor
         */
        shell();

        /**
         * execute_command: recieves user input and executes their command, returns false if we are to exit
         */
        [[nodiscard]] auto execute_command() -> bool;


        /**
         * destructor
         */
        ~shell();
    };
} // namespace jsh
