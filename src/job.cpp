#include "job.hpp"

namespace jsh {
    auto job::parse_job(std::string const& input) -> std::unique_ptr<job_data>{
        // stack allocated struct for job_data
        std::unique_ptr<job_data> j_data = std::make_unique<job_data>();

        // structure for information about operators

        struct op_data {
            std::size_t index;
            OPERATOR op;
        };

        // search for all instances of AND operator
        std::vector<op_data> indices;
        auto find_operators = [&](OPERATOR op){
            op_data op_d;
            op_d.op = op;
            op_d.index = 0;
            while(true) {
                // find the first instance of the operator
                op_d.index = input.find(OPERATOR_STR[op], op_d.index);

                // check to see if find was successful
                if(op_d.index == std::string::npos) [[likely]]{
                    break;
                }

                // after this point we should no longer be dealing with npos
                assert(op_d.index != std::string::npos);

                // ensure that this index is unique
                assert(std::find_if(std::begin(indices), std::end(indices), [&](op_data const& op){return op.index == op_d.index;}) == std::end(indices));

                // add index
                indices.push_back(op_d);

                // increment beyond operator
                op_d.index += OPERATOR_LENGTH[op];
            }

            return;
        };

        // check for all of the operators
        for(char op = OPERATOR::AND; op < OPERATOR::COUNT; ++op){
            find_operators(static_cast<OPERATOR>(op));
        }

        // populate job_data
        // sort based on indices, so commands will happen in the correct order
        std::sort(std::begin(indices), std::end(indices), [](op_data const& lhs, op_data const& rhs){return lhs.index < rhs.index;});

        // resize job vectors
        j_data->input_seq.reserve(indices.size() + 1);
        j_data->process_seq.reserve(indices.size() + 1);
        j_data->operator_seq.reserve(indices.size());

        // split the strings based on processes
        std::size_t index = 0;
        for(auto const& i : indices){
            // we don't want this to overflow
            assert(i.index >= index);

            j_data->input_seq.push_back(input.substr(index, i.index - index));
            index = i.index + OPERATOR_LENGTH[i.op];
        }

        // push back the final process command
        j_data->input_seq.push_back(input.substr(index, input.size()));

        // copy over all of the operators
        for(op_data const& o: indices){
            j_data->operator_seq.push_back(o.op);
        }

        // make sure all of the sequences are appropriately sized
        assert(j_data->operator_seq.size() == j_data->input_seq.size() - 1);

        return j_data;
    }

    void job::execute_job(std::unique_ptr<job_data>& data){
        // parse all of the individual process inputs
        for(std::string const& input : data->input_seq){
            // parse the users input into a data describing a specific process
            std::optional<std::unique_ptr<jsh::process_data>> proc_data = jsh::process::parse_process(input);

            // check the to see if the input was valid
            if(!proc_data.has_value()){ // invalid
                cout_logger.log(LOG_LEVEL::ERROR, "Error Parsing Process...");
                return;
            }

            // move the data from the optional into the vector
            assert(proc_data.has_value());
            data->process_seq.emplace_back(std::move(proc_data.value()));
        }

        // perform the process' execution
        assert(data->input_seq.size() == data->process_seq.size());
        assert(data->input_seq.size() == data->operator_seq.size() + 1);
        for(std::size_t i = 0; i < data->process_seq.size(); ++i){
            // get a reference to the process_data
            std::unique_ptr<process_data>& proc_data = data->process_seq[i];

            // if on last command dont do any checks
            if(i + 1 != data->process_seq.size()){
                // we should not have any invalid operators at this point
                assert(data->operator_seq[i] < OPERATOR::COUNT);

                // Check if the current process' output is being piped somewhere else
                if(data->operator_seq[i] == OPERATOR::PIPE){
                    // create the pipe
                    int pipe_fds[2];
                    CHECK_PIPE(pipe(pipe_fds),);

                    // set the current output and the next input to read from the pipe
                    std::visit([&](auto&& var){var.stdout = pipe_fds[1];}, *proc_data);
                    // there will always be another process since this is being piped somewhere else
                    assert(data->process_seq.size() > i);
                    std::visit([&](auto&& var){var.stdin = pipe_fds[0];}, *data->process_seq[i+1]);
                }
            }

            // execute the process
            jsh::process::execute(proc_data);
        }
    }
}// namespace jsh
