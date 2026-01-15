#include "caesar_cipher.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <limits>

namespace CaesarCipher {
namespace {

// Константы
constexpr size_t kMaxWordLength = 6;
constexpr size_t kBufferSize = 4096;
constexpr size_t kPageSize = 5;
constexpr int kASCII = 128;
constexpr size_t kMaxFileName = 256;

// Структура для статистики
struct SymbolStats {
    //символ
    char character;
    //код символа
    int ascii_code;
    //кол-во раз нахождения символа
    int count;
    //кол-во различных вариантов шифрования символа
    int encryption_variants;
    //минимальный код зашифровки символа
    int min_encrypted_code;
    //максимальный код зашифровки символа
    int max_encrypted_code;
    bool used_variants[kASCII] = {false};
};

// Структура для хранения аргументов
struct ProgramArguments {
    char input_file[kMaxFileName];
    char notebook_file[kMaxFileName];
    char encoded_file[kMaxFileName];
    char decoded_file[kMaxFileName];
};

// Динамический массив для ключей
struct KeyArray {
    int* data; //указатель на массив ключей
    size_t size; //текущее кол-во
    size_t capacity; //вместимость массива
};

// Создание динамического массива ключей
KeyArray createKeyArray(size_t initialCapacity = 100) {
    KeyArray arr;
    arr.data = new int[initialCapacity];
    arr.size = 0;
    arr.capacity = initialCapacity;
    return arr;
}

// Добавление ключа в конец динам. массива
void pushBackKey(KeyArray& arr, int key) {
    if (arr.size >= arr.capacity) {
        size_t newCapacity = arr.capacity * 2;
        int* newData = new int[newCapacity];
        std::memcpy(newData, arr.data, arr.size * sizeof(int));
        delete[] arr.data;
        arr.data = newData;
        arr.capacity = newCapacity;
    }
    arr.data[arr.size++] = key;
}

// Освобождение памяти массива ключей
void freeKeyArray(KeyArray& arr) {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = arr.capacity = 0;
}

// Динамический массив для использованных символов
struct UsedSymbolsArray {
    int* data;
    size_t size;
    size_t capacity;
};

// Создание динам. массива для использованных символов
UsedSymbolsArray createUsedSymbolsArray(size_t initialCapacity = 128) {
    UsedSymbolsArray arr;
    arr.data = new int[initialCapacity];
    arr.size = 0;
    arr.capacity = initialCapacity;
    return arr;
}

// Добавление индекса символа в массив used символов
void pushBackUsedSymbol(UsedSymbolsArray& arr, int symbol) {
    if (arr.size >= arr.capacity) {
        size_t newCapacity = arr.capacity * 2;
        int* newData = new int[newCapacity];
        std::memcpy(newData, arr.data, arr.size * sizeof(int));
        delete[] arr.data;
        arr.data = newData;
        arr.capacity = newCapacity;
    }
    arr.data[arr.size++] = symbol;
}

// Освобождение памяти массива used символов
void freeUsedSymbolsArray(UsedSymbolsArray& arr) {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = arr.capacity = 0;
}

// Проверка существования файла
bool fileExists(const char* filename) {
    std::ifstream file(filename);
    return file.good();
}

//Проверка принадлежности символа(буква/цифра) к слову
bool isWordCharacter(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

//Проверка символ = знак пунктуации?
bool isPunctuation(char c) {
    return std::ispunct(static_cast<unsigned char>(c)) != 0;
}

//Чтение ключей из код. блокнота
KeyArray readKeysFromNotebook(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        char errorMsg[512];
        std::strcpy(errorMsg, "Не удалось открыть файл блокнота: ");
        std::strcat(errorMsg, filename);
        throw std::runtime_error(errorMsg);
    }

    //массив для ключей
    KeyArray keys = createKeyArray(100);
    //буферы для чтения и временного хранения слова
    char buffer[kBufferSize];
    char current_word[kMaxWordLength + 1];
    size_t word_index = 0;
    bool in_word = false;

    //чтение файла
    while (file.read(buffer, kBufferSize) || file.gcount() > 0) {
        size_t bytes_read = static_cast<size_t>(file.gcount());
        //обработка каждого символа
        for (size_t i = 0; i < bytes_read; ++i) {
            char c = buffer[i];
            //если символ - часть слова
            if (isWordCharacter(c)) {
                if (!in_word) {
                    in_word = true;
                    word_index = 0;
                }

                if (word_index < kMaxWordLength) {
                    current_word[word_index++] = c;
                } else {
                    current_word[kMaxWordLength] = '\0';
                    int sum = 0;
                    for (size_t j = 0; j < kMaxWordLength; ++j) {
                        sum += static_cast<unsigned char>(current_word[j]);
                    }
                    pushBackKey(keys, sum % kASCII);

                    word_index = 1;
                    current_word[0] = c;
                }
            } else if (in_word && (std::isspace(static_cast<unsigned char>(c)) ||
                                  isPunctuation(c) || c == '\r' || c == '\n')) {
                current_word[word_index] = '\0';
                int sum = 0;
                //вычисление ключа
                for (size_t j = 0; j < word_index; ++j) {
                    sum += static_cast<unsigned char>(current_word[j]);
                }
                //добавление ключа в массив
                pushBackKey(keys, sum % kASCII);
                in_word = false;
            }
        }
    }
    //Если файл закончился, а слово не завершено
    if (in_word) {
        current_word[word_index] = '\0';
        int sum = 0;
        for (size_t j = 0; j < word_index; ++j) {
            sum += static_cast<unsigned char>(current_word[j]);
        }
        pushBackKey(keys, sum % kASCII);
    }

    if (keys.size == 0) {
        freeKeyArray(keys);
        throw std::runtime_error("Блокнот не содержит слов");
    }

    return keys;
}

//Кодирование файла и сбор статистики
void encodeFile(const char* input_file, const char* output_file,
                const KeyArray& keys, SymbolStats stats[]) {
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary | std::ios::trunc);
    //если входной файл НЕ открылся
    if (!in) {
        char errorMsg[512];
        std::strcpy(errorMsg, "Не удалось открыть входной файл: ");
        std::strcat(errorMsg, input_file);
        throw std::runtime_error(errorMsg);
    }
    //если выходной файл НЕ открылся
    if (!out) {
        char errorMsg[512];
        std::strcpy(errorMsg, "Не удалось создать выходной файл: ");
        std::strcat(errorMsg, output_file);
        throw std::runtime_error(errorMsg);
    }
    //буфер для чтения/записи
    char buffer[kBufferSize];
    //индекс ключа
    size_t key_index = 0;
    //общее кол-во обработ. символов (счетчик символов)
    size_t total_symbols = 0;

