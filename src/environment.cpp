#include "environment.hpp"

namespace jsh {
        void environment::set_var(char const* var, char const* value){
            // ensure that there is not an equals in the variable name
            assert(std::strstr(var, "=") == nullptr);

            // set the environment variable to the appropriate value, overriding if necessary
            setenv(var, value, true);
        }

        /**
         * get_var: gets the value of the environment variable var
         *
         * var: the name of the environment variable
         */
        inline static void get_var(char const* var){
            // ensure that there is not an equals in the variable name
            assert(std::strstr(var, "=") == nullptr);

<<<<<<< Updated upstream
    void environment::print(logger& log){
        // print all of the variables and their values
        for(auto const& [key, value] : variables){
            log.log(LOG_LEVEL::SILENT, key, EQUALS, value, '\n');
=======
            // get the environment variable from environ
            getenv(var);
>>>>>>> Stashed changes
        }

        /**
         * get: gets the array of environment strings representing the current process' environment
         */
        [[nodiscard]] inline static auto get() -> char**{
            return environ;
        }

        /**
         * print: 
         */
        inline static void print([[maybe_unused]] logger& log = cout_logger){

        }

} // namespace jsh
