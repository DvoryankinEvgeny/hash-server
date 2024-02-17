cmake_minimum_required(VERSION 3.16.0)
project(hash-server VERSION 0.1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(hash-server main.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        pthread
        OpenSSL::SSL
        hash-server-lib
    )