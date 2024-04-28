cmake_minimum_required (VERSION 3.7)

project (demo)

aux_source_directory(../src/ DIR_SRCS)

include_directories(../src/)

add_executable(main main.c  ${DIR_SRCS})

execute_process(COMMAND ls .. 
                OUTPUT_VARIABLE ls_ret)

                message(STATUS "ls output:\n${ls_ret}")


