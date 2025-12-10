#include <iostream>

// Константы для ограничений размеров
const int kMaxText = 1000;
const int kMaxWords = 500;
const int kMaxWordLen = 100;
// Константы для ASCII кодов
const int kMinPrintableAscii = 32;   // минимальный печатный символ ASCII
const int kMaxPrintableAscii = 126;  // максимальный печатный символ ASCII

// Функция для проверки допустимости символа
bool ValidChar(char symb) {
    return (symb >= 'a' && symb <= 'z') || (symb == ' ') || (symb == '.');
}

// Функция для очистки буфера ввода
void ClearInputBuffer() {
    while (std::cin.get() != '\n') {}
}

int main() {
    char text[kMaxText + 1] = {};  // инициализируем нулями
    int input_char = 0;  // используем int для get() чтобы избежать сужающего преобразования
    int text_length = 0;
    bool has_dot = false;

    std::cout << "Введите текст (только маленькие латинские буквы и пробелы, текст заканчивается точкой!):\n";

    // Чтение текста с проверкой ошибок
    while (true) {
        // Проверяем, не превышен ли максимальный размер текста
        if (text_length >= kMaxText) {
            std::cout << "Ошибка: Превышена максимальная длина текста (" << kMaxText << " символов).\n";
            std::cout << "Пожалуйста, введите более короткий текст.\n";
            return 1;
        }

        input_char = std::cin.get();  // get() возвращает int

        if (std::cin.eof()) {
            std::cout << "Ошибка: Неожиданный конец ввода. Текст должен заканчиваться точкой.\n";
            return 1;
        }

        // Если уже встретили точку и теперь получаем символ
        if (has_dot) {
            // Если это перевод строки - завершаем ввод
            if (input_char == '\n') {
                break;
            }
            // Если не перевод строки - ошибка
            else {
                std::cout << "Ошибка: После точки не должно быть символов.\n";
                std::cout << "Пожалуйста, введите текст заново: ";
                text_length = 0;
                has_dot = false;
                ClearInputBuffer();
                continue;
            }
        }

        // Проверяем допустимость символа
        if (!ValidChar(static_cast<char>(input_char))) {
            // Проверяем, является ли символ переводом строки
            if (input_char == '\n') {
                std::cout << "Ошибка: Текст должен заканчиваться точкой, а не переводом строки.\n";
                std::cout << "Пожалуйста, продолжайте ввод и завершите текст точкой.\n";
                std::cout << "Продолжите ввод: ";
                continue;
            }

            // Для печатных символов
            std::cout << "Ошибка ввода! Встречен недопустимый символ: ";

            if (input_char >= kMinPrintableAscii && input_char <= kMaxPrintableAscii) {
                std::cout << static_cast<char>(input_char);
            } else {
                std::cout << "(невидимый символ)";
            }
            std::cout << "\nРазрешены только: маленькие латинские буквы (a-z), пробел и точка.\n";
            ClearInputBuffer();
            std::cin.clear();
            std::cout << "Пожалуйста, введите текст заново: ";
            text_length = 0;
            has_dot = false;
            continue;
        }

        // Преобразуем int в char для сохранения в массив
        char symb = static_cast<char>(input_char);

        text[text_length] = symb;
        text_length++;

        if (symb == '.') {
            has_dot = true;

            if (text_length == 1) {
                std::cout << "Ошибка: Текст не может состоять только из точки.\n";
                std::cout << "Пожалуйста, введите текст с хотя бы одним словом: ";
                text_length = 0;
                has_dot = false;
                continue;
            }
            if (text_length > 1 && text[text_length - 2] == ' ') {
                std::cout << "Ошибка: Перед точкой не должно быть пробела.\n";
                std::cout << "Пожалуйста, введите текст заново: ";
                text_length = 0;
                has_dot = false;
                continue;
            }

            continue;
        }
    }

    text[text_length] = '\0';

    bool has_letters = false;
    for (int i = 0; i < text_length; i++) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            has_letters = true;
            break;
        }
    }

    if (!has_letters) {
        std::cout << "Ошибка: Текст должен содержать хотя бы одну букву.\n";
        return 1;
    }

    char words[kMaxWords][kMaxWordLen + 1] = {};  // инициализируем нулями
    int word_count = 0;
    int word_index = 0;

    for (int i = 0; i < text_length; i++) {
        if (word_index >= kMaxWordLen) {
            std::cout << "Ошибка: Превышена максимальная длина слова (" << kMaxWordLen << " символов).\n";
            return 1;
        }

        if (text[i] >= 'a' && text[i] <= 'z') {
            words[word_count][word_index] = text[i];
            word_index++;
        } else if (text[i] == ' ' || text[i] == '.') {
            if (word_index > 0) {
                words[word_count][word_index] = '\0';
                word_count++;
                if (word_count >= kMaxWords) {
                    std::cout << "Ошибка: Превышено максимальное количество слов (" << kMaxWords << ").\n";
                    return 1;
                }
                word_index = 0;
            }

            if (text[i] == '.') {
                break;
            }
        }
    }

    if (word_count == 0) {
        std::cout << "Ошибка: В тексте не найдено ни одного слова.\n";
        return 1;
    }

    std::cout << "\nСлова, встречающиеся один раз:\n";
    bool found_unique = false;
    int unique_counter = 0;

    for (int i = 0; i < word_count; i++) {
        int count = 0;

        for (int j = 0; j < word_count; j++) {
            bool equal = true;
            int k = 0;

            while (words[i][k] != '\0' && words[j][k] != '\0') {
                if (words[i][k] != words[j][k]) {
                    equal = false;
                    break;
                }
                k++;
            }

            if (words[i][k] != words[j][k]) {
                equal = false;
            }

            if (equal) {
                count++;
            }
        }

        if (count == 1) {
            found_unique = true;
            unique_counter++;
            std::cout << unique_counter << ". ";

            int k = 0;
            while (words[i][k] != '\0') {
                std::cout.put(words[i][k]);
                k++;
            }
            std::cout << std::endl;
        }
    }

    if (!found_unique) {
        std::cout << "Таких слов не найдено.\n";
    }

    return 0;
}

//запуск кода через cmake - ./homework
