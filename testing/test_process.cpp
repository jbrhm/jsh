// GTEST
#include <gtest/gtest.h>

// JSH
#include "posix_wrappers.hpp"
#include "process.hpp"

TEST(TestProcess, TestExportBasic1) {
    // input from user
    std::string const input = "export var=val";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_TRUE(proc_data.has_value());

    // should hold a expor_data struct
    ASSERT_TRUE(std::holds_alternative<jsh::export_data>(*(proc_data.value()))); // NOLINT assert catches this .value()

    // varify the contents of the data
    auto& data = std::get<jsh::export_data>(*(proc_data).value()); // NOLINT assert catches this .value()
    ASSERT_STREQ(data.name.c_str(), "var");
    ASSERT_STREQ(data.val.c_str(), "val");
}

TEST(TestProcess, TestExportBasic2) {
    // input from user
    std::string const input = "export v=v";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_TRUE(proc_data.has_value());

    // should hold a expor_data struct
    ASSERT_TRUE(std::holds_alternative<jsh::export_data>(*(proc_data.value()))); // NOLINT assert catches this .value()

    // varify the contents of the data
    auto& data = std::get<jsh::export_data>(*(proc_data).value()); // NOLINT assert catches this .value()
    ASSERT_STREQ(data.name.c_str(), "v");
    ASSERT_STREQ(data.val.c_str(), "v");
}

TEST(TestProcess, TestExportBasic3) {
    // input from user
    std::string const input = "export\tv=v";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_TRUE(proc_data.has_value());

    // should hold a expor_data struct
    ASSERT_TRUE(std::holds_alternative<jsh::export_data>(*(proc_data.value()))); // NOLINT assert catches this .value()

    // varify the contents of the data
    auto& data = std::get<jsh::export_data>(*(proc_data).value()); // NOLINT assert catches this .value()
    ASSERT_STREQ(data.name.c_str(), "v");
    ASSERT_STREQ(data.val.c_str(), "v");
}

TEST(TestProcess, TestMalformed1) {
    // input from user
    std::string const input = "export v=";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestMalformed2) {
    // input from user
    std::string const input = "export =v";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestMalformed3) {
    // input from user
    std::string const input = "export =";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestMalformed4) {
    // input from user
    std::string const input = "export";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestExtra) {
    // input from user
    std::string const input = "export var=val awdasdawdsawd";

    auto proc_data = jsh::process::parse_process(input);

    // we should have a value
    ASSERT_TRUE(proc_data.has_value());

    // should hold a expor_data struct
    ASSERT_TRUE(std::holds_alternative<jsh::export_data>(*(proc_data.value()))); // NOLINT assert catches this .value()

    // varify the contents of the data
    auto& data = std::get<jsh::export_data>(*(proc_data).value()); // NOLINT assert catches this .value()
    ASSERT_STREQ(data.name.c_str(), "var");
    ASSERT_STREQ(data.val.c_str(), "val");
}

TEST(TestProcess, TestExecuteBinary) {
    // make a fifo to store the output from standard out
    std::optional<std::vector<jsh::file_descriptor_wrapper>> pipe_fds_op = jsh::syscall_wrapper::pipe_wrapper();
    ASSERT_TRUE(pipe_fds_op.has_value());
    ASSERT_TRUE(pipe_fds_op.value().size() == 2); // NOLINT assert catches this .value()

    std::vector<jsh::file_descriptor_wrapper> pipe_fds = std::move(pipe_fds_op.value()); // NOLINT assert catches this .value()

    ASSERT_TRUE(pipe_fds.size() == 2);

    // create the command for the binary data
    std::unique_ptr<jsh::process_data> binary_var = std::make_unique<jsh::process_data>(jsh::binary_data{});

    assert(std::holds_alternative<jsh::binary_data>(*binary_var));

    auto& binary = std::get<jsh::binary_data>(*binary_var);
    binary.pgid = std::make_shared<pid_t>(-1);
    binary.is_foreground = false;

    binary.args = {"echo", "hi"};

    // direct the stdout to be the fifo
    binary.stdout = std::move(pipe_fds[1]);

    // run the binary
    jsh::process::execute(binary_var);

    // read the contents of the pipe
    std::array<char, 2> data{};
    int total_read = 0;
    while (total_read != sizeof(data)) {
        std::optional<ssize_t> rea = jsh::syscall_wrapper::read_wrapper(pipe_fds[0], data.data(), sizeof(data));

        // read should've succeeded
        ASSERT_TRUE(rea.has_value());

        // add the number of bytes read
        total_read += static_cast<int>(rea.value()); // NOLINT assert catches this .value()
    }

    // the pipe and buffer should now contain hi
    ASSERT_EQ(data[0], 'h');
    ASSERT_EQ(data[1], 'i');
}

TEST(TestProcess, TestExecuteExport) {
    // create the export data structure
    std::unique_ptr<jsh::process_data> export_var = std::make_unique<jsh::process_data>(jsh::export_data{});

    assert(std::holds_alternative<jsh::export_data>(*export_var));

    auto& exp = std::get<jsh::export_data>(*export_var);

    exp.name = "name";
    exp.val = "val";

    // run the export internal
    jsh::process::execute(export_var);

    // assert that the environment variable was properly exported
    ASSERT_STREQ(jsh::environment::get_var("name"), "val");
}

