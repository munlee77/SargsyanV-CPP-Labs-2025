#include <cmath>     // Для математических функций (pow, atan, fabs)
#include <iomanip>   // Для форматирования вывода (setw, setprecision)
#include <iostream>  // Для ввода-вывода (cin, cout)

using namespace std;  // Используем стандартное пространство имен

const int kDivider_5 = 5;
const int kStart_2 = 2;
const int kEnd_8 = 8;
const int kStart_3 = 3;
const int kEnd_9 = 9;
const int kWidth_8 = 8;
const int kWidth_12 = 12;
const int kPrecision_6 = 6;
const int kPrecision_1 = 1;
const double kEps = 1e-9;
const double kNum_2 = 2.0;
const double kNum_3 = 3.0;

// Прототипы функций - объявляем функции до их использования
void task1();         // Функция для задания 1
void task2();         // Функция для задания 2
void task3();         // Функция для задания 3
int task4();          // Функция для задания 4
bool continueWork();  // Функция для проверки продолжения работы

int main() {
    // Устанавливаем русскую локаль для корректного отображения кириллицы
    setlocale(LC_ALL, "Russian");

    char choice = 0;      // Переменная для выбора пункта меню
    bool running = true;  // Флаг продолжения работы программы

    // Главный цикл программы
    while (running) {
        // Выводим меню с доступными заданиями
        cout << "=== ЛАБОРАТОРНАЯ РАБОТА - ВАРИАНТ 10 ===" << endl;
        cout << "1. Задание 1 - Сумма чисел, делящихся на 5 и не делящихся на m" << endl;
        cout << "2. Задание 2 - Вычисление функции S" << endl;
        cout << "3. Задание 3 - Вычисление суммы ряда и функции Y(x)" << endl;
        cout << "4. Задание 4 - Вычисление y по формуле с частичными суммами" << endl;
        cout << "0. Выход" << endl;
        cout << "Выберите задание: ";
        cin >> choice;  // Считываем выбор пользователя

        // Обрабатываем выбор пользователя с помощью switch
        switch (choice) {
            case '1':
                task1();  // Вызываем функцию для задания 1
                break;
            case '2':
                task2();  // Вызываем функцию для задания 2
                break;
            case '3':
                task3();  // Вызываем функцию для задания 3
                break;
            case '4':
                task4();  // Вызываем функцию для задания 4
                break;
            case '0':
                running = false;  // Устанавливаем флаг выхода
                cout << "Выход из программы." << endl;
                break;
            default:
                // Сообщение об ошибке при неверном вводе
                cout << "Неверный выбор! Попробуйте снова." << endl;
        }

        // Если программа еще работает и пользователь не выбрал выход
        if (running && choice != '0') {
            // Спрашиваем, хочет ли пользователь продолжить
            running = continueWork();
        }
    }

    return 0;  // Завершаем программу
}

// Функция для проверки желания пользователя продолжить работу
bool continueWork() {
    char answer = 0;  // Переменная для ответа пользователя
    cout << "Продолжить работу? (y/n): ";
    cin >> answer;  // Считываем ответ
    // Возвращаем true если ответ 'y' или 'Y', иначе false
    return (answer == 'y' || answer == 'Y');
}

// Задание 1: Сумма чисел, делящихся на 5 и не делящихся на m
void task1() {
    cout << "\n=== ЗАДАНИЕ 1 ===" << endl;
    int n = 0;
    int m = 0;  // Переменные для границы диапазона и делителя
    cout << "Введите n: ";
    cin >> n;  // Считываем верхнюю границу диапазона
    cout << "Введите m (m < n): ";
    cin >> m;  // Считываем делитель

    // Проверяем корректность введенных данных
    if (m >= n) {
        cout << "Ошибка: m должно быть меньше n!" << endl;
        return;  // Выходим из функции если данные некорректны
    }

    int sum = 0;  // Переменная для накопления суммы

    // Цикл от 1 до n для перебора всех натуральных чисел
    for (int i = 1; i <= n; i++) {
        // Проверяем условия: делится на 5 И не делится на m
        if (i % kDivider_5 == 0 && i % m != 0) {
            sum += i;  // Добавляем число к сумме
        }
    }

    // Выводим результат
    cout << "Сумма чисел от 1 до " << n << ", которые делятся на 5 и не делятся на " << m << ": " << endl;
    cout << sum << endl;
}

