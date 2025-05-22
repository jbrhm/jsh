// GTEST
#include <gtest/gtest.h>

// JSH
#include <environment.hpp>
#include <macros.hpp>

// STL
#include <vector>
#include <sstream>
#include <string>

// UNIX
#include <unistd.h>

TEST(TestEnvironment, EnvironmentSetGet) {
    // create the envp
    std::string var1 = "VAR1", var2 = "VAR2", var3 = "VAR3", var4 = "VAR4";
    std::string val1 = "VAL1", val2 = "VAL2", val3 = "VAL3", val4 = "VAL4";
    std::string cor1 = "VAL1", cor2 = "VAL2", cor3 = "VAL3", cor4 = "VAL4";
    std::vector<char*> vars = {var1.data(), var2.data(), var3.data(), var4.data()};
    std::vector<char*> vals = {val1.data(), val2.data(), val3.data(), val4.data()};
    std::vector<char*> cors = {cor1.data(), cor2.data(), cor3.data(), cor4.data()};

    // update the current environment
    for(std::size_t i = 0; i < vars.size(); ++i){
        jsh::environment::set_var(vars[i], vals[i]);
    }

    // alter them to ensure they are copied properly
    for(std::size_t i = 0; i < vars.size(); ++i){
        vals[i][0] = 'a';
    }

    // make sure new vars persist
    for(std::size_t i = 0; i < vars.size(); ++i){
        ASSERT_FALSE(std::strcmp(jsh::environment::get_var(vars[i]), cors[i]));
    }
}

TEST(TestEnvironment, EnvironmentGetUnset) {
    // make sure if the environment variable is unset, then it returns an empty string
    ASSERT_FALSE(std::strcmp(jsh::environment::get_var("NOT SET"), ""));
}
