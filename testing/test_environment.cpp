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
    std::vector<char*> vars = {var1.data(), var2.data(), var3.data(), var4.data()};
    std::vector<char*> vals = {val1.data(), val2.data(), val3.data(), val4.data()};

    // update the current environment
    for(std::size_t i = 0; i < vars.size(); ++i){
        jsh::environment::set_var(vars[i], vals[i]);
    }

    // alter them to ensure they are copied properly
    for(std::size_t i = 0; i < vars.size(); ++i){
        vars[i][0] = vals[i][0] = 'a';
    }

    // create the test logger
    std::stringstream output;
    jsh::logger log(output);

    // make sure new vars persist

    for(std::size_t i = 0; i < vars.size(); ++i){
        ASSERT_FALSE(std::strcmp(jsh::environment::get_var(vars[i]), vals[i]));
    }
}
