#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
#include <map>

// Константы для программы
const int ASCII_SIZE = 128;           // Размер таблицы ASCII
const int MAX_WORD_LENGTH = 1024;     // Максимальная длина слова
const int BUFFER_SIZE = 4096;         // Размер буфера для чтения
const int PAGE_SIZE = 5;              // Количество строк на странице

// Структура для хранения данных о файлах
struct FileData {
    int* keys;                        // Динамический массив ключей
    int keysCount;                    // Количество ключей
    const char* inputFile;            // Исходный файл
    const char* notepadFile;          // Файл с кодовым блокнотом
    const char* encodedFile;          // Зашифрованный файл
    const char* decodedFile;          // Расшифрованный файл
};

// Структура для статистики символов
struct SymbolStats {
    char symbol;                      // Сам символ
    int asciiCode;                    // ASCII код символа
    int frequency;                    // Частота встречаемости
    int variantCount;                 // Количество вариантов шифрования
    std::vector<int> encryptedCodes;  // Коды зашифрованных вариантов
    int firstKeyIndex;                // Индекс первого использованного ключа
    std::string symbolName;           // Текстовое представление символа
};

// Вспомогательная функция для положительного остатка
int PositiveMod(int a, int b) {
    int result = a % b;
    if (result < 0) {
        result += b;
    }
    return result;
}

// Получение текстового представления символа
std::string GetSymbolName(int code) {
    if (code == 10) return "LF";      // Перевод строки
    if (code == 13) return "CR";      // Возврат каретки
    if (code == 9) return "TAB";      // Табуляция
    if (code == 32) return "SPACE";   // Пробел
    if (code < 32 || code == 127) return "CTRL"; // Управляющие символы

    char symbol = static_cast<char>(code);
    if (std::isprint(symbol)) {
        return std::string(1, symbol); // Печатаемые символы
    }

    return "?"; // Неизвестные символы
}

// Получение типа символа
std::string GetSymbolType(int code) {
    if (code >= 0 && code <= 31) return "Управляющий";
    if (code == 32) return "Пробел";
    if (code >= 48 && code <= 57) return "Цифра";
    if ((code >= 65 && code <= 90) || (code >= 97 && code <= 122)) return "Буква";
    return "Специальный";
}

// Сортировка статистики по частоте (от большего к меньшему)
bool CompareByFrequency(const SymbolStats& a, const SymbolStats& b) {
    return a.frequency > b.frequency;
}

// Сортировка статистики по ASCII коду (от меньшего к большему)
bool CompareByAscii(const SymbolStats& a, const SymbolStats& b) {
    return a.asciiCode < b.asciiCode;
}

// Установка имен файлов
void SetFilenames(FileData* data, const char* input, const char* notepad,
                  const char* encoded, const char* decoded) {
    data->inputFile = input;
    data->notepadFile = notepad;
    data->encodedFile = encoded;
    data->decodedFile = decoded;
    data->keys = nullptr;
    data->keysCount = 0;
}

// Подсчет слов в файле блокнота
int CountWordsInNotepad(FileData* data) {
    std::ifstream file(data->notepadFile, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Ошибка: не удалось открыть файл блокнота" << std::endl;
        return 0;
    }

    int wordCount = 0;
    char buffer[BUFFER_SIZE];
    bool inWord = false;

    while (file.getline(buffer, BUFFER_SIZE) || !file.eof()) {
        if (file.fail() && !file.eof()) {
            file.clear();
        }

        int len = 0;
        while (len < BUFFER_SIZE && buffer[len] != '\0') {
            len++;
        }

        for (int i = 0; i < len; i++) {
            char c = buffer[i];
            if (std::isalnum(static_cast<unsigned char>(c))) {
                if (!inWord) {
                    wordCount++;
                    inWord = true;
                }
            } else {
                inWord = false;
            }
        }

        inWord = false;
    }

    file.close();
    return wordCount;
}

