// GTEST
#include <gtest/gtest.h>

// JSH
#include <job.hpp>

TEST(TestJob, TestParseJobNoOperators){
    // input
    std::string input = "echo hi";

    auto j = jsh::job::parse_job(input);

    // ensure correct sizing
    ASSERT_TRUE(j->process_seq.size() == 1);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 3);
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
    ASSERT_TRUE(j->process_seq.size() == 3);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
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
    ASSERT_TRUE(j->process_seq.size() == 3);
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
    ASSERT_TRUE(j->process_seq.size() == 3);
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
    ASSERT_TRUE(j->process_seq.size() == 2);
    ASSERT_TRUE(j->input_seq.size() == 2);
    ASSERT_TRUE(j->operator_seq.size() == 1);

    // ensure correct contents for input and operator sequence
    ASSERT_STREQ(j->input_seq[0].c_str(), "aa");
    ASSERT_STREQ(j->input_seq[1].c_str(), " command");
    ASSERT_EQ(j->operator_seq[0], jsh::job::OPERATOR::PIPE);
}
