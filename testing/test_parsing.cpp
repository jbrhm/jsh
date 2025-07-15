// GTEST
#include <gtest/gtest.h>

// JSH
#include <parsing.hpp>

TEST(TestParsing, TestPeekValid) {
    // create a string to call peek on
    std::string const input = "abcdefghijklmnopqrstuvwxyz";

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
    std::string const input = "abcdefghijklmnopqrstuvwxyz";
    std::string const zero{};

    // make sure that npos returns nullopt
    ASSERT_TRUE(jsh::parsing::peek_char(input, std::string::npos) ==
                std::nullopt);

    // check upper bound
    ASSERT_TRUE(jsh::parsing::peek_char(zero, 0) == std::nullopt);

    // check upper bound
    ASSERT_TRUE(jsh::parsing::peek_char(input, 26) == std::nullopt);
}

TEST(TestParsing, TestSubstitutionBasic) {
    // set the environment variable
    jsh::environment::set_var("var", "val");

    // end substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("abc${var}") == "abcval");

    // beginning substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("${var}abc") == "valabc");

    // middle substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("ab${var}c") == "abvalc");

    // just substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("${var}") == "val");

    // no substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("abc") == "abc");
}

TEST(TestParsing, TestEnvironmentVariableSubstitutions) {
    // set the environment variable
    jsh::environment::set_var("var", "val");
    jsh::environment::set_var("var2", "${var}");

    // end substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("abc${var2}") == "abcval");

    // beginning substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("${var2}abc") == "valabc");

    // middle substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("ab${var2}c") == "abvalc");

    // just substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("${var2}") == "val");

    // no substitution
    ASSERT_TRUE(jsh::parsing::variable_substitution("abc") == "abc");
}

TEST(TestParsing, TestMaxSubstitutions) {
    // set the environment variable
    jsh::environment::set_var("var", "${var2}");
    jsh::environment::set_var("var2", "${var}");

    // infinite looping
    ASSERT_TRUE(jsh::parsing::variable_substitution("abc${var2}") ==
                "abc${var2}");
}

TEST(TestParsing, TestMalformed) {
    // set the environment variable
    jsh::environment::set_var("var", "val");

    // test extra characters one
    ASSERT_EQ(jsh::parsing::variable_substitution("${NOT SET}a$bc${var}"),
              "a$bcval");

    // test extra characters two
    ASSERT_EQ(jsh::parsing::variable_substitution("${NOT SET}a$${var}"),
              "a$val");

    // test extra characters three
    ASSERT_EQ(jsh::parsing::variable_substitution("${NOT SET}a${${var}"), "a");

    // test extra characters four
    ASSERT_EQ(jsh::parsing::variable_substitution("${NOT SET}a$a{${var}"),
              "a$a{val");

    // test extra characters five
    ASSERT_EQ(jsh::parsing::variable_substitution("${val"), "${val");

    // test extra characters six
    ASSERT_EQ(jsh::parsing::variable_substitution("${var}${var"), "val${var");
}