// Генерация ключей из кодового блокнота
void GenerateKeys(FileData* data) {
    int wordCount = CountWordsInNotepad(data);

    if (wordCount == 0) {
        std::cout << "Ошибка: блокнот не содержит слов" << std::endl;
        exit(1);
    }

    // Выделяем память для ключей
    data->keys = new int[wordCount];
    if (data->keys == nullptr) {
        std::cout << "Ошибка: не удалось выделить память для ключей" << std::endl;
        exit(1);
    }

    std::ifstream file(data->notepadFile, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Ошибка: не удалось открыть файл блокнота" << std::endl;
        delete[] data->keys;
        data->keys = nullptr;
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    char word[MAX_WORD_LENGTH];
    int wordPos = 0;
    int keyIndex = 0;
    bool inWord = false;

    while (file.getline(buffer, BUFFER_SIZE) || !file.eof()) {
        if (file.fail() && !file.eof()) {
            file.clear();
        }

        int len = 0;
        while (len < BUFFER_SIZE && buffer[len] != '\0') {
            len++;
        }

        for (int i = 0; i < len; i++) {
            char c = buffer[i];

            if (std::isalnum(static_cast<unsigned char>(c))) {
                if (wordPos < MAX_WORD_LENGTH - 1) {
                    word[wordPos++] = c;
                } else {
                    // Если слово слишком длинное, завершаем его
                    word[wordPos] = '\0';

                    int sum = 0;
                    for (int j = 0; j < wordPos; j++) {
                        sum += static_cast<unsigned char>(word[j]);
                    }
                    data->keys[keyIndex++] = sum % ASCII_SIZE;

                    // Начинаем новое слово
                    wordPos = 0;
                    word[wordPos++] = c;
                }
                inWord = true;
            } else {
                if (inWord && wordPos > 0) {
                    word[wordPos] = '\0';

                    int sum = 0;
                    for (int j = 0; j < wordPos; j++) {
                        sum += static_cast<unsigned char>(word[j]);
                    }
                    data->keys[keyIndex++] = sum % ASCII_SIZE;

                    wordPos = 0;
                }
                inWord = false;
            }
        }

        if (inWord && wordPos > 0) {
            word[wordPos] = '\0';
            int sum = 0;
            for (int j = 0; j < wordPos; j++) {
                sum += static_cast<unsigned char>(word[j]);
            }
            data->keys[keyIndex++] = sum % ASCII_SIZE;
            wordPos = 0;
            inWord = false;
        }
    }

    file.close();
    data->keysCount = keyIndex;
    std::cout << "Сгенерировано ключей: " << data->keysCount << std::endl;
}

// Проверка существования входных файлов
bool ValidateInputFiles(FileData* data) {
    std::ifstream input(data->inputFile, std::ios::binary);
    if (!input.is_open()) {
        std::cout << "Ошибка: исходный файл не существует" << std::endl;
        return false;
    }
    input.close();

    std::ifstream notepad(data->notepadFile, std::ios::binary);
    if (!notepad.is_open()) {
        std::cout << "Ошибка: файл блокнота не существует" << std::endl;
        return false;
    }
    notepad.close();

    return true;
}

// Подготовка выходных файлов
void PrepareOutputFiles(FileData* data) {
    std::ofstream encoded(data->encodedFile, std::ios::binary | std::ios::trunc);
    if (!encoded.is_open()) {
        std::cout << "Ошибка: не удалось создать зашифрованный файл" << std::endl;
        exit(1);
    }
    encoded.close();

    std::ofstream decoded(data->decodedFile, std::ios::binary | std::ios::trunc);
    if (!decoded.is_open()) {
        std::cout << "Ошибка: не удалось создать расшифрованный файл" << std::endl;
        exit(1);
    }
    decoded.close();
}

// Шифрование файла
void EncryptFile(FileData* data) {
    if (data->keys == nullptr || data->keysCount == 0) {
        std::cout << "Ошибка: ключи не сгенерированы" << std::endl;
        return;
    }

    std::ifstream input(data->inputFile, std::ios::binary);
    if (!input.is_open()) {
        std::cout << "Ошибка: не удалось открыть исходный файл" << std::endl;
        return;
    }

    std::ofstream encoded(data->encodedFile, std::ios::binary | std::ios::app);
    if (!encoded.is_open()) {
        std::cout << "Ошибка: не удалось открыть файл для шифрования" << std::endl;
        input.close();
        return;
    }

    int charCount = 0;
    int keyIndex = 0;
    char ch;

    while (input.get(ch)) {
        int original = static_cast<unsigned char>(ch);

        // Проверяем, что символ в ASCII диапазоне
        if (original >= ASCII_SIZE) {
            std::cout << "Ошибка: файл содержит не-ASCII символы" << std::endl;
            input.close();
            encoded.close();
            exit(1);
        }

        int key = data->keys[keyIndex % data->keysCount];
        int encrypted = PositiveMod(original + key, ASCII_SIZE);

        encoded.put(static_cast<char>(encrypted));
        charCount++;
        keyIndex++;
    }

    input.close();
    encoded.close();
    std::cout << "Шифрование завершено. Символов: " << charCount << std::endl;
}

// Дешифрование файла
void DecryptFile(FileData* data) {
    if (data->keys == nullptr || data->keysCount == 0) {
        std::cout << "Ошибка: ключи не сгенерированы" << std::endl;
        return;
    }

    std::ifstream encoded(data->encodedFile, std::ios::binary);
    if (!encoded.is_open()) {
        std::cout << "Ошибка: не удалось открыть зашифрованный файл" << std::endl;
        return;
    }

    std::ofstream decoded(data->decodedFile, std::ios::binary | std::ios::app);
    if (!decoded.is_open()) {
        std::cout << "Ошибка: не удалось открыть файл для дешифрования" << std::endl;
        encoded.close();
        return;
    }

    int charCount = 0;
    int keyIndex = 0;
    char ch;

    while (encoded.get(ch)) {
        int encrypted = static_cast<unsigned char>(ch);
        int key = data->keys[keyIndex % data->keysCount];
        int decrypted = PositiveMod(encrypted - key, ASCII_SIZE);

        decoded.put(static_cast<char>(decrypted));
        charCount++;
        keyIndex++;
    }

    encoded.close();
    decoded.close();
    std::cout << "Дешифрование завершено. Символов: " << charCount << std::endl;
}

// Сравнение двух файлов
bool CompareFiles(FileData* data) {
    std::ifstream file1(data->inputFile, std::ios::binary);
    std::ifstream file2(data->decodedFile, std::ios::binary);

    if (!file1.is_open() || !file2.is_open()) {
        std::cout << "Ошибка: не удалось открыть файлы для сравнения" << std::endl;
        return false;
    }

    bool identical = true;
    char ch1, ch2;

    while (file1.get(ch1) && file2.get(ch2)) {
        if (ch1 != ch2) {
            identical = false;
            break;
        }
    }

    // Проверяем, что оба файла закончились одновременно
    if (file1.get(ch1) || file2.get(ch2)) {
        identical = false;
    }

    file1.close();
    file2.close();
    return identical;
}

// Подсчет общего количества символов в тексте
int CountTotalCharacters(FileData* data) {
    std::ifstream input(data->inputFile, std::ios::binary);
    if (!input.is_open()) {
        return 0;
    }

    int count = 0;
    char ch;
    while (input.get(ch)) {
        count++;
    }

    input.close();
    return count;
}

// Сбор детальной статистики
std::vector<SymbolStats> CollectDetailedStatistics(FileData* data) {
    std::vector<SymbolStats> stats;
    std::map<int, SymbolStats> statsMap; // Для быстрого поиска по ASCII коду

    // Открываем файлы для сбора статистики
    std::ifstream input(data->inputFile, std::ios::binary);
    std::ifstream encoded(data->encodedFile, std::ios::binary);

    if (!input.is_open() || !encoded.is_open()) {
        std::cout << "Ошибка: не удалось открыть файлы для сбора статистики" << std::endl;
        return stats;
    }

    int keyIndex = 0;
    char ch1, ch2;

    while (input.get(ch1) && encoded.get(ch2)) {
        int original = static_cast<unsigned char>(ch1);
        int encrypted = static_cast<unsigned char>(ch2);

        if (original < ASCII_SIZE) {
            // Ищем символ в мапе
            auto it = statsMap.find(original);
            if (it == statsMap.end()) {
                // Создаем новую запись
                SymbolStats stat;
                stat.symbol = static_cast<char>(original);
                stat.asciiCode = original;
                stat.frequency = 1;
                stat.variantCount = 1;
                stat.encryptedCodes.push_back(encrypted);
                stat.firstKeyIndex = keyIndex % data->keysCount;
                stat.symbolName = GetSymbolName(original);

                statsMap[original] = stat;
            } else {
                // Обновляем существующую запись
                it->second.frequency++;

                // Проверяем, есть ли уже такой зашифрованный код
                bool found = false;
                for (int code : it->second.encryptedCodes) {
                    if (code == encrypted) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    it->second.encryptedCodes.push_back(encrypted);
                    it->second.variantCount++;
                }
            }
        }
        keyIndex++;
    }

    input.close();
    encoded.close();

    // Преобразуем мапу в вектор
    for (auto& pair : statsMap) {
        stats.push_back(pair.second);
    }

    // Сортируем по частоте (от большего к меньшему)
    std::sort(stats.begin(), stats.end(), CompareByFrequency);

    return stats;
}

// Подсчет количества использованных ключей
int CountUsedKeys(const std::vector<SymbolStats>& stats) {
    std::map<int, bool> usedKeys;

    for (const auto& stat : stats) {
        usedKeys[stat.firstKeyIndex] = true;
    }

    return usedKeys.size();
}

// Вывод общей информации
void DisplayGeneralInfo(int totalChars, int notepadWords, int keysCount,
                       const std::vector<SymbolStats>& stats) {
    std::cout << "╔══════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     СТАТИСТИКА ШИФРОВАНИЯ                           ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════╝" << std::endl;

    std::cout << "\nОБЩАЯ ИНФОРМАЦИЯ:" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ Длина исходного текста:   " << std::setw(40) << std::left << totalChars << " символов │" << std::endl;
    std::cout << "│ Размер блокнота:          " << std::setw(40) << std::left << notepadWords << " слов │" << std::endl;
    std::cout << "│ Количество ключей:        " << std::setw(40) << std::left << keysCount << " │" << std::endl;
    std::cout << "│ Уникальных символов:      " << std::setw(40) << std::left << stats.size() << " │" << std::endl;
    std::cout << "│ Использовано ключей:      " << std::setw(40) << std::left << CountUsedKeys(stats) << " │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────────┘" << std::endl;
}

// Вывод подробной страницы статистики
void DisplayStatisticsPage(const std::vector<SymbolStats>& stats,
                          int page, int totalSymbols,
                          int totalChars, int notepadWords,
                          int keysCount) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    // Выводим общую информацию
    DisplayGeneralInfo(totalChars, notepadWords, keysCount, stats);

    std::cout << "\nТАБЛИЦА СТАТИСТИКИ ПО СИМВОЛАМ:" << std::endl;
    std::cout << "┌─────┬────────────┬─────────┬──────────┬──────────────┬────────────┬──────────────┐" << std::endl;
    std::cout << "│  №  │   Символ   │  ASCII  │ Частота  │   Варианты   │  Ключ №    │     Тип      │" << std::endl;
    std::cout << "├─────┼────────────┼─────────┼──────────┼──────────────┼────────────┼──────────────┤" << std::endl;

    // Вывод строк таблицы для текущей страницы
    int startIndex = page * PAGE_SIZE;
    int endIndex = std::min(startIndex + PAGE_SIZE, totalSymbols);

    for (int i = startIndex; i < endIndex; i++) {
        const SymbolStats& stat = stats[i];

        std::cout << "│ "
                  << std::setw(3) << std::right << (i + 1) << " │ "
                  << std::setw(10) << std::left << stat.symbolName << " │ "
                  << std::setw(7) << std::right << stat.asciiCode << " │ "
                  << std::setw(8) << std::right << stat.frequency << " │ "
                  << std::setw(12) << std::right << stat.variantCount << " │ "
                  << std::setw(10) << std::right << (stat.firstKeyIndex + 1) << " │ "
                  << std::setw(12) << std::left << GetSymbolType(stat.asciiCode) << " │" << std::endl;
    }

    std::cout << "└─────┴────────────┴─────────┴──────────┴──────────────┴────────────┴──────────────┘" << std::endl;

    // Вывод информации о странице
    int totalPages = (totalSymbols + PAGE_SIZE - 1) / PAGE_SIZE;
    std::cout << "\nСтраница " << (page + 1) << " из " << totalPages
              << " (показано " << (endIndex - startIndex) << " из " << totalSymbols << " символов)" << std::endl;
}

