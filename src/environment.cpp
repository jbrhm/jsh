#include "environment.hpp"

namespace jsh {
auto environment::get() -> char** { return environ; }

void environment::set_var(char const* var, char const* val) {
    // ensure that there is not an equals in the variable name
    assert(std::strstr(var, "=") == nullptr);

    // set the environment variable to the appropriate value, overriding if
    // necessary
    setenv(var, val, 1); // NOLINT
}

auto environment::get_var(char const* var) -> char const* {
    // ensure that there is not an equals in the variable name
    assert(std::strstr(var, "=") == nullptr);

    // get the environment variable from environ
    char const* val = getenv(var); // NOLINT

    // check for failure
    if (val != nullptr) [[likely]] {
        return val;
    }

    return EMPTY_STRING;
}

void environment::print([[maybe_unused]] logger& log) {
    // iterate until nullptr
    for (std::size_t i = 0; get()[i]; ++i) [[likely]] { // NOLINT
        // print the env variable of the form var=value
        log.log(LOG_LEVEL::SILENT, get()[i], '\n'); // NOLINT
    }
}

} // namespace jsh
