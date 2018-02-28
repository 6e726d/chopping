cmake_minimum_required(VERSION 3.5)
project(chopping)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c interface.c wireless.c)
add_executable(chopping ${SOURCE_FILES})