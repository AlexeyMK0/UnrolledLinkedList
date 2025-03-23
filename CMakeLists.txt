cmake_minimum_required(VERSION 3.12)

project(
    unrolled_list
    LANGUAGES CXX
)

# add_compile_definitions(ADD_DEBUG_PRINT)

set(CMAKE_CXX_STANDARD 23)

include_directories(lib)

add_subdirectory(bin)
add_subdirectory(lib)

enable_testing()
add_subdirectory(tests)
