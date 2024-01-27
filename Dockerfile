FROM mcr.microsoft.com/devcontainers/base:focal

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -y install gcc-10 clang-format cmake
