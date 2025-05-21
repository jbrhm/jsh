// GTEST
#include <gtest/gtest.h>

// JSH
#include <environment.hpp>
#include <macros.hpp>

// STL
#include <vector>
#include <sstream>
#include <string>

TEST(TestEnvironment, EnvironmentConstructor) {
    // create the envp
    std::vector<std::string> strs = {"VAR1=VAL1", "VAR2=VAL2", "VAR3=VAL3", "VAR4=VAL5"};
    std::vector<char*> envp = {strs[0].data(), strs[1].data(), strs[2].data(), strs[3].data()};

    // create the jsh env
    jsh::environment env(envp.data());

    // create the test logger
    std::stringstream output;
    jsh::logger log(output);

    // print the output
    env.print(log);
    
    // check against the correct output
    std::cout << output.str();
}
