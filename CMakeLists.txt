cmake_minimum_required(VERSION 3.25)
project(EE3490E_KTLT C)

set(CMAKE_C_STANDARD 11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ ")

set(CMAKE_EXE_LINKER_FLAGS "-static")


add_executable(EE3490E_KTLT
    task2/task2_main.c)

