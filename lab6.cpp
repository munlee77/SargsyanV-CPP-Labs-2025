#include <iostream>
#include <cmath> // для мат. функций
#include <iomanip> // форматирование cin/cout
#include <cstring> // с-строки (вместо string -> массивы char)

using namespace std;

// константы для оформления таблицы
const int knumberOfTableColumns = 4;
const int kfirstColumnWidth = 20;
const int ksecondColumnWidth = 18;
const int kthirdColumnWidth = 18;
const int kfourthColumnWidth = 12;

// символы рамки в UTF-8
const char* ul = "┌";
const char* ur = "┐";
const char* dl = "└";
const char* dr = "┘";
const char* vt = "│";
const char* hz = "─";
const char* cr = "┼";
const char* Td = "┬";
const char* Tu = "┴";
const char* Tr = "├";
const char* Tl = "┤";

// структура хранения результатов
struct ResultToPrint {
    char* name;      // название функции
    double i_sum;    // численно рассчитанный интеграл
    double i_toch;   // точное значение интеграла
    int n;           // число разбиений
};

// функция для вычисления количества знаков после запятой на основе eps
int getPrecisionFromEps(double eps) {
    if (eps >= 0.1) return 1;
    else if (eps >= 0.01) return 2;
    else if (eps >= 0.001) return 3;
    else if (eps >= 0.0001) return 4;
    else if (eps >= 0.00001) return 5;
    else return 6;  // для eps = 0.000001
}

// функция для корректного вывода eps
void printEps(double eps) {
    int precision = getPrecisionFromEps(eps);
    cout << fixed << setprecision(precision) << eps;
}

// функция для печати горизонтальной линии таблицы
void printHorizontalLine(int col_widths[], const char* left, const char* middle, const char* right) {
    cout << left;
    for (int j = 0; j < knumberOfTableColumns; j++) {
        for (int k = 0; k < col_widths[j] + 2; k++) {
            cout << hz;
        }
        if (j < knumberOfTableColumns - 1) {
            cout << middle;
        }
    }
    cout << right << endl;
}

// Функция вывода таблицы (теперь принимает eps для определения точности вывода)
void printTabl(ResultToPrint* i_prn, int countRowOfTable, double eps) {
    int widthOfTableColumns[knumberOfTableColumns] = {
        kfirstColumnWidth, ksecondColumnWidth,
        kthirdColumnWidth, kfourthColumnWidth
    };
    // заголовки значений таблицы
    char title[knumberOfTableColumns][50] = {
        "Function", "Integral", "IntSum", "N"
    };

    // Определяем точность вывода чисел на основе eps
    int precision = getPrecisionFromEps(eps);

    printHorizontalLine(widthOfTableColumns, ul, Td, ur);

    cout << vt;
    for (int j = 0; j < knumberOfTableColumns; j++) {
        int title_len = strlen(title[j]);
        int left_spaces = (widthOfTableColumns[j] - title_len) / 2;
        int right_spaces = widthOfTableColumns[j] - title_len - left_spaces;

        cout << " ";
        for (int k = 0; k < left_spaces; k++) cout << " ";
        cout << title[j];
        for (int k = 0; k < right_spaces; k++) cout << " ";
        cout << " " << vt;
    }
    cout << endl;

    printHorizontalLine(widthOfTableColumns, Tr, cr, Tl);

    // Тело таблицы
    for (int i = 0; i < countRowOfTable; i++) {
        cout << vt;

        cout << " " << setw(widthOfTableColumns[0]) << left << i_prn[i].name << " " << vt;

        // Используем динамическую точность на основе eps
        cout << " " << setw(widthOfTableColumns[1]) << right << fixed
             << setprecision(precision) << i_prn[i].i_toch << " " << vt;

        cout << " " << setw(widthOfTableColumns[2]) << right << fixed
             << setprecision(precision) << i_prn[i].i_sum << " " << vt;

        cout << " " << setw(widthOfTableColumns[3]) << right << i_prn[i].n << " " << vt;

        cout << endl;

        if (i < countRowOfTable - 1) {
            printHorizontalLine(widthOfTableColumns, Tr, cr, Tl);
        }
    }

    printHorizontalLine(widthOfTableColumns, dl, Tu, dr);
    cout << endl;
}

