FROM mcr.microsoft.com/devcontainers/base:focal

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -y install gcc-10 g++-10 clang-format clang-tidy cmake