// Отображение детальной информации о символе
void DisplaySymbolDetails(const SymbolStats& stat, int totalChars) {
    std::cout << "\n══════════════════════════════════════════════════════════" << std::endl;
    std::cout << "ПОДРОБНАЯ ИНФОРМАЦИЯ О СИМВОЛЕ:" << std::endl;
    std::cout << "══════════════════════════════════════════════════════════" << std::endl;
    std::cout << "Символ:              " << stat.symbolName << std::endl;
    std::cout << "ASCII код:           " << stat.asciiCode << std::endl;
    std::cout << "Частота:             " << stat.frequency << " раз" << std::endl;

    if (totalChars > 0) {
        double percentage = (stat.frequency * 100.0) / totalChars;
        std::cout << "Процент от текста:   " << std::fixed << std::setprecision(2)
                  << percentage << "%" << std::endl;
    }

    std::cout << "Вариантов шифрования: " << stat.variantCount << std::endl;
    std::cout << "Первый ключ:          №" << (stat.firstKeyIndex + 1) << std::endl;
    std::cout << "Тип символа:          " << GetSymbolType(stat.asciiCode) << std::endl;

    // Вывод зашифрованных вариантов (первые 5)
    std::cout << "Зашифрованные варианты: ";
    int limit = std::min(5, static_cast<int>(stat.encryptedCodes.size()));
    for (int i = 0; i < limit; i++) {
        std::cout << GetSymbolName(stat.encryptedCodes[i])
                  << "(" << stat.encryptedCodes[i] << ")";
        if (i < limit - 1) std::cout << ", ";
    }
    if (stat.encryptedCodes.size() > 5) {
        std::cout << " ... и еще " << (stat.encryptedCodes.size() - 5);
    }
    std::cout << std::endl;

    std::cout << "══════════════════════════════════════════════════════════" << std::endl;
}

