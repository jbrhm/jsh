// UNIX
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>

// STD
#include <cassert>
#include <iostream>
#include <string>
#include <string.h>

int main(int argc, char** argv){
    std::string pipe_name;

    int c;
    while(true){
        int option = 0;
        static constexpr struct option options[] = {
            {"name", required_argument, nullptr, 'n'},
            {nullptr, 0, nullptr, 0}
        };

        int indx = 0;

        c = getopt_long(argc, argv, "n:", options, &indx);

        if(c == -1)
            break;

        switch(c){
            case 0: {
                assert(false);
                break;
            }
            case 'n': {
                pipe_name = optarg;
                break;
            }
            default: {
                break;
            }
        }
    }

    std::cout << pipe_name << '\n';

    // define pipe params
    static constexpr mode_t perms = 0666;

    // create the pipe
    if(mkfifo(pipe_name.c_str(), perms) == -1){
        std::cout << "Error making pipe: " << strerror(errno) << '\n';
    }

    // open the pipe and write to it
    int fd = open()

    return 0;
}
