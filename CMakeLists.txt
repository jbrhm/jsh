cmake_minimum_required(VERSION 3.30)

project(jsh 
        VERSION 0.0.0.1 # major.minor.patch.tweak
        DESCRIPTION "John's Shell"
        LANGUAGES C CXX
    )

# export compile commands for nvim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# use mold (modern linker) for faster link times
find_program(FOUND_MOLD mold)
if(FOUND_MOLD)
    message(STATUS "Found mold ${FOUND_MOLD}")
    add_link_options(-fuse-ld=mold)
endif()

# glob all sources
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.c*)

# create jsh executable
add_executable(${PROJECT_NAME} ${sources})