    //чтение входного файла
    while (in.read(buffer, kBufferSize) || in.gcount() > 0) {
        //кол-во прочитанных байт
        size_t bytes_read = static_cast<size_t>(in.gcount());

        //Обработка каждого символа в буфере
        for (size_t i = 0; i < bytes_read; ++i) {
            //исходный символ
            unsigned char original = static_cast<unsigned char>(buffer[i]);
            //тек. ключ
            int key = keys.data[key_index % keys.size];
            //индекс символа в массиве статистики
            int symbol_index = original % kASCII;
            //++счетчик встреч символа
            stats[symbol_index].count++;
            //сохранение символа
            stats[symbol_index].character = static_cast<char>(original);
            //сохранение ascii кода
            stats[symbol_index].ascii_code = original;

            //шифрование символа
            unsigned char encoded = (original + key) % kASCII;
            buffer[i] = static_cast<char>(encoded);

            //обновление статистики вариантов шифрования
            if (!stats[symbol_index].used_variants[encoded]) {
                stats[symbol_index].used_variants[encoded] = true;
                stats[symbol_index].encryption_variants++;

                //если это первый вар
                if (stats[symbol_index].encryption_variants == 1) {
                    stats[symbol_index].min_encrypted_code = encoded;
                    stats[symbol_index].max_encrypted_code = encoded;
                } else { //если их уже несколько
                    //обновление минимума
                    if (encoded < stats[symbol_index].min_encrypted_code) {
                        stats[symbol_index].min_encrypted_code = encoded;
                    }
                    //обновление максимума
                    if (encoded > stats[symbol_index].max_encrypted_code) {
                        stats[symbol_index].max_encrypted_code = encoded;
                    }
                }
            }

            key_index++;
            total_symbols++;
        }

        //Запись зашифрованных данных в файл
        out.write(buffer, bytes_read);
    }

