#include "caesar_cipher.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <limits>

namespace CaesarCipher {
namespace {

// Константы
constexpr size_t MAX_WORD_LENGTH = 1024;
constexpr size_t BUFFER_SIZE = 4096;
constexpr size_t PAGE_SIZE = 5;
constexpr int ASCII_MOD = 128;

// Структура для статистики
struct SymbolStats {
    char character;
    int ascii_code;
    int count;
    int encryption_variants;
    int min_encrypted_code;
    int max_encrypted_code;
    bool used_variants[ASCII_MOD] = {false};
};

// Структура для хранения аргументов
struct ProgramArguments {
    std::string input_file;
    std::string notebook_file;
    std::string encoded_file;
    std::string decoded_file;
};

// Вспомогательные функции
[[nodiscard]] bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

[[nodiscard]] bool isWordCharacter(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

[[nodiscard]] bool isPunctuation(char c) {
    return std::ispunct(static_cast<unsigned char>(c)) != 0;
}

[[nodiscard]] std::vector<int> readKeysFromNotebook(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл блокнота: " + filename);
    }

    std::vector<int> keys;
    char buffer[BUFFER_SIZE];
    char current_word[MAX_WORD_LENGTH + 1];
    size_t word_index = 0;
    bool in_word = false;

    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        size_t bytes_read = static_cast<size_t>(file.gcount());

        for (size_t i = 0; i < bytes_read; ++i) {
            char c = buffer[i];

            if (isWordCharacter(c)) {
                if (!in_word) {
                    in_word = true;
                    word_index = 0;
                }

                if (word_index < MAX_WORD_LENGTH) {
                    current_word[word_index++] = c;
                } else {
                    // Если слово превысило максимальную длину, завершаем его
                    current_word[MAX_WORD_LENGTH] = '\0';
                    int sum = 0;
                    for (size_t j = 0; j < MAX_WORD_LENGTH; ++j) {
                        sum += static_cast<unsigned char>(current_word[j]);
                    }
                    keys.push_back(sum % ASCII_MOD);

                    // Начинаем новое слово с текущего символа
                    word_index = 1;
                    current_word[0] = c;
                }
            } else if (in_word && (std::isspace(static_cast<unsigned char>(c)) ||
                                  isPunctuation(c) || c == '\r' || c == '\n')) {
                // Завершаем слово
                current_word[word_index] = '\0';
                int sum = 0;
                for (size_t j = 0; j < word_index; ++j) {
                    sum += static_cast<unsigned char>(current_word[j]);
                }
                keys.push_back(sum % ASCII_MOD);
                in_word = false;
            }
        }
    }

    // Обработка последнего слова
    if (in_word) {
        current_word[word_index] = '\0';
        int sum = 0;
        for (size_t j = 0; j < word_index; ++j) {
            sum += static_cast<unsigned char>(current_word[j]);
        }
        keys.push_back(sum % ASCII_MOD);
    }

    if (keys.empty()) {
        throw std::runtime_error("Блокнот не содержит слов");
    }

    return keys;
}

void encodeFile(const std::string& input_file, const std::string& output_file,
                const std::vector<int>& keys, SymbolStats stats[]) {
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary | std::ios::trunc);

    if (!in) {
        throw std::runtime_error("Не удалось открыть входной файл: " + input_file);
    }
    if (!out) {
        throw std::runtime_error("Не удалось создать выходной файл: " + output_file);
    }

    char buffer[BUFFER_SIZE];
    size_t key_index = 0;
    size_t total_symbols = 0;

    while (in.read(buffer, BUFFER_SIZE) || in.gcount() > 0) {
        size_t bytes_read = static_cast<size_t>(in.gcount());

        for (size_t i = 0; i < bytes_read; ++i) {
            unsigned char original = static_cast<unsigned char>(buffer[i]);
            int key = keys[key_index % keys.size()];

            // Обновляем статистику
            int symbol_index = original % ASCII_MOD;
            stats[symbol_index].count++;
            stats[symbol_index].character = static_cast<char>(original);
            stats[symbol_index].ascii_code = original;

            // Шифруем символ
            unsigned char encoded = (original + key) % ASCII_MOD;
            buffer[i] = static_cast<char>(encoded);

            // Обновляем информацию о вариантах шифрования
            if (!stats[symbol_index].used_variants[encoded]) {
                stats[symbol_index].used_variants[encoded] = true;
                stats[symbol_index].encryption_variants++;

                if (encoded < stats[symbol_index].min_encrypted_code) {
                    stats[symbol_index].min_encrypted_code = encoded;
                }
                if (encoded > stats[symbol_index].max_encrypted_code) {
                    stats[symbol_index].max_encrypted_code = encoded;
                }
            }

            key_index++;
            total_symbols++;
        }

        out.write(buffer, bytes_read);
    }

    std::cout << "Файл закодирован. Символов обработано: " << total_symbols << std::endl;
}

