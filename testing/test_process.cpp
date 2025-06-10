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
    std::unique_ptr<jsh::process_data> binary_var = std::make_unique<jsh::process_data>(jsh::binary_data{});

    assert(std::holds_alternative<jsh::binary_data>(*binary_var));

    jsh::binary_data& binary = std::get<jsh::binary_data>(*binary_var);

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
    std::unique_ptr<jsh::process_data> export_var = std::make_unique<jsh::process_data>(jsh::export_data{});

    assert(std::holds_alternative<jsh::export_data>(*export_var));

    jsh::export_data& exp = std::get<jsh::export_data>(*export_var);

    exp.name = "name";
    exp.val = "val";

    // run the export internal
    jsh::process::execute(export_var);

    // assert that the environment variable was properly exported
    ASSERT_STREQ(jsh::environment::get_var("name"), "val");
}

TEST(TestProcess, TestInputRedirectionParsingBasic1){
    std::string input = "grep hi < testing/tmp/file";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value()));
    
    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value());
    ASSERT_NE(data.stdin, STDIN_FILENO);
}

TEST(TestProcess, TestInputRedirectionParsingBasic2){
    std::string input = "echo hi hi < testing/tmp/file";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value()));
    
    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value());
    ASSERT_NE(data.stdin, STDIN_FILENO);
}

TEST(TestProcess, TestInputRedirectionParsingBasic3){
    std::string input = "echo hi hi <\ttesting/tmp/file more args";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value()));
    
    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value());
    ASSERT_NE(data.stdin, STDIN_FILENO);
}

TEST(TestProcess, TestInputRedirectionParsingMalformed1){
    std::string input = "echo hi hi <";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestInputRedirectionParsingMalformed2){
    std::string input = "echo hi hi < def/not/a/path/to/a/file";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestInputRedirectionParsingMalformed3){
    std::string input = "echo hi hi < testing/tmp/not_real";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestOutputRedirectionParsingBasic1){
    std::string input = "grep hi > testing/tmp/file";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value()));
    
    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value());
    ASSERT_NE(data.stdout, STDOUT_FILENO);
}

TEST(TestProcess, TestOutputRedirectionParsingBasic2){
    std::string input = "echo hi hi > testing/tmp/file";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value()));
    
    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value());
    ASSERT_NE(data.stdout, STDOUT_FILENO);
}

TEST(TestProcess, TestOutputRedirectionParsingBasic3){
    std::string input = "echo hi hi >\ttesting/tmp/file more args";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was valid
    ASSERT_TRUE(proc_data.has_value());

    // make sure it contains binary data
    ASSERT_TRUE(std::holds_alternative<jsh::binary_data>(*proc_data.value()));
    
    // get reference to underlying data
    auto& data = std::get<jsh::binary_data>(*proc_data.value());
    ASSERT_NE(data.stdout, STDOUT_FILENO);
}

TEST(TestProcess, TestOutputRedirectionParsingMalformed1){
    std::string input = "echo hi hi >";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}

TEST(TestProcess, TestOutputRedirectionParsingMalformed2){
    std::string input = "echo hi hi > def/not/a/path/to/a/file";

    auto proc_data = jsh::process::parse_process(input);
    
    // make sure the command was invalid
    ASSERT_FALSE(proc_data.has_value());
}