    std::cout << "Файл закодирован. Символов обработано: " << total_symbols << std::endl;
}

//Декодирование файла
void decodeFile(const char* input_file, const char* output_file,
                const KeyArray& keys) {
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary | std::ios::trunc);

    //если входной файл НЕ открылся
    if (!in) {
        char errorMsg[512];
        std::strcpy(errorMsg, "Не удалось открыть закодированный файл: ");
        std::strcat(errorMsg, input_file);
        throw std::runtime_error(errorMsg);
    }
    //если выходной файл НЕ открылся
    if (!out) {
        char errorMsg[512];
        std::strcpy(errorMsg, "Не удалось создать расшифрованный файл: ");
        std::strcat(errorMsg, output_file);
        throw std::runtime_error(errorMsg);
    }
    //буфер для чтения/записи
    char buffer[kBufferSize];
    //индекс тек. ключа
    size_t key_index = 0;

    //Чтение закодированного файла
    while (in.read(buffer, kBufferSize) || in.gcount() > 0) {
        //кол-во прочитнных байт
        size_t bytes_read = static_cast<size_t>(in.gcount());
        //Обработка каждого символа в буфере
        for (size_t i = 0; i < bytes_read; ++i) {
            //закодированный символ
            unsigned char encoded = static_cast<unsigned char>(buffer[i]);
            //текущий ключ
            int key = keys.data[key_index % keys.size];
            //дешифровка символа
            unsigned char decoded = (encoded - key + kASCII) % kASCII;
            //замена символа в буфере
            buffer[i] = static_cast<char>(decoded);
            key_index++;
        }
        //запись расшифрованных данных в файл
        out.write(buffer, bytes_read);
    }

    std::cout << "Файл расшифрован." << std::endl;
}

//Сравнение двух файлов побайтово
bool compareFiles(const char* file1, const char* file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);

    if (!f1 || !f2) {
        return false;
    }

    char buf1[kBufferSize];
    char buf2[kBufferSize];

    while (f1.read(buf1, kBufferSize) || f1.gcount() > 0) {
        size_t count1 = static_cast<size_t>(f1.gcount());
        if (!f2.read(buf2, count1)) {
            return false;
        }

        if (std::memcmp(buf1, buf2, count1) != 0) {
            return false;
        }
    }

    return f2.peek() == EOF;
}

//Вывод заголовка таблицы статистики
void printHeader() {
    std::cout << std::left
              << std::setw(10) << "Symbol"
              << std::setw(12) << "ASCII Code"
              << std::setw(10) << "Count"
              << std::setw(18) << "Encrypt Variants"
              << std::setw(12) << "Min Code"
              << std::setw(12) << "Max Code"
              << std::endl;
    std::cout << std::string(74, '-') << std::endl;
}

void printStatsRow(const SymbolStats& stats) {
    char char_display[32];

    if (std::isprint(static_cast<unsigned char>(stats.character)) &&
        !std::isspace(static_cast<unsigned char>(stats.character))) {
        char_display[0] = stats.character;
        char_display[1] = '\0';
    } else {
        char code_str[16];
        std::sprintf(code_str, "%d", stats.ascii_code);
        std::strcpy(char_display, "[");
        std::strcat(char_display, code_str);
        std::strcat(char_display, "]");
    }

    std::cout << std::left
              << std::setw(10) << char_display
              << std::setw(12) << stats.ascii_code
              << std::setw(10) << stats.count
              << std::setw(18) << stats.encryption_variants
              << std::setw(12) << stats.min_encrypted_code
              << std::setw(12) << stats.max_encrypted_code
              << std::endl;
}

