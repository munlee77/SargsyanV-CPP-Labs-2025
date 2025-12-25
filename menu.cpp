#include "menu.hpp"

using namespace std;

// namespace для вспомогательных функций
namespace {

const int kMaxIterations = 1e5;
const double kDivisionThreshold = 1e-12;
const double kCoefficient = 2.0;
const double kRootDegree = 0.25;
const double kDerivativeConstant = 1.0 / 8.0;
const double kRootDegreeDerivative = -0.75;
const double kHalfDivisor = 2.0;

// функция
[[nodiscard]] double CalculateF(double x, double k) {
    return x - k * cos(x);
}

// итерационная функция
[[nodiscard]] double CalculateIterationF(double x, double k) {
    return k * cos(x);
}

// функция для нового уравнения
[[nodiscard]] double CalculateF_New(double x) {
    return sin(x) - pow(x / kCoefficient + kCoefficient, kRootDegree) + kCoefficient;
}

// производная функции
[[nodiscard]] double CalculateDerivativeF(double x, double k) {
    return 1 + k * sin(x);
}

// проверка совпадения знаков
[[nodiscard]] bool IsFunctionsSignsEqual(double left, double right, double k) {
    return ((CalculateF(left, k) > 0.) == (CalculateF(right, k) > 0.));
}

[[nodiscard]] double CalculateDerivativeF_New(double x) {
    return cos(x) - (kDerivativeConstant)*pow(x / kCoefficient + kCoefficient, kRootDegreeDerivative);
}

[[nodiscard]] int ConvertAccuracyToPrecision(double accuracy) {
    double epsilon = log10(accuracy);
    int precision = static_cast<int>(epsilon);
    return abs(precision);
}

// Ввод точности
[[nodiscard]] double EnterAccuracy() {
    double accuracy{};
    cout << "Введите погрешность: ";
    cin >> accuracy;
    return accuracy;
}

// Ввод коэффициента (только для первого уравнения)
[[nodiscard]] double EnterCoefficient() {
    double coefficient{};
    cout << "Введите коэффициент перед косинусом (k): ";
    cin >> coefficient;
    return coefficient;
}

// Ввод начального приближения
[[nodiscard]] double EnterX() {
    double x0{};
    cout << "Введите начальное приближение x0: ";
    cin >> x0;
    return x0;
}

// Ввод начала диапазона
[[nodiscard]] double EnterBeginRange() {
    double left{};
    cout << "Введите начало диапазона: ";
    cin >> left;
    return left;
}

// Ввод конца диапазона
[[nodiscard]] double EnterEndRange() {
    double right{};
    cout << "Введите конец диапазона: ";
    cin >> right;
    return right;
}

// Ввод метода решения
[[nodiscard]] int EnterMethod() {
    int method{};
    cout << "\n ВЫБОР МЕТОДА " << endl;
    cout << "1. Метод простых итераций" << endl;
    cout << "2. Метод Ньютона" << endl;
    cout << "3. Метод половинного деления" << endl;
    cout << "Введите номер метода: ";
    cin >> method;
    return method;
}

// Выбор уравнения (только для методов Ньютона и половинного деления)
[[nodiscard]] int ChooseEquationType() {
    int choice{};
    cout << "\n ВЫБОР УРАВНЕНИЯ " << endl;
    cout << "1. x - k*cos(x) = 0" << endl;
    cout << "2. sin(x) - (x/2+2)^(1/4) + 2 = 0" << endl;
    cout << "Введите номер уравнения: ";
    cin >> choice;
    return choice;
}

// Запрос на продолжение
[[nodiscard]] char EnterContinueExecution() {
    char continueExecution{};
    cout << "\nХотите продолжить? (y/n): ";
    cin >> continueExecution;
    return continueExecution;
}

// Вывод результатов решения
void PrintEquationResult(NonLinearEquation::EquationResult rez, double accuracy) {
    if (!rez.solution) {
        cerr << "\nОШИБКА: Не удалось найти корень с заданными параметрами" << endl;
        return;
    }
    cout << fixed << setprecision(ConvertAccuracyToPrecision(accuracy)) << "\nРЕЗУЛЬТАТ:" << endl
         << "Корень: " << rez.root << endl
         << "Количество итераций: " << rez.iterations << endl;
}
}  // namespace

