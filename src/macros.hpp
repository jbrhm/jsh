#pragma once

#include "pch.hpp"

static constexpr std::size_t ERR_BUF_SIZE = 100;
inline std::array<char, ERR_BUF_SIZE> err_buf;

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
        SILENT = 5,
        COUNT
    };

    inline static constexpr char const* log_level_strings[] = {"DEBUG", "WARN", "ERROR", "FATAL", "STATUS", ""};
    

    inline char global_log_level = 0;
    inline void set_log_level(char level){
        global_log_level = level;
    }

    class logger{
    private:
        // output stream which the logger 
        std::ostream& _os;

    public:
        // delete all copy/move constructors and assignment operators
        logger(logger const& other) = delete;
        logger(logger&& other) = delete;
        logger& operator=(logger const& other) = delete;
        logger& operator=(logger&& other) = delete;

        logger([[maybe_unused]] std::ostream& os = std::cout) : _os{os}{}

        template<printable ...T>
        inline void log(LOG_LEVEL log_level, T... arg){
            if(log_level == LOG_LEVEL::SILENT){
                (_os << ... << arg);
                _os << std::flush;
            }else if(log_level >= global_log_level){
                _os << log_level_strings[log_level] << ": ";
                (_os << ... << arg);
                _os << '\n';
                _os << std::flush;
            }
        }
    };

    inline static logger cout_logger;
};
