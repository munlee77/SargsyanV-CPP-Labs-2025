#include "menu.hpp"

using namespace std;

// анонимное пространство для вспомогательных функций
namespace {
// макс. число итераций
const int kMaxIterations = 1e5;

// функция
[[nodiscard]] double CalculateF(double x, double k) {
    return x - k * cos(x);
}

// итерационная функция
[[nodiscard]] double CalculateIterationF(double x, double k) {
    return k * cos(x);
}

// производная функции
[[nodiscard]] double CalculateDerivativeF(double x, double k) {
    return 1 + k * sin(x);
}

// проверка совпадения знаков
[[nodiscard]] bool IsFunctionsSignsEqual(double left, double right, double k) {
    return ((CalculateF(left, k) > 0.) == (CalculateF(right, k) > 0.));
}

// точность кол-ва знаков
[[nodiscard]] int ConvertAccuracyToPrecision(double accuracy) {
    double epsilon = log10(accuracy);
    int precision = static_cast<int>(epsilon);
    return abs(precision);
}

// ф-ции ввода данных
[[nodiscard]] double EnterAccuracy() {
    double accuracy{};

    cout << "Введите погрешность" << '\n';
    cin >> accuracy;

    return accuracy;
}

[[nodiscard]] double EnterCoefficient() {
    double coefficient{};

    cout << "Введите коеффициент перед косинусом" << '\n';
    cin >> coefficient;

    return coefficient;
}

[[nodiscard]] double EnterX() {
    double x0{};

    cout << "Введите x0 цифрами" << '\n';
    cin >> x0;

    return x0;
}

[[nodiscard]] double EnterBeginRange() {
    double left{};

    cout << "Введите начало диапазона цифрами" << '\n';
    cin >> left;

    return left;
}

[[nodiscard]] double EnterEndRange() {
    double right{};

    cout << "Введите конец диапазона цифрами" << '\n';
    cin >> right;

    return right;
}

[[nodiscard]] int EnterMethod() {
    int method{};

    cout << "Введите номер метода, которым хотите найти корень" << '\n';
    cout << "1. Нахождение корня методом итераций" << '\n'
         << "2. Нахождение корня методом Ньютона" << '\n'
         << "3. Нахождение корня методом половинного деления" << '\n';
    cin >> method;

    return method;
}

[[nodiscard]] char EnterContinueExecution() {
    char continueExecution{};

    cout << "Хотите продолжить? Введите [y/n]: ";
    cin >> continueExecution;

    return continueExecution;
}

// вывод результатов решения
void PrintEquationResult(NonLinearEquation::EquationResult rez, double accuracy) {
    if (!rez.solution) {
        cerr << "Программа не смогла найти корень c заданными данными" << endl;
        exit(0);
    }
    cout << fixed << setprecision(ConvertAccuracyToPrecision(accuracy)) << "Корень " << rez.root << '\t' << "Количество итераций " << rez.iterations
         << '\n';
}
}  // namespace
// namespace

// именованное пространство для имён и функций
namespace NonLinearEquation {
void StartApp() {
    char continueExecution = 'y';

    while (continueExecution == 'y') {
        ChooseTask();

        continueExecution = EnterContinueExecution();
    }
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
            cout << "Неверно введены данные" << '\n';
            exit(0);
    }
}

// МЕТОД ИТЕРАЦИЙ
void StartIterationMethod() {
    double сoefficient = EnterCoefficient();
    double accuracy = EnterAccuracy();
    double x0 = EnterX();

    EquationResult result = CalculateIterationMethod(сoefficient, accuracy, x0);

    PrintEquationResult(result, accuracy);
}

EquationResult CalculateIterationMethod(double сoefficient, double accuracy, double x0) {
    EquationResult res;
    res.root = CalculateIterationF(x0, сoefficient);

    while (fabs(res.root - x0) > accuracy) {
        if (res.iterations == kMaxIterations) {
            res.solution = false;
            break;
        }
        x0 = res.root;
        res.root = CalculateIterationF(x0, сoefficient);
        ++res.iterations;
    }

    if (fabs(res.root - x0) > accuracy) {
        res.solution = false;
    }
    return res;
}

// МЕТОД НЬЮТОНА
void StartNewtonMethod() {
    double сoefficient = EnterCoefficient();
    double accuracy = EnterAccuracy();
    double x0 = EnterX();

    EquationResult result = CalculateNewtonMethod(сoefficient, accuracy, x0);

    PrintEquationResult(result, accuracy);
}

EquationResult CalculateNewtonMethod(double сoefficient, double accuracy, double x0) {
    EquationResult res;
    res.root = x0 - (CalculateF(x0, сoefficient) / CalculateDerivativeF(x0, сoefficient));

    while (fabs(res.root - x0) > accuracy) {
        if (res.iterations == kMaxIterations) {
            res.solution = false;
            break;
        }
        x0 = res.root;
        res.root -= CalculateF(x0, сoefficient) / CalculateDerivativeF(x0, сoefficient);
        ++res.iterations;
    }

    if (fabs(res.root - x0) > accuracy) {
        res.solution = false;
    }

    return res;
}

// МЕТОД ПОЛОВИННОГО ДЕЛЕНИЯ
void StartHalfDivisionMethod() {
    double сoefficient = EnterCoefficient();
    double accuracy = EnterAccuracy();
    double left = EnterBeginRange();
    double right = EnterEndRange();

    EquationResult result = CalculateHalfDivisionMethod(сoefficient, accuracy, left, right);

    PrintEquationResult(result, accuracy);
}

EquationResult CalculateHalfDivisionMethod(double сoefficient, double accuracy, double left, double right) {
    EquationResult res;
    if (left > right) {
        swap(left, right);
    }

    if (IsFunctionsSignsEqual(left, right, сoefficient) == true) {
        res.solution = false;
        return res;
    }

    int iterations{};
    double x{};

    while (right - left > accuracy) {
        x = (right + left) / 2;

        if (IsFunctionsSignsEqual(x, right, сoefficient) == false) {
            left = x;
        } else {
            right = x;
        }
        ++iterations;
    }
    res.root = (right + left) / 2;
    res.iterations = iterations;
    return res;
}
}  // namespace NonLinearEquation
   // namespace NonLinearEquation
