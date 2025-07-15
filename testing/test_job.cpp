// GTEST
#include <gtest/gtest.h>

// JSH
#include <job.hpp>
#include <posix_wrappers.hpp>

TEST(TestJob, TestParseJobNoOperators){
    // input
    std::string input = "echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 1);
    ASSERT_TRUE(job->operator_seq.size() == 0);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi");
}

TEST(TestJob, TestParseJobAndBasic1){
    // input
    std::string input = "echo hi && echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi ");
    ASSERT_STREQ(job->input_seq[1].c_str(), " echo hi");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndBasic2){
    // input
    std::string input = "echo hi&& echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi");
    ASSERT_STREQ(job->input_seq[1].c_str(), " echo hi");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndBasic3){
    // input
    std::string input = "echo hi &&echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi ");
    ASSERT_STREQ(job->input_seq[1].c_str(), "echo hi");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndBasic4){
    // input
    std::string input = "echo hi&&echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi");
    ASSERT_STREQ(job->input_seq[1].c_str(), "echo hi");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge1){
    // input
    std::string input = "&&";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "");
    ASSERT_STREQ(job->input_seq[1].c_str(), "");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge2){
    // input
    std::string input = "&&&";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "");
    ASSERT_STREQ(job->input_seq[1].c_str(), "&");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge3){
    // input
    std::string input = "&&&&";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 3);
    ASSERT_TRUE(job->operator_seq.size() == 2);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "");
    ASSERT_STREQ(job->input_seq[1].c_str(), "");
    ASSERT_STREQ(job->input_seq[2].c_str(), "");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
    ASSERT_EQ(job->operator_seq[1], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge4){
    // input
    std::string input = "&&a&& command";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 3);
    ASSERT_TRUE(job->operator_seq.size() == 2);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "");
    ASSERT_STREQ(job->input_seq[1].c_str(), "a");
    ASSERT_STREQ(job->input_seq[2].c_str(), " command");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
    ASSERT_EQ(job->operator_seq[1], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge5){
    // input
    std::string input = "aa&& command";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "aa");
    ASSERT_STREQ(job->input_seq[1].c_str(), " command");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobPipeBasic1){
    // input
    std::string input = "echo hi | echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi ");
    ASSERT_STREQ(job->input_seq[1].c_str(), " echo hi");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeBasic2){
    // input
    std::string input = "echo hi| echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi");
    ASSERT_STREQ(job->input_seq[1].c_str(), " echo hi");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeBasic3){
    // input
    std::string input = "echo hi |echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi ");
    ASSERT_STREQ(job->input_seq[1].c_str(), "echo hi");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeBasic4){
    // input
    std::string input = "echo hi|echo hi";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "echo hi");
    ASSERT_STREQ(job->input_seq[1].c_str(), "echo hi");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeEdge1){
    // input
    std::string input = "|";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "");
    ASSERT_STREQ(job->input_seq[1].c_str(), "");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::PIPE);
}