// Тип указателя на функцию
typedef double (*TPF)(double);

// функции для численного интегрирования методом прямоугольника
double integrationByRectangle(TPF f, double a, double b, double eps, int& n) {
    double I1, I2;
    double h;
    n = 1;
    I2 = 0.0;

    do {
        I1 = I2;
        n *= 2;
        h = (b - a) / n;
        I2 = 0.0;

        // Метод средних прямоугольников
        for (int i = 0; i < n; i++) {
            double x = a + (i + 0.5) * h;
            I2 += f(x);
        }
        I2 *= h;

    } while (fabs(I2 - I1) / 3 >= eps); // правило Рунге для оценки погрешности

    return I2;
}

// функции для численного интегрирования методом трапеций
double integrationByTrapezoidal(TPF f, double a, double b, double eps, int& n) {
    double I1, I2;
    double h;
    n = 1;
    I2 = 0.0;

    do {
        I1 = I2;
        n *= 2;
        h = (b - a) / n;
        I2 = (f(a) + f(b)) / 2.0;

        for (int i = 1; i < n; i++) {
            double x = a + i * h;
            I2 += f(x);
        }
        I2 *= h;

    } while (fabs(I2 - I1) / 3 >= eps); // правило Рунге для метожа трапеций

    return I2;
}

// Подынтегральные функции
double f1(double x) { return x; }
double f2(double x) { return sin(22 * x); }
double f3(double x) { return pow(x, 4); }
double f4(double x) { return atan(x); }

int main() {
    const double a = 0.0;
    const double b = 1.0;

    // Массив указателей на подынтегральные функции
    TPF funcs[] = { f1, f2, f3, f4 };

    const char* names[] = { "f1(x)=x", "f2(x)=sin(22x)", "f3(x)=x^4", "f4(x)=arctg(x)" };

    // точные значения интегралов на интервале [0, 1]
    double exact[] = {
        0.5,
        (1.0 - cos(22.0)) / 22.0,
        0.2,
        atan(1.0) - 0.5 * log(2.0)
    };

    double epsilons[] = { 0.01, 0.001, 0.0001, 0.00001, 0.000001 };
    const int numEps = 5;
    const int numFuncs = 4;

    // динамическое выделение массива структур для хранения рез.
    ResultToPrint* results = new ResultToPrint[numFuncs];

    cout << "МЕТОД ПРЯМОУГОЛЬНИКОВ" << endl << endl;

    for (int i = 0; i < numEps; i++) {
        double eps = epsilons[i];
        cout << "Точность eps = ";
        printEps(eps);  // Используем функцию для корректного вывода eps
        cout << endl;

        for (int j = 0; j < numFuncs; j++) {
            // выделение памяти для строки
            results[j].name = new char[strlen(names[j]) + 1];
            strcpy(results[j].name, names[j]);

            results[j].i_toch = exact[j];
            int n = 0;

            // численное интегрирование методом прямоугольников
            results[j].i_sum = integrationByRectangle(funcs[j], a, b, eps, n);
            results[j].n = n;
        }

        // Передаем eps в функцию печати таблицы
        printTabl(results, numFuncs, eps);

        // освобождение памяти
        for (int j = 0; j < numFuncs; j++) {
            delete[] results[j].name;
        }
    }

    // ТЕПЕРЬ МЕТОД ТРАПЕЦИЙ
    cout << "МЕТОД ТРАПЕЦИЙ" << endl << endl;

    for (int i = 0; i < numEps; i++) {
        double eps = epsilons[i];
        cout << "Точность eps = ";
        printEps(eps);
        cout << endl;

        for (int j = 0; j < numFuncs; j++) {
            results[j].name = new char[strlen(names[j]) + 1];
            strcpy(results[j].name, names[j]);

            results[j].i_toch = exact[j];
            int n = 0;

            // численное интегрирование методом трапеций
            results[j].i_sum = integrationByTrapezoidal(funcs[j], a, b, eps, n);
            results[j].n = n;
        }

        printTabl(results, numFuncs, eps);

        for (int j = 0; j < numFuncs; j++) {
            delete[] results[j].name;
        }
    }

    delete[] results;
    return 0;
}
