// GTEST
#include <gtest/gtest.h>

// JSH
#include <process.hpp>

TEST(TestProcess, test) {
    // input from user
    std::string input = "export var1=val1";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_TRUE(proc_data.has_value());

    // should not be nullptr
    ASSERT_NE(proc_data.value().get(), nullptr);

    // should hold a expor_data struct
    ASSERT_TRUE(std::holds_alternative<jsh::export_data>(*(proc_data.value())));

    // varify the contents of the data
    jsh::export_data& data = std::get<jsh::export_data>(*(proc_data).value());
    ASSERT_STREQ(data.name.c_str(), "var1");
    ASSERT_STREQ(data.val.c_str(), "val1");
}
