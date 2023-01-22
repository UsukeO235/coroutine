cmake_minimum_required(VERSION 3.5)

project(gcc_x64_printf)

add_compile_options(-std=c11 -O2 -Wall)

include_directories(
    ${CMAKE_SOURCE_DIR}/../../include
)

add_library(coroutine_kernel
    ${CMAKE_SOURCE_DIR}/../../src/coroutine_kernel.c
)

add_executable(gcc_x64_printf
    ${CMAKE_SOURCE_DIR}/main.c
)

target_link_libraries(gcc_x64_printf
    coroutine_kernel
)