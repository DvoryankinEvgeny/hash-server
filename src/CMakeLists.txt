cmake_minimum_required(VERSION 3.16.0)
project(hash-server-lib VERSION 0.1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (Sources
    hasher.cpp
    polling.cpp
    server.cpp
    socket.cpp
    socket_address.cpp
    thread_pool.cpp
)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(${PROJECT_NAME} STATIC ${Sources})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        pthread
        OpenSSL::SSL
    )