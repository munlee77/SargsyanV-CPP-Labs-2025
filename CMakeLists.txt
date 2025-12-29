cmake_minimum_required(VERSION 3.10)

project(Lab6_Numerical_Integration)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(lab6_integration lab6.cpp)

set_target_properties(lab6_integration PROPERTIES OUTPUT_NAME "lab6")

add_custom_target(clang-format
    COMMAND find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i
    COMMENT "Форматирование кода с помощью clang-format"
)