// Поиск символа в статистике
void SearchSymbol(const std::vector<SymbolStats>& stats, int totalChars) {
    std::cout << "\n╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║           ПОИСК СИМВОЛА                 ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;

    std::cout << "\nВведите символ, его название или ASCII код (0-127): ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) return;

    bool found = false;

    for (const auto& stat : stats) {
        // Проверка по ASCII коду
        if (std::to_string(stat.asciiCode) == input) {
            DisplaySymbolDetails(stat, totalChars);
            found = true;
            break;
        }

        // Проверка по названию символа
        std::string lowerName = stat.symbolName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        std::string lowerInput = input;
        std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

        if (lowerName == lowerInput || lowerName.find(lowerInput) != std::string::npos) {
            DisplaySymbolDetails(stat, totalChars);
            found = true;
            break;
        }

        // Проверка по самому символу (если ввод - один символ)
        if (input.length() == 1) {
            int inputCode = static_cast<unsigned char>(input[0]);
            if (inputCode == stat.asciiCode) {
                DisplaySymbolDetails(stat, totalChars);
                found = true;
                break;
            }
        }
    }

    if (!found) {
        std::cout << "\nСимвол '" << input << "' не найден в тексте." << std::endl;
    }

    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.get();
}

// Показать дополнительную информацию
void ShowAdditionalInfo(const std::vector<SymbolStats>& stats, int totalChars,
                       int notepadWords, int keysCount) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    std::cout << "╔══════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  ДОПОЛНИТЕЛЬНАЯ ИНФОРМАЦИЯ                          ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════╝" << std::endl;

    // Подсчет статистики по типам символов
    int controlChars = 0, digits = 0, letters = 0, spaces = 0, specialChars = 0;
    int controlFreq = 0, digitsFreq = 0, lettersFreq = 0, spacesFreq = 0, specialFreq = 0;

    for (const auto& stat : stats) {
        std::string type = GetSymbolType(stat.asciiCode);
        if (type == "Управляющий") {
            controlChars++;
            controlFreq += stat.frequency;
        } else if (type == "Пробел") {
            spaces++;
            spacesFreq += stat.frequency;
        } else if (type == "Цифра") {
            digits++;
            digitsFreq += stat.frequency;
        } else if (type == "Буква") {
            letters++;
            lettersFreq += stat.frequency;
        } else {
            specialChars++;
            specialFreq += stat.frequency;
        }
    }

    std::cout << "\nРАСПРЕДЕЛЕНИЕ СИМВОЛОВ ПО ТИПАМ:" << std::endl;
    std::cout << "┌────────────────────────────────┬─────────┬─────────────┬─────────────┐" << std::endl;
    std::cout << "│           Тип символа          │ Кол-во  │   Частота   │   Процент   │" << std::endl;
    std::cout << "├────────────────────────────────┼─────────┼─────────────┼─────────────┤" << std::endl;

    // Вспомогательная функция для вывода строки таблицы
    auto PrintTypeRow = [totalChars](const std::string& type, int count, int freq) {
        double freqPercent = (totalChars > 0) ? (freq * 100.0 / totalChars) : 0;

        std::cout << "│ " << std::setw(30) << std::left << type << " │ "
                  << std::setw(7) << std::right << count << " │ "
                  << std::setw(11) << std::right << freq << " │ "
                  << std::setw(11) << std::right << std::fixed << std::setprecision(2)
                  << freqPercent << "% │" << std::endl;
    };

    PrintTypeRow("Буквы", letters, lettersFreq);
    PrintTypeRow("Цифры", digits, digitsFreq);
    PrintTypeRow("Пробелы", spaces, spacesFreq);
    PrintTypeRow("Специальные символы", specialChars, specialFreq);
    PrintTypeRow("Управляющие символы", controlChars, controlFreq);

    std::cout << "└────────────────────────────────┴─────────┴─────────────┴─────────────┘" << std::endl;

    // Топ-5 самых частых символов
    std::cout << "\nТОП-5 САМЫХ ЧАСТЫХ СИМВОЛОВ:" << std::endl;
    std::cout << "┌─────┬────────────┬─────────┬──────────┬──────────────┬────────────┐" << std::endl;
    std::cout << "│  №  │   Символ   │  ASCII  │ Частота  │   Процент    │    Тип     │" << std::endl;
    std::cout << "├─────┼────────────┼─────────┼──────────┼──────────────┼────────────┤" << std::endl;

    int limit = std::min(5, static_cast<int>(stats.size()));
    for (int i = 0; i < limit; i++) {
        const SymbolStats& stat = stats[i];
        double percentage = totalChars > 0 ? (stat.frequency * 100.0 / totalChars) : 0;

        std::cout << "│ "
                  << std::setw(3) << std::right << (i + 1) << " │ "
                  << std::setw(10) << std::left << stat.symbolName << " │ "
                  << std::setw(7) << std::right << stat.asciiCode << " │ "
                  << std::setw(8) << std::right << stat.frequency << " │ "
                  << std::setw(12) << std::right << std::fixed << std::setprecision(2)
                  << percentage << "% │ "
                  << std::setw(10) << std::left << GetSymbolType(stat.asciiCode) << " │" << std::endl;
    }
    std::cout << "└─────┴────────────┴─────────┴──────────┴──────────────┴────────────┘" << std::endl;

    // Статистика по вариантам шифрования
    int maxVariants = 0;
    int minVariants = 1000;
    int avgVariants = 0;

    for (const auto& stat : stats) {
        if (stat.variantCount > maxVariants) maxVariants = stat.variantCount;
        if (stat.variantCount < minVariants) minVariants = stat.variantCount;
        avgVariants += stat.variantCount;
    }

    if (!stats.empty()) {
        avgVariants /= stats.size();
    }

    std::cout << "\nСТАТИСТИКА ПО ВАРИАНТАМ ШИФРОВАНИЯ:" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ Максимальное количество вариантов: " << std::setw(32) << maxVariants << " │" << std::endl;
    std::cout << "│ Минимальное количество вариантов:  " << std::setw(32) << minVariants << " │" << std::endl;
    std::cout << "│ Среднее количество вариантов:      " << std::setw(32)
              << std::fixed << std::setprecision(2) << avgVariants << " │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────────┘" << std::endl;

    std::cout << "\nНажмите Enter для возврата...";
    std::cin.get();
}

