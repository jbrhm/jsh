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
        j_data->process_seq.resize(indices.size() + 1);
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
        assert(j_data->input_seq.size() == j_data->process_seq.size());

        return j_data;
    }
}// namespace jsh
