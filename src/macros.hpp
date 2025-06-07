#pragma once

#include "pch.hpp"

#define CHECK_DUP(fd) \
    if(fd == -1) { \
        std::string err = strerror(errno); \
        cout_logger.log(LOG_LEVEL::ERROR, err); \
    } \
    return;

#define CHECK_CLOSE(status) \
    if(status == -1) { \
        std::string err = strerror(errno); \
        cout_logger.log(LOG_LEVEL::ERROR, err); \
    } \
    return;

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
            }else if(log_level >= global_log_level){
                _os << log_level_strings[log_level] << ": ";
                (_os << ... << arg);
                _os << '\n';
            }
        }
    };

    inline static logger cout_logger;
};
