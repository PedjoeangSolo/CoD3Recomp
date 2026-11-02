cmake_minimum_required(VERSION 3.22.1)
project(cod3android LANGUAGES CXX)

add_library(cod3android SHARED
    native-lib.cpp
    ppc_guest_memory.cpp
    ppc_runtime_tests.cpp
    ppc_context_probe.cpp
)

set(XENON_RECOMP_ROOT "${CMAKE_SOURCE_DIR}/tools/XenonRecomp")
target_include_directories(cod3android PRIVATE
    "${XENON_RECOMP_ROOT}/XenonUtils"
    "${XENON_RECOMP_ROOT}/thirdparty/simde"
)

find_library(log-lib log)

target_compile_features(cod3android PRIVATE cxx_std_20)
target_compile_options(cod3android PRIVATE -Wall -Wextra -Werror)
target_link_libraries(cod3android PRIVATE ${log-lib})
