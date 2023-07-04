cmake_minimum_required(VERSION 3.23)
project(EE3490E_KTLT C)

set(CMAKE_C_STANDARD 14)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ ")



add_executable(EE3490E_KTLT
    task2/task2_main.c)

