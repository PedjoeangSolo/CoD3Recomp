cmake_minimum_required(VERSION 3.22.1)
project(cod3android LANGUAGES CXX)

add_library(cod3android SHARED
    native-lib.cpp
    ppc_guest_memory.cpp
    ppc_runtime_tests.cpp
)

find_library(log-lib log)

target_compile_features(cod3android PRIVATE cxx_std_17)
target_compile_options(cod3android PRIVATE -Wall -Wextra -Werror)
target_link_libraries(cod3android PRIVATE ${log-lib})