// Отображение статистики с постраничным выводом
void ShowDetailedStatistics(FileData* data) {
    // Собираем детальную статистику
    std::vector<SymbolStats> stats = CollectDetailedStatistics(data);

    if (stats.empty()) {
        std::cout << "Нет данных для отображения" << std::endl;
        return;
    }

    // Подсчитываем общее количество символов
    int totalChars = CountTotalCharacters(data);

    // Получаем количество слов в блокноте
    int notepadWords = CountWordsInNotepad(data);

    int totalSymbols = stats.size();

    // Постраничный вывод
    int currentPage = 0;
    int totalPages = (totalSymbols + PAGE_SIZE - 1) / PAGE_SIZE;
    char command;
    bool sortedByFrequency = true;

    do {
        DisplayStatisticsPage(stats, currentPage, totalSymbols,
                             totalChars, notepadWords, data->keysCount);

        std::cout << "\nКОМАНДЫ:" << std::endl;
        std::cout << "  n - следующая страница        p - предыдущая страница" << std::endl;
        std::cout << "  f - первая страница           l - последняя страница" << std::endl;
        std::cout << "  s - поиск символа             i - дополнительная информация" << std::endl;
        std::cout << "  o - сортировать по частоте    a - сортировать по ASCII коду" << std::endl;
        std::cout << "  q - выход из статистики" << std::endl;
        std::cout << "\nВведите команду: ";
        std::cin >> command;
        std::cin.ignore(); // Очищаем буфер ввода

        if (command == 'n' || command == 'N') {
            if (currentPage < totalPages - 1) {
                currentPage++;
            } else {
                std::cout << "\nЭто последняя страница. Нажмите Enter...";
                std::cin.get();
            }
        } else if (command == 'p' || command == 'P') {
            if (currentPage > 0) {
                currentPage--;
            } else {
                std::cout << "\nЭто первая страница. Нажмите Enter...";
                std::cin.get();
            }
        } else if (command == 'f' || command == 'F') {
            currentPage = 0;
        } else if (command == 'l' || command == 'L') {
            currentPage = totalPages - 1;
        } else if (command == 's' || command == 'S') {
            SearchSymbol(stats, totalChars);
        } else if (command == 'i' || command == 'I') {
            ShowAdditionalInfo(stats, totalChars, notepadWords, data->keysCount);
        } else if (command == 'o' || command == 'O') {
            if (!sortedByFrequency) {
                std::sort(stats.begin(), stats.end(), CompareByFrequency);
                sortedByFrequency = true;
                currentPage = 0;
                std::cout << "Сортировка по частоте применена. Нажмите Enter...";
                std::cin.get();
            }
        } else if (command == 'a' || command == 'A') {
            if (sortedByFrequency) {
                std::sort(stats.begin(), stats.end(), CompareByAscii);
                sortedByFrequency = false;
                currentPage = 0;
                std::cout << "Сортировка по ASCII коду применена. Нажмите Enter...";
                std::cin.get();
            }
        } else if (command != 'q' && command != 'Q') {
            std::cout << "\nНеизвестная команда. Нажмите Enter...";
            std::cin.get();
        }

    } while (command != 'q' && command != 'Q');
}

