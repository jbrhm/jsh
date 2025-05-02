#pragma once

// STL
#include <concepts>
#include <iostream>
#include <type_traits>
#include <array>

template <typename T>
concept printable = 
    requires(T t) {{std::cout << t};};

namespace jsh {
    enum LOG_LEVEL : char {
        DEBUG = 0,
        WARN = 1,
        ERROR = 2,
        FATAL = 3,
        STATUS = 4,
        COUNT
    };

    inline static constexpr std::array<std::string, LOG_LEVEL::COUNT> log_level_strings = {"DEBUG", "WARN", "ERROR", "FATAL", "STATUS"};

    template<printable ...T>
    inline void log(LOG_LEVEL log_level, T... arg){
        std::cout << log_level_strings[log_level] << ": ";
        (std::cout << ... << arg);
        std::cout << '\n';
    }
};
