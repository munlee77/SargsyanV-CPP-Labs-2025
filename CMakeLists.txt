cmake_minimum_required(VERSION 3.20)

project(CaesarCipher)

# Установите стандарт C++14 для лучшей поддержки
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Флаги компилятора
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-parameter")

# Если нужен режим отладки
option(ENABLE_DEBUG "Enable debug mode" OFF)
if(ENABLE_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG")
endif()

# Список исходных файлов
set(SOURCES main.cpp)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES})

# Цель для форматирования кода
find_program(CLANG_FORMAT_EXE "clang-format")
if(CLANG_FORMAT_EXE)
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting source files..."
    )

    add_custom_target(
        check-format
        COMMAND ${CLANG_FORMAT_EXE} --dry-run -Werror -style=file ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Checking code format..."
    )
else()
    message(WARNING "clang-format not found. Formatting targets will not be available.")
endif()
