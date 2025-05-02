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

    inline char global_log_level = 0;
    inline void set_log_level(char level){
        global_log_level = level;
    }

    template<printable ...T>
    inline void log(LOG_LEVEL log_level, T... arg){
        if(log_level >= global_log_level){
            std::cout << log_level_strings[log_level] << ": ";
            (std::cout << ... << arg);
            std::cout << '\n';
        }
    }
};
