#include <iostream>
#include <termios.h>
#include <unistd.h>

const int kMaxText = 1000;
const int kMaxWords = 500;
const int kMaxWordLen = 100;

const char kBackspaceLinux = 127;
const char kBackspaceOther = 8;

bool IsValidChar(char c) {
    return (c >= 'a' && c <= 'z') || (c == ' ') || (c == '.');
}

void SetRawTerminal(struct termios *original_termios) {
    struct termios raw;

    tcgetattr(STDIN_FILENO, original_termios);
    raw = *original_termios;

    raw.c_lflag &= ~(ICANON | ECHO);

    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void RestoreTerminal(struct termios *original_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, original_termios);
}

void WriteString(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        write(STDOUT_FILENO, &str[i], 1);
        i++;
    }
}

void WriteChar(char c) {
    write(STDOUT_FILENO, &c, 1);
}

void WriteWord(const char* word) {
    int i = 0;
    while (word[i] != '\0') {
        WriteChar(word[i]);
        i++;
    }
}

int main() {
    char text[kMaxText + 1] = {0};
    char words[kMaxWords][kMaxWordLen + 1] = {0};

    struct termios original_termios;
    int text_length = 0;
    char c;
    bool has_dot = false;

    WriteString("Введите текст (только маленькие латинские буквы и пробелы, поставьте точку для завершения):\n");

    SetRawTerminal(&original_termios);

    while (true) {
        ssize_t bytes_read = read(STDIN_FILENO, &c, 1);

        if (bytes_read <= 0) {
            continue;
        }

        if (c == kBackspaceLinux || c == kBackspaceOther) {
            if (text_length > 0) {
                text_length--;

                if (text[text_length] == '.') {
                    has_dot = false;
                }

                WriteChar('\b');
                WriteChar(' ');
                WriteChar('\b');
            }
            continue;
        }

        if (IsValidChar(c)) {
            if (text_length >= kMaxText) {
                continue;
            }

            if (c == '.') {
                if (has_dot) {
                    continue;
                }

                has_dot = true;
            }

            text[text_length] = c;
            text_length++;
            WriteChar(c);

            if (c == '.') {
                break;
            }
        }
    }

    RestoreTerminal(&original_termios);
    WriteChar('\n');

    text[text_length] = '\0';


    bool has_letters = false;
    for (int i = 0; i < text_length; i++) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            has_letters = true;
            break;
        }
    }

    if (!has_letters) {
        WriteString("\nОшибка: Текст должен содержать хотя бы одну букву.\n");
        return 1;
    }

    if (text_length == 0 || text[text_length - 1] != '.') {
        WriteString("\nОшибка: Текст должен заканчиваться точкой.\n");
        return 1;
    }

    int word_count = 0;
    int word_index = 0;

    for (int i = 0; i < text_length; i++) {
        if (word_index >= kMaxWordLen) {
            WriteString("\nОшибка: Превышена максимальная длина слова.\n");
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
                    WriteString("\nОшибка: Превышено максимальное количество слов.\n");
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
        WriteString("\nОшибка: В тексте не найдено ни одного слова.\n");
        return 1;
    }

    // Находим слова, встречающиеся один раз
    WriteString("\nСлова, встречающиеся один раз:\n");
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

            WriteString(" ");
            if (unique_counter < 10) WriteString(" ");
            char num_str[10];
            int n = unique_counter;
            int idx = 0;

            do {
                num_str[idx++] = '0' + (n % 10);
                n /= 10;
            } while (n > 0);

            for (int j = idx - 1; j >= 0; j--) {
                WriteChar(num_str[j]);
            }
            WriteString(". ");

            WriteWord(words[i]);
            WriteChar('\n');
        }
    }

    if (!found_unique) {
        WriteString(" Таких слов не найдено.\n");
    }

    WriteChar('\n');
    return 0;
}
