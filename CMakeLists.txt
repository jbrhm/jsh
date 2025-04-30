cmake_minimum_required(VERSION 3.30)

project(jsh 
        VERSION 0.0.0.1 # major.minor.patch.tweak
        DESCRIPTION "John's Shell"
        LANGUAGES C CXX
    )

# glob all sources
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.c*)

add_executable(${PROJECT_NAME} ${sources})