void decodeFile(const std::string& input_file, const std::string& output_file,
                const std::vector<int>& keys) {
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary | std::ios::trunc);

    if (!in) {
        throw std::runtime_error("Не удалось открыть закодированный файл: " + input_file);
    }
    if (!out) {
        throw std::runtime_error("Не удалось создать расшифрованный файл: " + output_file);
    }

    char buffer[BUFFER_SIZE];
    size_t key_index = 0;

    while (in.read(buffer, BUFFER_SIZE) || in.gcount() > 0) {
        size_t bytes_read = static_cast<size_t>(in.gcount());

        for (size_t i = 0; i < bytes_read; ++i) {
            unsigned char encoded = static_cast<unsigned char>(buffer[i]);
            int key = keys[key_index % keys.size()];

            // Дешифруем символ
            unsigned char decoded = (encoded - key + ASCII_MOD) % ASCII_MOD;
            buffer[i] = static_cast<char>(decoded);
            key_index++;
        }

        out.write(buffer, bytes_read);
    }

    std::cout << "Файл расшифрован." << std::endl;
}

[[nodiscard]] bool compareFiles(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);

    if (!f1 || !f2) {
        return false;
    }

    char buf1[BUFFER_SIZE];
    char buf2[BUFFER_SIZE];

    while (f1.read(buf1, BUFFER_SIZE) || f1.gcount() > 0) {
        size_t count1 = static_cast<size_t>(f1.gcount());
        if (!f2.read(buf2, count1)) {
            return false;
        }

        if (std::memcmp(buf1, buf2, count1) != 0) {
            return false;
        }
    }

    // Проверяем, что второй файл тоже закончился
    return f2.peek() == EOF;
}

void printHeader() {
    std::cout << std::left
              << std::setw(12) << "Symbol"
              << std::setw(12) << "ASCII Code"
              << std::setw(12) << "Count"
              << std::setw(20) << "Encrypt Variants"
              << std::setw(12) << "Min Code"
              << std::setw(12) << "Max Code"
              << std::endl;
    std::cout << std::string(80, '-') << std::endl;
}

void printStatsRow(const SymbolStats& stats) {
    std::string char_display;
    if (std::isprint(static_cast<unsigned char>(stats.character)) &&
        !std::isspace(static_cast<unsigned char>(stats.character))) {
        char_display = std::string(1, stats.character);
    } else {
        char_display = "[" + std::to_string(stats.ascii_code) + "]";
    }

    std::cout << std::left
              << std::setw(12) << char_display
              << std::setw(12) << stats.ascii_code
              << std::setw(12) << stats.count
              << std::setw(20) << stats.encryption_variants
              << std::setw(12) << stats.min_encrypted_code
              << std::setw(12) << stats.max_encrypted_code
              << std::endl;
}

void interactiveStatsDisplay(SymbolStats stats[], size_t notebook_size, size_t text_length) {
    // Собираем символы, которые встречались
    std::vector<int> used_symbols;
    for (int i = 0; i < ASCII_MOD; ++i) {
        if (stats[i].count > 0) {
            used_symbols.push_back(i);
        }
    }

    if (used_symbols.empty()) {
        std::cout << "Нет статистики для отображения." << std::endl;
        return;
    }

    size_t total_pages = (used_symbols.size() + PAGE_SIZE - 1) / PAGE_SIZE;
    size_t current_page = 0;

    while (true) {
        std::cout << "\n=== СТАТИСТИКА ШИФРОВАНИЯ ===" << std::endl;
        std::cout << "Размер блокнота: " << notebook_size << " слов" << std::endl;
        std::cout << "Длина текста: " << text_length << " символов" << std::endl;
        std::cout << "Страница " << (current_page + 1) << " из " << total_pages << std::endl;
        std::cout << std::endl;

        printHeader();

        size_t start = current_page * PAGE_SIZE;
        size_t end = std::min(start + PAGE_SIZE, used_symbols.size());

        for (size_t i = start; i < end; ++i) {
            printStatsRow(stats[used_symbols[i]]);
        }

        std::cout << "\nКоманды:" << std::endl;
        std::cout << "  n - следующая страница" << std::endl;
        std::cout << "  p - предыдущая страница" << std::endl;
        std::cout << "  s - поиск по символу/коду" << std::endl;
        std::cout << "  e - выход из программы" << std::endl;
        std::cout << "  q - выход из статистики" << std::endl;
        std::cout << "\nВведите команду: ";

        std::string command;
        std::getline(std::cin, command);

        if (command == "n" || command == "N") {
            if (current_page < total_pages - 1) {
                current_page++;
            } else {
                std::cout << "Вы уже на последней странице." << std::endl;
            }
        } else if (command == "p" || command == "P") {
            if (current_page > 0) {
                current_page--;
            } else {
                std::cout << "Вы уже на первой странице." << std::endl;
            }
        } else if (command == "s" || command == "S") {
            std::cout << "Введите символ или код ASCII (0-127): ";
            std::string input;
            std::getline(std::cin, input);

            if (input.empty()) {
                continue;
            }

            int code = -1;
            if (input.length() == 1) {
                code = static_cast<unsigned char>(input[0]) % ASCII_MOD;
            } else {
                try {
                    code = std::stoi(input);
                    if (code < 0 || code >= ASCII_MOD) {
                        std::cout << "Код должен быть в диапазоне 0-127" << std::endl;
                        continue;
                    }
                } catch (...) {
                    std::cout << "Неверный ввод" << std::endl;
                    continue;
                }
            }

            if (stats[code].count > 0) {
                std::cout << "\nРезультат поиска:" << std::endl;
                printHeader();
                printStatsRow(stats[code]);
                std::cout << "\nНажмите Enter для продолжения...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::cout << "Символ с кодом " << code << " не встречался в тексте." << std::endl;
            }
        } else if (command == "e" || command == "E") {
            std::cout << "Завершение программы." << std::endl;
            std::exit(0);
        } else if (command == "q" || command == "Q") {
            break;
        } else {
            std::cout << "Неизвестная команда." << std::endl;
        }
    }
}

