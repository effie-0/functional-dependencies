cmake_minimum_required(VERSION 3.10)
project(FD)

set(CMAKE_CXX_STANDARD 11)

add_executable(FD main.cpp solver.h solver.cpp base.h base.cpp test_base.cpp)