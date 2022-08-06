cmake_minimum_required(VERSION 3.22)
project(chelps C)

set(CMAKE_C_STANDARD 99)

add_library(chelps SHARED library.c)
