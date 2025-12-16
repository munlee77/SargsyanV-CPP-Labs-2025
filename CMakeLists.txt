# Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(Lab6_Numerical_Integration)

# Устанавливаем стандарт C++ (C++11 или новее)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция для включения предупреждений компилятора
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Создаем исполняемый файл из исходного кода
add_executable(lab6_integration lab6.cpp)

# Альтернативное имя для исполняемого файла (опционально)
set_target_properties(lab6_integration PROPERTIES OUTPUT_NAME "lab6")
