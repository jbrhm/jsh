// GTEST
#include <gtest/gtest.h>

// JSH
#include <job.hpp>
#include <posix_wrappers.hpp>

TEST(TestJob, TestParseJobNoOperators){
    // input
    std::string input = "echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 1);
    ASSERT_TRUE(j->operator_seq.size() == 0);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi");
}

TEST(TestJob, TestParseJobAndBasic1){
    // input
    std::string input = "echo hi && echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi ");
    ASSERT_STREQ(j->input_seq[1].c_str(), " echo hi");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndBasic2){
    // input
    std::string input = "echo hi&& echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi");
    ASSERT_STREQ(j->input_seq[1].c_str(), " echo hi");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndBasic3){
    // input
    std::string input = "echo hi &&echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi ");
    ASSERT_STREQ(j->input_seq[1].c_str(), "echo hi");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndBasic4){
    // input
    std::string input = "echo hi&&echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi");
    ASSERT_STREQ(j->input_seq[1].c_str(), "echo hi");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge1){
    // input
    std::string input = "&&";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "");
    ASSERT_STREQ(j->input_seq[1].c_str(), "");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge2){
    // input
    std::string input = "&&&";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "");
    ASSERT_STREQ(j->input_seq[1].c_str(), "&");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge3){
    // input
    std::string input = "&&&&";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 3);
    ASSERT_TRUE(j->operator_seq.size() == 2);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "");
    ASSERT_STREQ(j->input_seq[1].c_str(), "");
    ASSERT_STREQ(j->input_seq[2].c_str(), "");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
    ASSERT_EQ(j->operator_seq[1], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge4){
    // input
    std::string input = "&&a&& command";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 3);
    ASSERT_TRUE(j->operator_seq.size() == 2);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "");
    ASSERT_STREQ(j->input_seq[1].c_str(), "a");
    ASSERT_STREQ(j->input_seq[2].c_str(), " command");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
    ASSERT_EQ(j->operator_seq[1], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobAndEdge5){
    // input
    std::string input = "aa&& command";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "aa");
    ASSERT_STREQ(j->input_seq[1].c_str(), " command");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::AND);
}

TEST(TestJob, TestParseJobPipeBasic1){
    // input
    std::string input = "echo hi | echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi ");
    ASSERT_STREQ(j->input_seq[1].c_str(), " echo hi");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeBasic2){
    // input
    std::string input = "echo hi| echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi");
    ASSERT_STREQ(j->input_seq[1].c_str(), " echo hi");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeBasic3){
    // input
    std::string input = "echo hi |echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi ");
    ASSERT_STREQ(j->input_seq[1].c_str(), "echo hi");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeBasic4){
    // input
    std::string input = "echo hi|echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "echo hi");
    ASSERT_STREQ(j->input_seq[1].c_str(), "echo hi");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeEdge1){
    // input
    std::string input = "|";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "");
    ASSERT_STREQ(j->input_seq[1].c_str(), "");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
}


TEST(TestJob, TestParseJobPipeEdge2){
    // input
    std::string input = "||";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 3);
    ASSERT_TRUE(j->operator_seq.size() == 2);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "");
    ASSERT_STREQ(j->input_seq[1].c_str(), "");
    ASSERT_STREQ(j->input_seq[2].c_str(), "");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
    ASSERT_EQ(j->operator_seq[1], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeEdge3){
    // input
    std::string input = "|a| command";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 3);
    ASSERT_TRUE(j->operator_seq.size() == 2);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "");
    ASSERT_STREQ(j->input_seq[1].c_str(), "a");
    ASSERT_STREQ(j->input_seq[2].c_str(), " command");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
    ASSERT_EQ(j->operator_seq[1], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestParseJobPipeEdge4){
    // input
    std::string input = "aa| command";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 0);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "aa");
    ASSERT_STREQ(j->input_seq[1].c_str(), " command");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
}

