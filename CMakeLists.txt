cmake_minimum_required(VERSION 3.10)
project(caesar_cipher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(caesar_cipher main.cpp caesar_cipher.cpp)

# Настройка форматирования
add_custom_target(
    clang-format
    COMMAND find . -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format -i -style=file
    COMMENT "Форматирование кода с помощью clang-format"
)
