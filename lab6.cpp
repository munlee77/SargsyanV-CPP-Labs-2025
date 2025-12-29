#include <cmath>    // для мат. функций
#include <cstring>  // с-строки (вместо string -> массивы char)
#include <iomanip>
#include <iostream>

using namespace std;

// константы для оформления таблицы
const int knumberOfTableColumns = 4;
const int kfirstColumnWidth = 20;
const int ksecondColumnWidth = 18;
const int kthirdColumnWidth = 18;
const int kfourthColumnWidth = 12;

// Константы для getPrecisionFromEps
const double kEpsThreshold1 = 0.1;
const double kEpsThreshold2 = 0.01;
const double kEpsThreshold3 = 0.001;
const double kEpsThreshold4 = 0.0001;
const double kEpsThreshold5 = 0.00001;
const int kDefaultPrecision = 6;
const int kPrecision1 = 1;
const int kPrecision2 = 2;
const int kPrecision3 = 3;
const int kPrecision4 = 4;
const int kPrecision5 = 5;

// Магические числа для функций
const int kSinCoefficient = 22;
const double kHalf = 0.5;
const double kTwo = 2.0;
const int kPowerFive = 5;
const double kDivisor22 = 22.0;
const int kTitleBufferSize = 50;

// символы для оформления рамки в UTF-8
constexpr const char* const ul = "┌";
constexpr const char* const ur = "┐";
constexpr const char* const dl = "└";
constexpr const char* const dr = "┘";
constexpr const char* const vt = "│";
constexpr const char* const hz = "─";
constexpr const char* const cr = "┼";
constexpr const char* const Td = "┬";
constexpr const char* const Tu = "┴";
constexpr const char* const Tr = "├";
constexpr const char* const Tl = "┤";

// структура хранения результатов
struct ResultToPrint {
    char* name;     // название функции
    double i_sum;   // вычисленное значение
    double i_toch;  // точное значение интеграла
    int n;          // число разбиений
};

// функция для вычисления количества знаков после запятой на основе eps
int getPrecisionFromEps(double eps) {
    if (eps >= kEpsThreshold1) {
        return kPrecision1;
    } else if (eps >= kEpsThreshold2) {
        return kPrecision2;
    } else if (eps >= kEpsThreshold3) {
        return kPrecision3;
    } else if (eps >= kEpsThreshold4) {
        return kPrecision4;
    } else if (eps >= kEpsThreshold5) {
        return kPrecision5;
    } else {
        return kDefaultPrecision;  // для eps = 0.000001
    }
}

// функция для корректного вывода eps
void printEps(double eps) {
    int precision = getPrecisionFromEps(eps);
    cout << fixed << setprecision(precision) << eps;
}

// функция для печати горизонтальной линии таблицы
void printHorizontalLine(int col_widths[], const char* left, const char* middle, const char* right) {
    cout << left;
    for (int j = 0; j < knumberOfTableColumns; ++j) {
        for (int k = 0; k < col_widths[j] + 2; ++k) {
            cout << hz;
        }
        if (j < knumberOfTableColumns - 1) {
            cout << middle;
        }
    }
    cout << right << '\n';
}

// Функция вывода таблицы (теперь принимает eps для определения точности вывода)
void printTabl(ResultToPrint* i_prn, int countRowOfTable, double eps) {
    int widthOfTableColumns[knumberOfTableColumns] = {kfirstColumnWidth, ksecondColumnWidth, kthirdColumnWidth, kfourthColumnWidth};
    // заголовки значений таблицы
    const char* title[knumberOfTableColumns] = {"Function", "Integral", "IntSum", "N"};

    // Определяем точность вывода чисел на основе eps
    int precision = getPrecisionFromEps(eps);

    printHorizontalLine(widthOfTableColumns, ul, Td, ur);

    cout << vt;
    for (int j = 0; j < knumberOfTableColumns; ++j) {
        int title_len = static_cast<int>(strlen(title[j]));
        int left_spaces = (widthOfTableColumns[j] - title_len) / 2;
        int right_spaces = widthOfTableColumns[j] - title_len - left_spaces;

        cout << " ";
        for (int k = 0; k < left_spaces; ++k) {
            cout << " ";
        }
        cout << title[j];
        for (int k = 0; k < right_spaces; ++k) {
            cout << " ";
        }
        cout << " " << vt;
    }
    cout << '\n';

    printHorizontalLine(widthOfTableColumns, Tr, cr, Tl);

    // Тело таблицы
    for (int i = 0; i < countRowOfTable; ++i) {
        cout << vt;

        cout << " " << setw(widthOfTableColumns[0]) << left << i_prn[i].name << " " << vt;

        // Используем динамическую точность на основе eps
        cout << " " << setw(widthOfTableColumns[1]) << right << fixed << setprecision(precision) << i_prn[i].i_toch << " " << vt;

        cout << " " << setw(widthOfTableColumns[2]) << right << fixed << setprecision(precision) << i_prn[i].i_sum << " " << vt;

        cout << " " << setw(widthOfTableColumns[3]) << right << i_prn[i].n << " " << vt;

        cout << '\n';

        if (i < countRowOfTable - 1) {
            printHorizontalLine(widthOfTableColumns, Tr, cr, Tl);
        }
    }

    printHorizontalLine(widthOfTableColumns, dl, Tu, dr);
    cout << '\n';
}

