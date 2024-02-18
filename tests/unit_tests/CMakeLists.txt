cmake_minimum_required(VERSION 3.16.0)
project(hash-server-unit-tests VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (Sources
    hasher_tests.cpp
    socket_address_tests.cpp
)

add_executable(${PROJECT_NAME} ${Sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_link_libraries(${PROJECT_NAME} PUBLIC gmock_main hash-server-lib)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})