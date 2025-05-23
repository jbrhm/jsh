// GTEST
#include <gtest/gtest.h>

// JSH
#include <parsing.hpp>

TEST(TestParsing, TestPeekValid) {
    // create a string to call peek on
    std::string input = "abcdefghijklmnopqrstuvwxyz";

    // make sure the string was initialized properly
    ASSERT_TRUE(input.size() == 26);

    // peek a
    ASSERT_TRUE(jsh::parsing::peek_char(input, 0) == 'a');
    
    // peek z
    ASSERT_TRUE(jsh::parsing::peek_char(input, 25) == 'z');

    // peek j
    ASSERT_TRUE(jsh::parsing::peek_char(input, 9) == 'j');

    // peek s
    ASSERT_TRUE(jsh::parsing::peek_char(input, 18) == 's');

    // peek h
    ASSERT_TRUE(jsh::parsing::peek_char(input, 7) == 'h');
}

TEST(TestParsing, TestPeekInvalid) {
    // create a string to call peek on
    std::string input = "abcdefghijklmnopqrstuvwxyz";
    std::string zero = "";

    // make sure that npos returns nullopt
    ASSERT_TRUE(jsh::parsing::peek_char(input, std::string::npos) == std::nullopt);
    
    // check upper bound
    ASSERT_TRUE(jsh::parsing::peek_char(zero, 0) == std::nullopt);

    // check upper bound
    ASSERT_TRUE(jsh::parsing::peek_char(input, 26) == std::nullopt);
}