TEST(TestJob, TestExecuteJobBasic1){
    // create job data
    auto job = std::make_unique<jsh::job_data>();

    // output file name
    static constexpr char const* file = "testing/tmp/file";
    static constexpr char const* cmd = "echo hi > testing/tmp/file";
    static constexpr char const* corr = "hi\n";

    // push back this process
    job->input_seq.emplace_back(cmd);

    // execute the job
    jsh::job::execute_job(job);

    // open the file
    int fides = open(file, O_RDONLY);

    // check to see if open succeeded
    if(fides == -1){
        std::cout << "Error opening file: " << strerror(errno) << '\n';
        return;
    }

    // read from the pipe
    ssize_t num_bytes_read = 1;
    for(std::size_t i = 0; num_bytes_read != 0; ++i){
        char chr = '\0';
        num_bytes_read = read(fides, &chr, sizeof(chr));

        if(num_bytes_read == -1){
            std::cout << "Error reading from pipe: " << strerror(errno) << '\n';
            return;
        }
        
        ASSERT_TRUE(i < std::strlen(corr) + 1);
        std::cout << i << " " << chr << " " << corr[i] << '\n';
        ASSERT_TRUE(chr == corr[i]);
    }
}

TEST(TestJob, TestExecuteJobBasic2){
    // create job data
    auto job = std::make_unique<jsh::job_data>();

    // output file name
    static constexpr char const* file = "testing/tmp/file";
    static constexpr char const* cmd = "echo hi yo | grep -i hi> testing/tmp/file";
    static constexpr char const* corr = "hi\n";

    // push back this process
    job->input_seq.emplace_back(cmd);

    // execute the job
    jsh::job::execute_job(job);

    // open the file
    int fides = open(file, O_RDONLY);

    // check to see if open succeeded
    if(fides == -1){
        std::cout << "Error opening file: " << strerror(errno) << '\n';
        return;
    }

    // read from the pipe
    ssize_t num_bytes_read = 1;
    for(std::size_t i = 0; num_bytes_read != 0; ++i){
        char chr = '\0';
        num_bytes_read = read(fides, &chr, sizeof(chr));

        if(num_bytes_read == -1){
            std::cout << "Error reading from pipe: " << strerror(errno) << '\n';
            return;
        }
        
        ASSERT_TRUE(i < std::strlen(corr) + 1);
        std::cout << i << " " << chr << " " << corr[i] << '\n';
        ASSERT_TRUE(chr == corr[i]);
    }
}

TEST(TestJob, TestExecuteJobBasic3){
    // create job data
    auto job = std::make_unique<jsh::job_data>();

    // output file name
    static constexpr char const* file = "testing/tmp/file";
    static constexpr char const* file2 = "testing/tmp/file2";
    static constexpr char const* cmd = "echo linux > testing/tmp/file2 && echo hi yo | grep -i hi> testing/tmp/file";
    static constexpr char const* corr = "hi\n";
    static constexpr char const* corr2 = "linux ";

    // push back this process
    job->input_seq.emplace_back(cmd);

    // execute the job
    jsh::job::execute_job(job);

    // open the file
    int fides = open(file, O_RDONLY);

    // check to see if open succeeded
    if(fides == -1){
        std::cout << "Error opening file: " << strerror(errno) << '\n';
        return;
    }

    // read from the pipe
    ssize_t num_bytes_read = 1;
    for(std::size_t i = 0; num_bytes_read != 0; ++i){
        char chr = '\0';
        num_bytes_read = read(fides, &chr, sizeof(chr));

        if(num_bytes_read == -1){
            std::cout << "Error reading from pipe: " << strerror(errno) << '\n';
            return;
        }
        
        ASSERT_TRUE(i < std::strlen(corr) + 1);
        std::cout << i << " " << chr << " " << corr[i] << '\n';
        ASSERT_TRUE(chr == corr[i]);
    }

    close(fides);

    // open the file
    fides = open(file2, O_RDONLY);

    // check to see if open succeeded
    if(fides == -1){
        std::cout << "Error opening file: " << strerror(errno) << '\n';
        return;
    }

    // read from the pipe
    num_bytes_read = 1;
    for(std::size_t i = 0; num_bytes_read != 0; ++i){
        char chr = '\0';
        num_bytes_read = read(fides, &chr, sizeof(chr));

        if(num_bytes_read == -1){
            std::cout << "Error reading from pipe: " << strerror(errno) << '\n';
            return;
        }
        
        ASSERT_TRUE(i < std::strlen(corr2) + 1);
        std::cout << i << " " << chr << " " << corr2[i] << '\n';
        ASSERT_TRUE(chr == corr2[i]);
    }
}