[[nodiscard]] ProgramArguments parseArguments(int argc, char* argv[]) {
    ProgramArguments args;

    if (argc == 5) {
        // Позиционные аргументы
        args.input_file = argv[1];
        args.notebook_file = argv[2];
        args.encoded_file = argv[3];
        args.decoded_file = argv[4];
    } else {
        // Попытка парсинга именованных аргументов
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg.substr(0, 2) == "--") {
                size_t eq_pos = arg.find('=');
                if (eq_pos != std::string::npos) {
                    std::string name = arg.substr(2, eq_pos - 2);
                    std::string value = arg.substr(eq_pos + 1);

                    if (name == "input") {
                        args.input_file = value;
                    } else if (name == "notebook") {
                        args.notebook_file = value;
                    } else if (name == "encoded") {
                        args.encoded_file = value;
                    } else if (name == "decoded") {
                        args.decoded_file = value;
                    }
                }
            }
        }

        // Проверка, что все аргументы заданы
        if (args.input_file.empty() || args.notebook_file.empty() ||
            args.encoded_file.empty() || args.decoded_file.empty()) {
            throw std::runtime_error(
                "Использование: ./program <входной_файл> <блокнот> <закодированный> <расшифрованный>\n"
                "Или: ./program --input=<файл> --notebook=<файл> --encoded=<файл> --decoded=<файл>"
            );
        }
    }

    return args;
}

} // namespace

int runApplication(int argc, char* argv[]) {
    try {
        // Парсинг аргументов
        ProgramArguments args = parseArguments(argc, argv);

        // Проверка существования входных файлов
        if (!fileExists(args.input_file)) {
            throw std::runtime_error("Входной файл не существует: " + args.input_file);
        }
        if (!fileExists(args.notebook_file)) {
            throw std::runtime_error("Файл блокнота не существует: " + args.notebook_file);
        }

        // Чтение ключей из блокнота
        std::cout << "Чтение ключей из блокнота..." << std::endl;
        std::vector<int> keys = readKeysFromNotebook(args.notebook_file);
        std::cout << "Прочитано " << keys.size() << " ключей." << std::endl;

        // Инициализация статистики
        SymbolStats stats[ASCII_MOD];
        for (int i = 0; i < ASCII_MOD; ++i) {
            stats[i].character = static_cast<char>(i);
            stats[i].ascii_code = i;
            stats[i].count = 0;
            stats[i].encryption_variants = 0;
            stats[i].min_encrypted_code = ASCII_MOD;
            stats[i].max_encrypted_code = -1;
        }

        // Кодирование файла
        std::cout << "Кодирование файла..." << std::endl;
        encodeFile(args.input_file, args.encoded_file, keys, stats);

        // Декодирование файла
        std::cout << "Декодирование файла..." << std::endl;
        decodeFile(args.encoded_file, args.decoded_file, keys);

        // Проверка совпадения файлов
        std::cout << "Проверка совпадения файлов..." << std::endl;
        if (compareFiles(args.input_file, args.decoded_file)) {
            std::cout << " Файлы идентичны. Шифрование/дешифрование выполнено успешно." << std::endl;
        } else {
            std::cout << " Ошибка: файлы отличаются!" << std::endl;
            return 1;
        }

        // Вывод статистики
        std::cout << "\n=== РЕЗУЛЬТАТЫ ===" << std::endl;

        // Вычисление длины текста
        size_t text_length = 0;
        for (int i = 0; i < ASCII_MOD; ++i) {
            text_length += stats[i].count;
        }

        // Интерактивный вывод статистики
        interactiveStatsDisplay(stats, keys.size(), text_length);

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
}

} // namespace CaesarCipher
