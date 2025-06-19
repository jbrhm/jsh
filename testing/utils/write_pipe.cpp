// UNIX
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

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
        return 1;
    }

    // open the pipe and write to it
    int fd = open(pipe_name.c_str(), O_CREAT | O_RDWR);
    if(fd == -1){
        std::cout << "Error opening pipe: " << strerror(errno) << '\n';
        return 1;
    }

    // read in from stdin
    // TODO: REDO this portion
    assert(false);
    std::string str;
    while(std::cin >> str){
        int num_bytes_written = 0;

        // write the string to the pipe
        while(num_bytes_written != str.size()){
            ssize_t num_written = write(fd, str.c_str(), str.size());

            if(num_written == -1){
                std::cout << "Error writing to pipe: " << strerror(errno) << '\n';
                return 1;
            }

            num_bytes_written += static_cast<int>(num_written);
        }

        int num_bytes_read = 0;

        // write the string to the pipe
        while(num_bytes_read != str.size()){
            ssize_t num_read = read(fd, str.data(), str.size());

            if(num_read == -1){
                std::cout << "Error reading from pipe: " << strerror(errno) << '\n';
                return 1;
            }

            num_bytes_read += static_cast<int>(num_read);
        }

        std::cout << str << '\n';
    }

    return 0;
}