// Основная функция обработки
void ProcessFiles(FileData* data) {
    // Проверка файлов
    if (!ValidateInputFiles(data)) {
        return;
    }

    // Подготовка выходных файлов
    PrepareOutputFiles(data);

    // Генерация ключей
    GenerateKeys(data);

    // Шифрование
    std::cout << "\nНачинаем шифрование..." << std::endl;
    EncryptFile(data);

    // Дешифрование
    std::cout << "\nНачинаем дешифрование..." << std::endl;
    DecryptFile(data);

    // Проверка совпадения файлов
    std::cout << "\nПроверяем совпадение файлов..." << std::endl;
    if (CompareFiles(data)) {
        std::cout << "✓ Файлы совпадают! Шифрование работает корректно." << std::endl;
    } else {
        std::cout << "⚠ Внимание! Файлы не совпадают! Проверьте алгоритм шифрования." << std::endl;
    }

    // Вывод статистики
    std::cout << "\nДля просмотра подробной статистики нажмите Enter...";
    std::cin.get();

    ShowDetailedStatistics(data);

    // Освобождение памяти
    if (data->keys != nullptr) {
        delete[] data->keys;
        data->keys = nullptr;
    }
}

// Главная функция
int main(int argc, char* argv[]) {
    // Проверка аргументов командной строки
    if (argc != 5) {
        std::cout << "Использование: " << argv[0]
                  << " <исходный_файл> <файл_блокнота> <зашифрованный_файл> <расшифрованный_файл>" << std::endl;
        std::cout << "\nПример:" << std::endl;
        std::cout << "  " << argv[0] << " input.txt notepad.txt encoded.txt decoded.txt" << std::endl;
        return 1;
    }

    // Создаем структуру для хранения данных
    FileData fileData;

    // Устанавливаем имена файлов
    SetFilenames(&fileData, argv[1], argv[2], argv[3], argv[4]);

    // Обрабатываем файлы
    ProcessFiles(&fileData);

    std::cout << "\nПрограмма завершена. Результаты сохранены в файлы:" << std::endl;
    std::cout << "  Зашифрованный: " << argv[3] << std::endl;
    std::cout << "  Расшифрованный: " << argv[4] << std::endl;

    return 0;
}
