#pragma once

#include "pch.hpp"

// JSH
#include "macros.hpp"
#include "process.hpp"

namespace jsh {
struct job_data;

class job {
  public:
    /**
     * parse_job: takes in user inputs and separates them into individual process commands
     *
     * input: the users input which will be split by process
     */
    [[nodiscard]] static auto parse_job(std::string const& input) -> std::unique_ptr<job_data>;

    /**
     * execute_job: executes a job
     *
     * data: job_data structure which describes how to execute the job
     */
    static void execute_job(std::unique_ptr<job_data>& data);

    /**
     * OPERATOR: enum which describes what operator is used to chain together a series of processes
     */
    enum OPERATOR : char {
        AND = 0,
        PIPE = 1,
        COUNT = 2
    };

  private:
    /**
     * CONSTANTS
     */
    static constexpr char const* OPERATOR_STR[OPERATOR::COUNT] = {"&&", "|"}; // NOLINT

    static constexpr std::size_t OPERATOR_LENGTH[OPERATOR::COUNT] = {std::string_view(OPERATOR_STR[OPERATOR::AND]).size(), std::string_view(OPERATOR_STR[OPERATOR::PIPE]).size()}; // NOLINT
    static_assert(OPERATOR_LENGTH[OPERATOR::AND] == 2, "&& operator not correct length");
    static_assert(OPERATOR_LENGTH[OPERATOR::PIPE] == 1, "| operator not correct length");
};

/**
 * job_data:
 *
 * NOTES: SoA is used since we will wont need to access both data inbesides during initial population
 */
// TODO (john): use memory prefetching to grab this process structure while the current one is running since we are using lists
static constexpr std::size_t JOB_DATA_ALIGNMENT = 128;
struct __attribute__((packed)) __attribute__((aligned(JOB_DATA_ALIGNMENT))) job_data {
    /**
     * input_seq: the sequence of commands which make up a job
     */
    std::vector<std::string> input_seq;

    /**
     * operator_seq: the sequence of operators which make up the job
     */
    std::vector<job::OPERATOR> operator_seq;

    /**
     * process_seq: the sequence of processes which make up a job
     */
    std::vector<std::unique_ptr<process_data>> process_seq;

    /**
     * is_foreground: indicates whether the job will be executing in the foreground or background
     */
    bool is_foreground = true;

    /**
     * pgid: the process group id for the job
     */
    std::shared_ptr<pid_t> pgid;
};
} // namespace jsh
