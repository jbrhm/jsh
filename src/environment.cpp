#include "environment.hpp"

namespace jsh {
        auto environment::get() -> char**{
            return environ;
        }

        void environment::set_var(char const* var, char const* val){
            // ensure that there is not an equals in the variable name
            assert(std::strstr(var, "=") == nullptr);

            // set the environment variable to the appropriate value, overriding if necessary
            setenv(var, val, true);
        }

        auto environment::get_var(char const* var) -> char const*{
            // ensure that there is not an equals in the variable name
            assert(std::strstr(var, "=") == nullptr);

            // get the environment variable from environ
            return getenv(var);
        }

        void environment::print([[maybe_unused]] logger& log){
            // iterate until nullptr
            for(std::size_t i = 0; get()[i]; ++i){
                // print the env variable of the form var=value
                log.log(LOG_LEVEL::SILENT, get()[i], '\n');
            }
        }

} // namespace jsh
