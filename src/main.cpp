#include "pch.hpp"

int main(int argc, char** argv){
    std::cout << "Welcome to John's Shell\n";

    while(true){
        std::string input;
        std::cin >> input;

        jsh::log(jsh::LOG_LEVEL::STATUS, input);
    }
}
