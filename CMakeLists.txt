cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 23)

project(jsh 
        VERSION 0.0.0.1 # major.minor.patch.tweak
        DESCRIPTION "John's Shell"
        LANGUAGES C CXX
    )


# export compile commands for nvim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# use mold (modern linker) for faster link times
# https://github.com/rui314/mold?tab=readme-ov-file
find_program(FOUND_MOLD mold)
if(FOUND_MOLD)
    message(STATUS "Found mold ${FOUND_MOLD}")
    add_link_options(-fuse-ld=mold)
endif()

# use ccache to cache parts of project builds
# https://ccache.dev/
find_program(FOUND_CCACHE ccache)
if(FOUND_CCACHE)
    message(STATUS "Found ccache ${FOUND_CCACHE}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
    set(CMAKE_C_COMPILER_LAUNCHER ccache)
endif()

# add testing utils subdirectory
add_subdirectory("testing/utils")

# create jsh utils library (so both the executable and the tests can link against it)
set(utils_sources
    src/environment.cpp
    src/parsing.cpp
    src/process.cpp
    src/job.cpp
    src/posix_wrappers.cpp
)
add_library(${PROJECT_NAME}_utils SHARED ${utils_sources})
target_include_directories(${PROJECT_NAME}_utils PUBLIC src)

# create jsh executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_utils)

# use google test for regression suite
find_package(GTest REQUIRED)

file(GLOB tests CONFIGURE_DEPENDS testing/*.cpp)

enable_testing()

add_executable(test_suite ${tests})
target_link_libraries(test_suite GTest::gtest_main ${PROJECT_NAME}_utils)

include(GoogleTest)
gtest_discover_tests(test_suite)
