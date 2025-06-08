#pragma once

#include "pch.hpp"

// JSH
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
        static auto parse_job(std::string const& input) -> std::unique_ptr<job_data>;

        /**
         * OPERATOR: enum which describes what operator is used to chain together a series of processes
         */
        enum OPERATOR : char {
            AND = 0,
            PIPE = 1,
            COUNT
        };

    private:
        /**
         * CONSTANTS
         */
        static constexpr char const* OPERATOR_STR[OPERATOR::COUNT] = {"&&", "|"};

        static constexpr std::size_t OPERATOR_LENGTH[OPERATOR::COUNT] = {std::string_view(OPERATOR_STR[OPERATOR::AND]).size(), std::string_view(OPERATOR_STR[OPERATOR::PIPE]).size()};
        static_assert(OPERATOR_LENGTH[OPERATOR::AND] == 2, "&& operator not correct length");
        static_assert(OPERATOR_LENGTH[OPERATOR::PIPE] == 1, "| operator not correct length");
    };

    /**
     * job_data: 
     *
     * NOTES: SoA is used since we will wont need to access both data inbesides during initial population
     */
    // TODO: use memory prefetching to grab this process structure while the current one is running since we are using lists
    struct job_data {
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
        std::vector<process_data> process_seq;
    };
} // namespace jsh
