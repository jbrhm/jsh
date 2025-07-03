#! /usr/bin/bash

wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 20

sudo apt update
sudo apt install libc++-dev libc++abi-dev cmake ninja-build

sudo ln -s /usr/bin/clang++-20 /usr/bin/clang++
sudo ln -s /usr/bin/clang-20 /usr/bin/clang

git submodule update --init --recursive
