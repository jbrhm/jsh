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

    int opt_char;
    while(true){
        int option = 0;
        static constexpr struct option options[] = {
            {"name", required_argument, nullptr, 'n'},
            {nullptr, 0, nullptr, 0}
        };

        int indx = 0;

        opt_char = getopt_long(argc, argv, "n:", options, &indx);

        if(opt_char == -1)
            break;

        switch(opt_char){
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


    // open the pipe and write to it
    int fd = open(pipe_name.c_str(), O_CREAT | O_WRONLY);
    if(fd == -1){
        std::cout << "Error opening pipe: " << strerror(errno) << '\n';
        return 1;
    }

    while(true){
        char c = '\0';
        ssize_t num_bytes_read = read(STDIN_FILENO, &c, sizeof(c));

        std::cout << c << '\n';

        if(num_bytes_read == -1){ // error occurred
            std::cout << "Error reading from pipe: " << strerror(errno) << '\n';
            return 1;
        }

        if(num_bytes_read == 0){ // read EOF 
            break;
        }

        // we should only be able to read one byte
        assert(num_bytes_read == 1);

        // write the char out to the pipe
        ssize_t num_bytes_wrote = 0;
        while(num_bytes_wrote == 0){
            num_bytes_wrote = write(fd, &c, sizeof(c));

            if(num_bytes_wrote == -1){ // error occurred
                std::cout << "Error writing to pipe: " << strerror(errno) << '\n';
                return 1;
            }
        }
    }

    // close the fd
    close(fd);

    return 0;
}
