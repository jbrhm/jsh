// GTEST
#include <gtest/gtest.h>

// JSH
#include <process.hpp>

TEST(TestProcess, TestBasic1) {
    // input from user
    std::string input = "export var=val";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_TRUE(proc_data.has_value());

    // should hold a expor_data struct
    ASSERT_TRUE(std::holds_alternative<jsh::export_data>(*(proc_data.value())));

    // varify the contents of the data
    jsh::export_data& data = std::get<jsh::export_data>(*(proc_data).value());
    ASSERT_STREQ(data.name.c_str(), "var");
    ASSERT_STREQ(data.val.c_str(), "val");
}

TEST(TestProcess, TestBasic2) {
    // input from user
    std::string input = "export v=v";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_TRUE(proc_data.has_value());

    // should hold a expor_data struct
    ASSERT_TRUE(std::holds_alternative<jsh::export_data>(*(proc_data.value())));

    // varify the contents of the data
    jsh::export_data& data = std::get<jsh::export_data>(*(proc_data).value());
    ASSERT_STREQ(data.name.c_str(), "v");
    ASSERT_STREQ(data.val.c_str(), "v");
}