TEST(TestJob, TestParseJobPipeEdge2){
    // input
    std::string input = "||";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 3);
    ASSERT_TRUE(job->operator_seq.size() == 2);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "");
    ASSERT_STREQ(job->input_seq[1].c_str(), "");
    ASSERT_STREQ(job->input_seq[2].c_str(), "");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::PIPE);
    ASSERT_EQ(job->operator_seq[1], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeEdge3){
    // input
    std::string input = "|a| command";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 3);
    ASSERT_TRUE(job->operator_seq.size() == 2);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "");
    ASSERT_STREQ(job->input_seq[1].c_str(), "a");
    ASSERT_STREQ(job->input_seq[2].c_str(), " command");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::PIPE);
    ASSERT_EQ(job->operator_seq[1], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeEdge4){
    // input
    std::string input = "aa| command";

    auto job = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(job->process_seq.size() == 0);
    ASSERT_TRUE(job->input_seq.size() == 2);
    ASSERT_TRUE(job->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(job->input_seq[0].c_str(), "aa");
    ASSERT_STREQ(job->input_seq[1].c_str(), " command");
    ASSERT_EQ(job->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestExecuteJobBasic1){
    // Test Constants
    static constexpr char const* FILE = "testing/tmp/file";
    static constexpr char const* CMD = "echo test1 > testing/tmp/file";
    static constexpr char const* CORR = "test1\n";

    // parse the command
    auto job = jsh::job::parse_job(CMD);

    // not actually in the terminal so we use background processes
    job->is_foreground = false;

    // execute the job
    jsh::job::execute_job(job);

    // open the file
    static constexpr mode_t MODE = 0777;
    std::optional<jsh::file_descriptor_wrapper> fides = jsh::syscall_wrapper::open_wrapper(FILE, O_RDONLY, MODE);

    // check to see if open succeeded
    ASSERT_TRUE(fides.has_value());

    // read from the pipe
    std::optional<ssize_t> num_bytes_read = std::make_optional<ssize_t>(1);
    std::size_t idx = 0;
    for(; num_bytes_read.has_value() && num_bytes_read.value() != 0; ++idx){
        char chr = '\0';
        num_bytes_read = jsh::syscall_wrapper::read_wrapper(fides.value(), &chr, sizeof(chr));

        // check for success
        if(!num_bytes_read.has_value()){
            return;
        }
        
        ASSERT_TRUE(idx < std::strlen(CORR) + 1);
        EXPECT_TRUE(chr == CORR[idx]);
    }
    ASSERT_EQ(idx, std::strlen(CORR) + 1);
}

TEST(TestJob, TestExecuteJobBasic2){
    // Test constants
    static constexpr char const* file = "testing/tmp/file";
    static constexpr char const* cmd = "echo hi test2 yo | grep -i hi > testing/tmp/file";
    static constexpr char const* corr = "hi test2 yo\n";

    // create a job
    auto job = jsh::job::parse_job(cmd);

    // not actually in the terminal so we use background processes
    job->is_foreground = false;

    // execute the job
    jsh::job::execute_job(job);

    // open the file
    std::optional<jsh::file_descriptor_wrapper> fides = jsh::syscall_wrapper::open_wrapper(file, O_RDONLY, 0777);

    // check to see if open succeeded
    ASSERT_TRUE(fides.has_value());

    // read from the pipe
    std::size_t i = 0;
    std::optional<ssize_t> num_bytes_read = std::make_optional<ssize_t>(1);
    for(; num_bytes_read.has_value() && num_bytes_read.value() != 0; ++i){
        char chr = '\0';
        num_bytes_read = jsh::syscall_wrapper::read_wrapper(fides.value(), &chr, sizeof(chr));

        // check for success
        if(!num_bytes_read.has_value()){
            return;
        }
        
        ASSERT_TRUE(i < std::strlen(corr) + 1);
        EXPECT_EQ(chr, corr[i]);
    }
    ASSERT_EQ(i, std::strlen(corr) + 1);
}

TEST(TestJob, TestExecuteJobBasic3){
    // Test constants
    static constexpr char const* file = "testing/tmp/file";
    static constexpr char const* file2 = "testing/tmp/file2";
    static constexpr char const* cmd = "echo linux test3 > testing/tmp/file2 && echo test3 hi yo | grep -i hi > testing/tmp/file";
    static constexpr char const* corr = "test3 hi yo\n";
    static constexpr char const* corr2 = "linux test3\n";

    // parse job
    auto job = jsh::job::parse_job(cmd);

    // not actually in the terminal so we use background processes
    job->is_foreground = false;

    // execute the job
    jsh::job::execute_job(job);

    { // fides wrapper
        // open the file
        std::optional<jsh::file_descriptor_wrapper> fides = jsh::syscall_wrapper::open_wrapper(file, O_RDONLY, 0777);

        // check to see if open succeeded
        ASSERT_TRUE(fides.has_value());

        // read from the pipe
        std::optional<ssize_t> num_bytes_read = std::make_optional<ssize_t>(1);
        std::size_t i = 0;
        for(; num_bytes_read.has_value() && num_bytes_read.value() != 0; ++i){
            char chr = '\0';
            num_bytes_read = jsh::syscall_wrapper::read_wrapper(fides.value(), &chr, sizeof(chr));

            // check for success
            if(!num_bytes_read.has_value()){
                return;
            }
            
            ASSERT_TRUE(i < std::strlen(corr) + 1);
	    ASSERT_EQ(chr, corr[i]);
        }

        ASSERT_EQ(i, std::strlen(corr) + 1);
    }

    { // fides wrapper
        // open the file
        std::optional<jsh::file_descriptor_wrapper> fides = jsh::syscall_wrapper::open_wrapper(file2, O_RDONLY, 0777);

        // check to see if open succeeded
        ASSERT_TRUE(fides.has_value());

        // read from the pipe
        std::optional<ssize_t> num_bytes_read = std::make_optional<ssize_t>(1);
        for(std::size_t i = 0; num_bytes_read.has_value() && num_bytes_read.value() != 0; ++i){
            char chr = '\0';
            num_bytes_read = jsh::syscall_wrapper::read_wrapper(fides.value(), &chr, sizeof(chr)); // NOLINT

            // check for success
            if(!num_bytes_read.has_value()){
                return;
            }
            
            ASSERT_TRUE(i < std::strlen(corr2) + 1);
            EXPECT_EQ(chr, corr2[i]);
        }
    }
}