// Задание 2: Вычисление функции S
void task2() {
    cout << "\n=== ЗАДАНИЕ 2 ===" << endl;

    double a = 0.0;
    cout << "Введите значение a: " << endl;
    cin >> a;

    double resultS = 0.0;

    if (a >= 0) {
        double result = 1.0;
        for (int i = 2; i <= kEnd_8; i += kStart_2) {
            double currentTerm = pow(i, 2);
            result *= currentTerm;
        }
        resultS = result - a;
    } else {
        double result = 1.0;
        for (int i = 3; i <= kEnd_9; i += kStart_3) {
            double currentTerm = i - 2;
            result *= currentTerm;
        }
        resultS = result;
    }
    cout << "Результат вычислений:" << endl;
    cout << "При a = " << a << ", S = " << fixed << resultS << endl;
}

// Задание 3: Вычисление суммы ряда и функции Y(x)
void task3() {
    cout << "\n=== ЗАДАНИЕ 3 ===" << endl;

    const double step = 0.2;
    const double eps = 1e-6;
    const int maxIterations = 1000000;
    double x = 0.0;

    cout << setw(kWidth_8) << "X" << setw(kWidth_12) << "Y(x)" << setw(kWidth_12) << "S(x)" << setw(kWidth_8) << "N" << endl;
    cout << fixed;

    while (x <= 1.0 + kEps) {
        double y_x = atan(x);
        double s_x = 0.0;
        int n = 0;
        double term = 0.0;
        if (fabs(x) < eps) {
            s_x = 0;
            n = 0;
        } else {
            term = x;
            s_x = term;
            n = 0;

            while (n < maxIterations) {
                term *= -x * x * (2 * n - 1) / (2 * n + 1);
                if (fabs(term) < eps) {
                    break;
                }
                s_x += term;
                n++;
            }
        }
        // do {
        //   term = (n % 2 == 0 ? 1.0 : -1.0) * pow(x, 2.0 * n + 1.0) / (2.0 * n + 1.0);
        // if (fabs(term) >= eps) {
        //   s_x += term;
        // n++;
        //}
        //} while (fabs(term) >= eps);
        cout << setw(kWidth_8) << setprecision(kPrecision_1) << x << setw(kWidth_12) << setprecision(kPrecision_6) << y_x << setw(kWidth_12)
             << setprecision(kPrecision_6) << s_x << setw(kWidth_8) << n << endl;
        x += step;
    }

    //    double y = atan(x);
}

// Задание 4: Вычисление y по формуле с частичными суммами
int task4() {
    cout << "\n=== ЗАДАНИЕ 4 ===" << endl;
    const int firstCheckValueCount = 3;
    const int secCheckValueCount = 5;
    const int thirdCheckValueCount = 10;

    int n = 0;
    double x = 0.0;

    cout << "Введите натуральное число n и x для нахождения y" << endl;
    cin >> n >> x;

    if (cin.fail() || n <= 0) {
        cout << "Введены недопустимые значения" << endl;
        return -1;
    }

    double y = 1.0;
    double curTerm = 1.0;

    for (int numOfTerm = 1; numOfTerm <= n; ++numOfTerm) {
        curTerm *= -x * (kNum_2 * numOfTerm + kNum_3) / (kNum_2 * numOfTerm);
        y += curTerm;
        //   double numerator = 1.0;
        // double denominator = 1.0;

        //    for (int i = 1; i <= numOfTerm; ++i) {
        //      numerator *= (2 * i + 3);
        //
        // for (int i = 1; i <= numOfTerm; ++i) {
        //  denominator *= (2 * i);
        //}

        //  double curTerm = pow(-1, numOfTerm) * (numerator / denominator) * pow(x, numOfTerm);

        //    y += curTerm;

        if (numOfTerm == firstCheckValueCount - 1) {  // 3 слагаемых: 1 + первое + второе
            cout << "Сумма при 3 слагаемых: " << fixed << setprecision(kPrecision_6) << y << endl;
        } else if (numOfTerm == secCheckValueCount - 1) {  // 5 слагаемых
            cout << "Сумма при 5 слагаемых: " << fixed << setprecision(kPrecision_6) << y << endl;
        } else if (numOfTerm == thirdCheckValueCount - 1) {  // 10 слагаемых
            cout << "Сумма при 10 слагаемых: " << fixed << setprecision(kPrecision_6) << y << endl;
        }
    }
    cout << "Окончательная сумма: " << n << " слагаемых: " << fixed << setprecision(kPrecision_6) << y << endl;
    return 0;
}
