cmake_minimum_required(VERSION 3.16.0)
project(hash-server-tests VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(../3rdparty/googletest/CMakeLists.txt.in ../3rdparty/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

set (Sources
    hasher_tests.cpp
    server_tests.cpp
    socket_address_tests.cpp
)

add_executable(${PROJECT_NAME} ${Sources})
target_link_libraries(${PROJECT_NAME} PUBLIC gmock_main hash-server-lib)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})