// Интерактивный вывод статистики с постраничной навигацией
void interactiveStatsDisplay(SymbolStats stats[], size_t notebook_size, size_t text_length) {
    UsedSymbolsArray used_symbols = createUsedSymbolsArray(128);

    for (int i = 0; i < kASCII; ++i) {
        if (stats[i].count > 0) {
            pushBackUsedSymbol(used_symbols, i);
        }
    }

    if (used_symbols.size == 0) {
        std::cout << "Нет статистики для отображения." << std::endl;
        freeUsedSymbolsArray(used_symbols);
        return;
    }

    size_t total_pages = (used_symbols.size + kPageSize - 1) / kPageSize;
    size_t current_page = 0;

    char command[256];

    while (true) {
        std::cout << "\n=== СТАТИСТИКА ШИФРОВАНИЯ ===" << std::endl;
        std::cout << "Размер блокнота: " << notebook_size << " слов" << std::endl;
        std::cout << "Длина текста: " << text_length << " символов" << std::endl;
        std::cout << "Страница " << (current_page + 1) << " из " << total_pages << std::endl;
        std::cout << std::endl;

        printHeader();

        size_t start = current_page * kPageSize;
        size_t end = (start + kPageSize < used_symbols.size) ? start + kPageSize : used_symbols.size;

        for (size_t i = start; i < end; ++i) {
            printStatsRow(stats[used_symbols.data[i]]);
        }

        std::cout << "\nКоманды:" << std::endl;
        std::cout << "  n - следующая страница" << std::endl;
        std::cout << "  p - предыдущая страница" << std::endl;
        std::cout << "  s - поиск по символу/коду" << std::endl;
        std::cout << "  e - выход из программы" << std::endl;
        std::cout << "  q - выход из статистики" << std::endl;
        std::cout << "\nВведите команду: ";

        std::cin.getline(command, 256);

        if (command[0] == 'n' || command[0] == 'N') {
            if (current_page < total_pages - 1) {
                current_page++;
            } else {
                std::cout << "Вы уже на последней странице." << std::endl;
            }
        } else if (command[0] == 'p' || command[0] == 'P') {
            if (current_page > 0) {
                current_page--;
            } else {
                std::cout << "Вы уже на первой странице." << std::endl;
            }
        } else if (command[0] == 's' || command[0] == 'S') {
            std::cout << "Введите символ или код ASCII (0-127): ";
            char input[256];
            std::cin.getline(input, 256);

            if (std::strlen(input) == 0) {
                continue;
            }

            int code = -1;
            if (std::strlen(input) == 1) {
                code = static_cast<unsigned char>(input[0]) % kASCII;
            } else {
                char* endptr;
                code = std::strtol(input, &endptr, 10);
                if (*endptr != '\0' || code < 0 || code >= kASCII) {
                    std::cout << "Код должен быть в диапазоне 0-127" << std::endl;
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
        } else if (command[0] == 'e' || command[0] == 'E') {
            std::cout << "Завершение программы." << std::endl;
            freeUsedSymbolsArray(used_symbols);
            std::exit(0);
        } else if (command[0] == 'q' || command[0] == 'Q') {
            break;
        } else {
            std::cout << "Неизвестная команда." << std::endl;
        }
    }

    freeUsedSymbolsArray(used_symbols);
}

//Парсинг (анализ) аргументов командной строки
void parseArguments(int argc, char* argv[], ProgramArguments& args) {
    if (argc == 5) {
        std::strncpy(args.input_file, argv[1], kMaxFileName - 1);
        args.input_file[kMaxFileName - 1] = '\0';

        std::strncpy(args.notebook_file, argv[2], kMaxFileName - 1);
        args.notebook_file[kMaxFileName - 1] = '\0';

        std::strncpy(args.encoded_file, argv[3], kMaxFileName - 1);
        args.encoded_file[kMaxFileName - 1] = '\0';

        std::strncpy(args.decoded_file, argv[4], kMaxFileName - 1);
        args.decoded_file[kMaxFileName - 1] = '\0';
    } else if (argc == 1) {
        throw std::runtime_error(
            "Использование: ./program <входной_файл> <блокнот> <закодированный> <расшифрованный>\n"
            "Или: ./program --input=<файл> --notebook=<файл> --encoded=<файл> --decoded=<файл>"
        );
    } else {
        for (int i = 1; i < argc; ++i) {
            if (std::strncmp(argv[i], "--input=", 8) == 0) {
                std::strncpy(args.input_file, argv[i] + 8, kMaxFileName - 1);
                args.input_file[kMaxFileName - 1] = '\0';
            } else if (std::strncmp(argv[i], "--notebook=", 11) == 0) {
                std::strncpy(args.notebook_file, argv[i] + 11, kMaxFileName - 1);
                args.notebook_file[kMaxFileName - 1] = '\0';
            } else if (std::strncmp(argv[i], "--encoded=", 10) == 0) {
                std::strncpy(args.encoded_file, argv[i] + 10, kMaxFileName - 1);
                args.encoded_file[kMaxFileName - 1] = '\0';
            } else if (std::strncmp(argv[i], "--decoded=", 10) == 0) {
                std::strncpy(args.decoded_file, argv[i] + 10, kMaxFileName - 1);
                args.decoded_file[kMaxFileName - 1] = '\0';
            }
        }

        if (std::strlen(args.input_file) == 0 || std::strlen(args.notebook_file) == 0 ||
            std::strlen(args.encoded_file) == 0 || std::strlen(args.decoded_file) == 0) {
            throw std::runtime_error(
                "Использование: ./program <входной_файл> <блокнот> <закодированный> <расшифрованный>\n"
                "Или: ./program --input=<файл> --notebook=<файл> --encoded=<файл> --decoded=<файл>"
            );
        }
    }
}

} // namespace

//Основная функция приложения
int runApplication(int argc, char* argv[]) {
    try {
        ProgramArguments args;
        parseArguments(argc, argv, args);

        if (!fileExists(args.input_file)) {
            char errorMsg[512];
            std::strcpy(errorMsg, "Входной файл не существует: ");
            std::strcat(errorMsg, args.input_file);
            throw std::runtime_error(errorMsg);
        }
        if (!fileExists(args.notebook_file)) {
            char errorMsg[512];
            std::strcpy(errorMsg, "Файл блокнота не существует: ");
            std::strcat(errorMsg, args.notebook_file);
            throw std::runtime_error(errorMsg);
        }

        std::cout << "Чтение ключей из блокнота..." << std::endl;
        KeyArray keys = readKeysFromNotebook(args.notebook_file);
        std::cout << "Прочитано " << keys.size << " ключей." << std::endl;

        SymbolStats stats[kASCII];
        for (int i = 0; i < kASCII; ++i) {
            stats[i].character = static_cast<char>(i);
            stats[i].ascii_code = i;
            stats[i].count = 0;
            stats[i].encryption_variants = 0;
            stats[i].min_encrypted_code = kASCII;
            stats[i].max_encrypted_code = -1;
        }

        std::cout << "Кодирование файла..." << std::endl;
        encodeFile(args.input_file, args.encoded_file, keys, stats);

        std::cout << "Декодирование файла..." << std::endl;
        decodeFile(args.encoded_file, args.decoded_file, keys);

        std::cout << "Проверка совпадения файлов..." << std::endl;
        if (compareFiles(args.input_file, args.decoded_file)) {
            std::cout << "✓ Файлы идентичны. Шифрование/дешифрование выполнено успешно." << std::endl;
        } else {
            std::cout << "✗ Ошибка: файлы отличаются!" << std::endl;
            freeKeyArray(keys);
            return 1;
        }

        std::cout << "\n=== РЕЗУЛЬТАТЫ ===" << std::endl;

        size_t text_length = 0;
        for (int i = 0; i < kASCII; ++i) {
            text_length += stats[i].count;
        }

        interactiveStatsDisplay(stats, keys.size, text_length);

        freeKeyArray(keys);
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
}

} // namespace CaesarCipher
