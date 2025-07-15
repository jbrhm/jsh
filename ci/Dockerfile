FROM ubuntu:latest

ARG UID=1069
ARG GID=1000
ARG DOCKER_USER="docker"

RUN apt-get update

RUN apt-get -y install sudo

RUN apt-get update

RUN apt-get -y install wget lsb-release curl gnupg software-properties-common git cmake ninja-build

RUN wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 20

RUN apt-get -y install clang-tidy-20 clang-format-20

RUN sudo ln -s /usr/bin/clang++-20 /usr/bin/clang++ && sudo ln -s /usr/bin/clang-20 /usr/bin/clang && sudo ln -s /usr/bin/clang-tidy-20 /usr/bin/clang-tidy && sudo ln -s /usr/bin/clang-format-20 /usr/bin/clang-format
