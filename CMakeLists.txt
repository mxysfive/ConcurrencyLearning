cmake_minimum_required(VERSION 3.20)
project(ConcurrencyLearning)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads)
add_executable(singleton singleton.cpp)
target_link_libraries (singleton ${CMAKE_THREAD_LIBS_INIT})