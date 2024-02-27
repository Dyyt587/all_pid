cmake_minimum_required (VERSION 2.8)

project (demo)

aux_source_directory(../src/ DIR_SRCS)

include_directories(../src/)

add_executable(main main.c  ${DIR_SRCS})



