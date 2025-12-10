cmake_minimum_required(VERSION 3.10)
project(Homework VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found!")
    endif()
endif()

add_executable(homework homework.cpp)

set_target_properties(homework PROPERTIES OUTPUT_NAME "homework")

add_custom_command(TARGET homework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:homework> ${CMAKE_SOURCE_DIR}/
)
