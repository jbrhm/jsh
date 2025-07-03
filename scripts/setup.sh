#! /usr/bin/bash

wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 20

sudo apt update
sudo apt install libc++-dev libc++abi-dev
