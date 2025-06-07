// GTEST
#include <gtest/gtest.h>

// JSH
#include <process.hpp>

TEST(TestProcess, TestExportBasic1) {
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

TEST(TestProcess, TestExportBasic2) {
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

TEST(TestProcess, TestExportBasic3) {
    // input from user
    std::string input = "export\tv=v";

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

TEST(TestProcess, TestMalformed1) {
    // input from user
    std::string input = "export v=";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestMalformed2) {
    // input from user
    std::string input = "export =v";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestMalformed3) {
    // input from user
    std::string input = "export =";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestMalformed4) {
    // input from user
    std::string input = "export";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestExtra) {
    // input from user
    std::string input = "export var=val awdasdawdsawd";

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

TEST(TestProcess, TestExecuteBinary) {
    // make a fifo to store the output from standard out
    int fds[2];
    if(pipe(fds)){
        char const* msg = strerror(errno);
        jsh::cout_logger.log(jsh::LOG_LEVEL::FATAL, msg);
        assert(false);
    }

    // create the command for the binary data
    std::optional<std::unique_ptr<jsh::process_data>> binary_var = std::make_optional<std::unique_ptr<jsh::process_data>>(std::make_unique<jsh::process_data>(jsh::binary_data{}));

    assert(std::holds_alternative<jsh::binary_data>(*binary_var.value()));

    jsh::binary_data& binary = std::get<jsh::binary_data>(*binary_var.value());

    binary.args = {"echo", "hi"};

    // direct the stdout to be the fifo
    binary.stdout = fds[1];

    // run the binary
    jsh::process::execute(binary_var);

    // read the contents of the pipe
    char data[2];
    read(fds[0], data, sizeof(data));

    // the pipe and buffer should now contain hi
    ASSERT_EQ(data[0], 'h');
    ASSERT_EQ(data[1], 'i');

    // close the read fd for the pipe
    close(fds[0]);
}

TEST(TestProcess, TestExecuteExport) {
    // create the export data structure
    std::optional<std::unique_ptr<jsh::process_data>> export_var = std::make_optional<std::unique_ptr<jsh::process_data>>(std::make_unique<jsh::process_data>(jsh::export_data{}));

    assert(std::holds_alternative<jsh::export_data>(*export_var.value()));

    jsh::export_data& exp = std::get<jsh::export_data>(*export_var.value());

    exp.name = "name";
    exp.val = "val";

    // run the export internal
    jsh::process::execute(export_var);

    // assert that the environment variable was properly exported
    ASSERT_STREQ(jsh::environment::get_var("name"), "val");
}

TEST(TestProcess, TestRestore) {
    // create the export data structure
    std::optional<std::unique_ptr<jsh::process_data>> export_var = std::make_optional<std::unique_ptr<jsh::process_data>>(std::make_unique<jsh::process_data>(jsh::export_data{}));

    assert(std::holds_alternative<jsh::export_data>(*export_var.value()));

    jsh::export_data& exp = std::get<jsh::export_data>(*export_var.value());

    exp.name = "name";
    exp.val = "val";

    // run the export internal
    jsh::process::execute(export_var);

    // assert that the environment variable was properly exported
    ASSERT_STREQ(jsh::environment::get_var("name"), "val");
}
