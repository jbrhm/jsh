// GTEST
#include <gtest/gtest.h>

// JSH
#include <job.hpp>

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