TEST(TestProcess, TestInputRedirectionParsingBasic1) {
    std::string const input = "grep hi < testing/tmp/file";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()

    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()
    ASSERT_NE(data.stdin, jsh::syscall_wrapper::stdin_file_descriptor);
}

TEST(TestProcess, TestInputRedirectionParsingBasic2) {
    std::string const input = "echo hi hi < testing/tmp/file";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()

    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()
    ASSERT_NE(data.stdin, jsh::syscall_wrapper::stdin_file_descriptor);
}

TEST(TestProcess, TestInputRedirectionParsingBasic3) {
    std::string const input = "echo hi hi <\ttesting/tmp/file more args";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()

    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()
    ASSERT_NE(data.stdin, jsh::syscall_wrapper::stdin_file_descriptor);
}

TEST(TestProcess, TestInputRedirectionParsingMalformed1) {
    std::string const input = "echo hi hi <";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestInputRedirectionParsingMalformed2) {
    std::string const input = "echo hi hi < def/not/a/path/to/a/file";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestInputRedirectionParsingMalformed3) {
    std::string const input = "echo hi hi < testing/tmp/not_real";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestOutputRedirectionParsingBasic1) {
    std::string const input = "grep hi > testing/tmp/file";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()

    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()
    ASSERT_NE(data.stdout, jsh::syscall_wrapper::stdout_file_descriptor);
}

TEST(TestProcess, TestOutputRedirectionParsingBasic2) {
    std::string const input = "echo hi hi > testing/tmp/file";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()

    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()
    ASSERT_NE(data.stdout, jsh::syscall_wrapper::stdout_file_descriptor);
}

TEST(TestProcess, TestOutputRedirectionParsingBasic3) {
    std::string const input = "echo hi hi >\ttesting/tmp/file more args";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()

    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()
    ASSERT_NE(data.stdout, jsh::syscall_wrapper::stdout_file_descriptor);
}

TEST(TestProcess, TestOutputRedirectionParsingMalformed1) {
    std::string const input = "echo hi hi >";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestOutputRedirectionParsingMalformed2) {
    std::string const input = "echo hi hi > def/not/a/path/to/a/file";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestPosixNestedQuotes1) {
    std::string const input = R"(echo "a'awda'a")";
    std::vector<std::string> correct{"echo", R"(a'awda'a)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotes2) {
    std::string const input = R"(echo "a'awda'")";
    std::vector<std::string> correct{"echo", R"(a'awda')"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotes3) {
    std::string const input = R"(echo "'awda'a")";
    std::vector<std::string> correct{"echo", R"('awda'a)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixAdjacentQuotes1) {
    std::string const input = R"(echo a"'awda'a"a)";
    std::vector<std::string> correct{"echo", R"(a'awda'aa)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixAdjacentQuotes2) {
    std::string const input = R"(echo a"'awda'a"r)";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotesAndFilenames1) {
    std::string const input = R"(echo a"'awda'a"r > apwodpk"adjwojo"a)";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotesAndFilenames2) {
    std::string const input = R"(echo a"'awda'a"r >>> "adjwojo"a)";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotesAndFilenames3) {
    std::string const input = R"(echo a"'awda'a"r >>> adwad"adjwojo")";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotesAndFilenames4) {
    std::string const input = R"(echo a"'awda'a"r >>> "adjwojo")";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixFilenameMalformed) {
    std::string const input = R"(echo a"'awda'a"r >>>)";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestPosixUnclosedEscape) {
    std::string const input = R"(echo a"'awda'a"r\)";

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestPosixNestedQuotesEscape1) {
    std::string const input = R"(echo "a'awda'a")";
    std::vector<std::string> correct{"echo", R"(a'awda'a)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotes2) {
    std::string const input = R"(echo "a'awda'")";
    std::vector<std::string> correct{"echo", R"(a'awda')"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotes3) {
    std::string const input = R"(echo "'awda'a")";
    std::vector<std::string> correct{"echo", R"('awda'a)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixAdjacentQuotes1) {
    std::string const input = R"(echo a"'awda'a"a)";
    std::vector<std::string> correct{"echo", R"(a'awda'aa)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixAdjacentQuotes2) {
    std::string const input = R"(echo a"'awda'a"r)";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotesAndFilenames1) {
    std::string const input = R"(echo a"'awda'a"r > apwodpk"adjwojo"a)";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotesAndFilenames2) {
    std::string const input = R"(echo a"'awda'a"r >>> "adjwojo"a)";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotesAndFilenames3) {
    std::string const input = R"(echo a"'awda'a"r >>> adwad"adjwojo")";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}

TEST(TestProcess, TestPosixNestedQuotesAndFilenames4) {
    std::string const input = R"(echo a"'awda'a"r >>> "adjwojo")";
    std::vector<std::string> correct{"echo", R"(a'awda'ar)"};

    auto proc_data = jsh::process::parse_process(input);

    // make sure the command was invalid
    ASSERT_TRUE(proc_data.has_value());
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value())); // NOLINT assert catches this .value()
    auto& data = std::get<jsh::binary_data>(*proc_data.value()); // NOLINT assert catches this .value()

    // compare the arguments
    for(std::size_t i = 0; i < correct.size(); ++i){
        ASSERT_STREQ(correct[i].c_str(), data.args[i].c_str());
    }
}