// namespace для имён и функций
namespace NonLinearEquation {
void StartApp() {
    char continueExecution = 'y';

    cout << " РЕШЕНИЕ НЕЛИНЕЙНЫХ УРАВНЕНИЙ " << endl;

    while (continueExecution == 'y' || continueExecution == 'Y') {
        ChooseTask();
        continueExecution = EnterContinueExecution();
    }

    cout << "\nПрограмма завершена. До свидания!" << endl;
}

// меню выбора метода
void ChooseTask() {
    int method = EnterMethod();

    switch (static_cast<EquationMethod>(method)) {
        case EquationMethod::Iteration:
            StartIterationMethod();
            break;
        case EquationMethod::Newton:
            StartNewtonMethod();
            break;
        case EquationMethod::HalfDivision:
            StartHalfDivisionMethod();
            break;
        default:
            cerr << "ОШИБКА: Неверный номер метода. Допустимые значения: 1, 2, 3." << endl;
            exit(1);
    }
}

// МЕТОД ИТЕРАЦИЙ (ТОЛЬКО ДЛЯ ПЕРВОГО УРАВНЕНИЯ)
void StartIterationMethod() {
    cout << "\n МЕТОД ПРОСТЫХ ИТЕРАЦИЙ " << endl;
    cout << "Уравнение: x - k*cos(x) = 0" << endl;

    // Только первое уравнение - коэффициент, точность, начальное приближение
    double coefficient = EnterCoefficient();
    double accuracy = EnterAccuracy();
    double x0 = EnterX();

    EquationResult result = CalculateIterationMethod(coefficient, accuracy, x0);
    PrintEquationResult(result, accuracy);
}

EquationResult CalculateIterationMethod(double coefficient, double accuracy, double x0) {
    EquationResult res;
    res.root = CalculateIterationF(x0, coefficient);

    while (fabs(res.root - x0) > accuracy) {
        if (res.iterations >= kMaxIterations) {
            res.solution = false;
            break;
        }
        x0 = res.root;
        res.root = CalculateIterationF(x0, coefficient);
        ++res.iterations;
    }

    if (fabs(res.root - x0) > accuracy) {
        res.solution = false;
    }
    return res;
}

// МЕТОД НЬЮТОНА (ДЛЯ ОБОИХ УРАВНЕНИЙ)
void StartNewtonMethod() {
    cout << "\n МЕТОД НЬЮТОНА " << endl;
    int eqChoice = ChooseEquationType();

    if (eqChoice == 1) {
        // Для первого уравнения: x - k*cos(x) = 0
        double coefficient = EnterCoefficient();
        double accuracy = EnterAccuracy();
        double x0 = EnterX();
        EquationResult result = CalculateNewtonMethod(coefficient, accuracy, x0);
        PrintEquationResult(result, accuracy);
    } else if (eqChoice == 2) {
        // Для второго уравнения: sin(x) - (x/2+2)^(1/4) + 2 = 0
        double accuracy = EnterAccuracy();
        double x0 = EnterX();
        EquationResult result = CalculateNewtonMethodNewEquation(accuracy, x0);
        PrintEquationResult(result, accuracy);
    } else {
        cerr << "Неверный выбор уравнения" << endl;
        exit(1);
    }
}

EquationResult CalculateNewtonMethod(double coefficient, double accuracy, double x0) {
    EquationResult res;
    res.root = x0 - (CalculateF(x0, coefficient) / CalculateDerivativeF(x0, coefficient));

    while (fabs(res.root - x0) > accuracy) {
        if (res.iterations >= kMaxIterations) {
            res.solution = false;
            break;
        }
        x0 = res.root;
        double derivative = CalculateDerivativeF(x0, coefficient);
        if (fabs(derivative) < kDivisionThreshold) {
            res.solution = false;
            break;
        }
        res.root = x0 - (CalculateF(x0, coefficient) / derivative);
        ++res.iterations;
    }

    if (fabs(res.root - x0) > accuracy) {
        res.solution = false;
    }
    return res;
}

EquationResult CalculateNewtonMethodNewEquation(double accuracy, double x0) {
    EquationResult res;

    // ПРОСТО вычисляем
    res.root = x0 - (CalculateF_New(x0) / CalculateDerivativeF_New(x0));

    if (std::isnan(res.root)) {
        cerr << "ОШИБКА: Не удалось вычислить." << endl;
        res.solution = false;
        return res;
    }

    // Обычный цикл метода Ньютона
    while (fabs(res.root - x0) > accuracy) {
        if (res.iterations >= kMaxIterations) {
            res.solution = false;
            break;
        }
        x0 = res.root;
        double derivative = CalculateDerivativeF_New(x0);

        if (std::isnan(derivative)) {
            res.solution = false;
            break;
        }

        if (fabs(derivative) < kDivisionThreshold) {
            res.solution = false;
            break;
        }
        res.root = x0 - (CalculateF_New(x0) / derivative);
        ++res.iterations;
    }

    if (fabs(res.root - x0) > accuracy || std::isnan(res.root)) {
        res.solution = false;
    }
    return res;
}

// МЕТОД ПОЛОВИННОГО ДЕЛЕНИЯ (ДЛЯ ОБОИХ УРАВНЕНИЙ)
void StartHalfDivisionMethod() {
    cout << "\n МЕТОД ПОЛОВИННОГО ДЕЛЕНИЯ " << endl;
    int eqChoice = ChooseEquationType();

    if (eqChoice == 1) {
        // Для первого уравнения: x - k*cos(x) = 0
        double coefficient = EnterCoefficient();
        double accuracy = EnterAccuracy();
        double left = EnterBeginRange();
        double right = EnterEndRange();
        EquationResult result = CalculateHalfDivisionMethod(coefficient, accuracy, left, right);
        PrintEquationResult(result, accuracy);
    } else if (eqChoice == 2) {
        // Для второго уравнения: sin(x) - (x/2+2)^(1/4) + 2 = 0
        double accuracy = EnterAccuracy();
        double left = EnterBeginRange();
        double right = EnterEndRange();
        EquationResult result = CalculateHalfDivisionMethodNewEquation(accuracy, left, right);
        PrintEquationResult(result, accuracy);
    } else {
        cerr << "ОШИБКА: Неверный выбор уравнения." << endl;
        exit(1);
    }
}

EquationResult CalculateHalfDivisionMethod(double coefficient, double accuracy, double left, double right) {
    EquationResult res;

    if (left > right) {
        swap(left, right);
        cout << "Замечание: Границы интервала были поменяны местами." << endl;
    }

    if (IsFunctionsSignsEqual(left, right, coefficient)) {
        cerr << "ОШИБКА: На концах интервала функция имеет одинаковые знаки." << endl;
        res.solution = false;
        return res;
    }

    while (right - left > accuracy && res.iterations < kMaxIterations) {
        double mid = (left + right) / kHalfDivisor;

        if (CalculateF(left, coefficient) * CalculateF(mid, coefficient) <= 0) {
            right = mid;
        } else {
            left = mid;
        }
        ++res.iterations;
    }

    if (res.iterations >= kMaxIterations) {
        res.solution = false;
        return res;
    }

    res.root = (left + right) / kHalfDivisor;
    return res;
}

EquationResult CalculateHalfDivisionMethodNewEquation(double accuracy, double left, double right) {
    EquationResult res;

    if (left > right) {
        swap(left, right);
        cout << "Замечание: Границы интервала были поменяны местами." << endl;
    }

    double f_left = CalculateF_New(left);
    double f_right = CalculateF_New(right);

    if (std::isnan(f_left) || std::isnan(f_right)) {
        cerr << "\nОШИБКА: Функция не определена на границах интервала!" << endl;
        res.solution = false;
        return res;
    }

    if (f_left * f_right > 0) {
        cerr << "\nОШИБКА: Функция имеет одинаковые знаки на концах интервала!" << endl;
        res.solution = false;
        return res;
    }

    while (right - left > accuracy) {
        if (res.iterations >= kMaxIterations) {
            cerr << "\nОШИБКА: Достигнуто максимальное количество итераций (" << kMaxIterations << ")" << endl;
            res.solution = false;
            return res;
        }

        double mid = (left + right) / kHalfDivisor;
        double f_mid = CalculateF_New(mid);

        if (std::isnan(f_mid)) {
            cerr << "\nОШИБКА: Функция не определена в точке x = " << mid << endl;
            res.solution = false;
            return res;
        }

        if (f_left * f_mid <= 0) {
            right = mid;
        } else {
            left = mid;
            f_left = f_mid;
        }

        ++res.iterations;
    }

    res.root = (left + right) / kHalfDivisor;
    return res;
}
}  // namespace NonLinearEquation
