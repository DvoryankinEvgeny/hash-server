cmake_minimum_required(VERSION 3.16.0)
project(hash-server-integration-tests VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (Sources
    main.cpp
    server_tests.cpp
)

add_executable(${PROJECT_NAME} ${Sources})

include(GoogleTest)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_link_libraries(${PROJECT_NAME} PUBLIC gtest gmock hash-server-lib)