// Тип указателя на функцию
using TPF = double (*)(double);

// функции для численного интегрирования методом прямоугольника
double integrationByRectangle(TPF f, double a, double b, double eps, int& n) {
    double I1 = 0.0;
    double I2 = 0.0;
    double h = 0.0;
    n = 1;
    I2 = 0.0;

    // Заменяем do-while на while
    while (true) {
        I1 = I2;
        n *= 2;
        h = (b - a) / n;
        I2 = 0.0;

        // Метод средних прямоугольников
        for (int i = 0; i < n; ++i) {
            double x = a + (i + kHalf) * h;
            I2 += f(x);
        }
        I2 *= h;

        if (fabs(I2 - I1) / 3 < eps) {
            break;  // правило Рунге для оценки погрешности
        }
    }

    return I2;
}

// функции для численного интегрирования методом трапеций
double integrationByTrapezoidal(TPF f, double a, double b, double eps, int& n) {
    double I1 = 0.0;
    double I2 = 0.0;
    double h = 0.0;
    n = 1;
    I2 = 0.0;

    // Заменяем do-while на while
    while (true) {
        I1 = I2;
        n *= 2;
        h = (b - a) / n;
        I2 = (f(a) + f(b)) / kTwo;

        for (int i = 1; i < n; ++i) {
            double x = a + i * h;
            I2 += f(x);
        }
        I2 *= h;

        if (fabs(I2 - I1) / 3 < eps) {
            break;  // правило Рунге для метода трапеций
        }
    }

    return I2;
}

// Подынтегральные функции
double f1(double x) {
    return x;
}
double f2(double x) {
    return sin(kSinCoefficient * x);
}
double f3(double x) {
    return pow(x, 4);
}
double f4(double x) {
    return atan(x);
}

int main() {
    double a = 0.0;
    double b = 1.0;
    cout << "Введите нижний (a) и верхний (b) пределы\n";
    cin >> a;
    cin >> b;

    // Массив указателей на подынтегральные функции
    TPF funcs[] = {f1, f2, f3, f4};

    const char* names[] = {"f1(x)=x", "f2(x)=sin(22x)", "f3(x)=x^4", "f4(x)=arctg(x)"};

    // точные значения интегралов на интервале [a, b]
    double exact[] = {kHalf * (b * b - a * a), (cos(kSinCoefficient * a) - cos(kSinCoefficient * b)) / kDivisor22,
                      (pow(b, kPowerFive) - pow(a, kPowerFive)) / static_cast<double>(kPowerFive),
                      b * atan(b) - a * atan(a) - kHalf * log((b * b + 1) / (a * a + 1))};

    const double epsilons[] = {0.01, 0.001, 0.0001, 0.00001, 0.000001};
    const int numFuncs = 4;

    // динамическое выделение памяти для массива структур
    ResultToPrint* results = new ResultToPrint[numFuncs];

    for (int j = 0; j < numFuncs; ++j) {
        // выделение памяти для названия функции (только один раз!)
        results[j].name = new char[strlen(names[j]) + 1];
        strncpy(results[j].name, names[j], strlen(names[j]) + 1);
        results[j].name[strlen(names[j])] = '\0';  // гарантируем завершающий ноль
        results[j].i_toch = exact[j];              // точное значение вычисляется один раз
        results[j].i_sum = 0.0;                    // инициализируем
        results[j].n = 0;                          // инициализируем
    }

    cout << "МЕТОД ПРЯМОУГОЛЬНИКОВ" << endl << endl;

    // Используем range-based for loop
    for (double eps : epsilons) {
        cout << "Точность eps = ";
        printEps(eps);  // Используем функцию для корректного вывода eps
        cout << endl;
        // заполнение массива results данными
        for (int j = 0; j < numFuncs; ++j) {
            int n = 0;
            // численное интегрирование методом прямоугольников
            results[j].i_sum = integrationByRectangle(funcs[j], a, b, eps, n);
            results[j].n = n;
        }
        printTabl(results, numFuncs, eps);
    }

    // ТЕПЕРЬ МЕТОД ТРАПЕЦИЙ
    cout << "МЕТОД ТРАПЕЦИЙ" << endl << endl;

    // Используем range-based for loop
    for (double eps : epsilons) {
        cout << "Точность eps = ";
        printEps(eps);
        cout << endl;

        for (int j = 0; j < numFuncs; ++j) {
            int n = 0;
            // численное интегрирование методом трапеций
            results[j].i_sum = integrationByTrapezoidal(funcs[j], a, b, eps, n);
            results[j].n = n;
        }

        printTabl(results, numFuncs, eps);
    }

    // Освобождаем память
    for (int j = 0; j < numFuncs; ++j) {
        delete[] results[j].name;
    }
    delete[] results;

    return 0;
}
