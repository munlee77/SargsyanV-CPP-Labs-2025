#include "menu.hpp"

namespace {
const int kMaxIterations = 1e5;

[[nodiscard]] double CalculateF(double x, double k) {
    return x - k * cos(x);
}

[[nodiscard]] double CalculateIterationF(double x, double k) {
    return k * cos(x);
}

[[nodiscard]] double CalculateDerivativeF(double x, double k) {
    return 1 + k * sin(x);
}

[[nodiscard]] bool IsFunctionsSignsEqual(double left, double right, double k) {
    return ((CalculateF(left, k) > 0.) == (CalculateF(right, k) > 0.));
}

[[nodiscard]] int ConvertAccuracyToPrecision(double accuracy) {
    double epsilon = log10(accuracy);
    int precision = static_cast<int>(epsilon);
    return abs(precision);
}

[[nodiscard]] double EnterAccuracy() {
    double accuracy{};

    std::cout << "Введите погрешность" << '\n';
    std::cin >> accuracy;

    return accuracy;
}

[[nodiscard]] double EnterCoefficient() {
    double coefficient{};

    std::cout << "Введите коеффициент перед косинусом" << '\n';
    std::cin >> coefficient;

    return coefficient;
}

[[nodiscard]] double EnterX() {
    double x0{};

    std::cout << "Введите x0 цифрами" << '\n';
    std::cin >> x0;

    return x0;
}

[[nodiscard]] double EnterBeginRange() {
    double left{};

    std::cout << "Введите начало диапазона цифрами" << '\n';
    std::cin >> left;

    return left;
}

[[nodiscard]] double EnterEndRange() {
    double right{};

    std::cout << "Введите конец диапазона цифрами" << '\n';
    std::cin >> right;

    return right;
}

[[nodiscard]] int EnterMethod() {
    int method{};

    std::cout << "Введите номер метода, которым хотите найти корень" << '\n';
    std::cout << "1. Нахождение корня методом итераций" << '\n'
              << "2. Нахождение корня методом Ньютона" << '\n'
              << "3. Нахождение корня методом половинного деления" << '\n';
    std::cin >> method;

    return method;
}

[[nodiscard]] char EnterContinueExecution() {
    char continueExecution{};

    std::cout << "Хотите продолжить? Введите [y/n]: ";
    std::cin >> continueExecution;

    return continueExecution;
}

void PrintEquationResult(NonLinearEquation::EquationResult rez, double accuracy) {
    if (!rez.solution) {
        std::cerr << "Программа не смогла найти корень c заданными данными" << std::endl;
        return;
    }
    std::cout << std::fixed << std::setprecision(ConvertAccuracyToPrecision(accuracy)) << "Корень " << rez.root << '\t' << "Количество итераций "
              << rez.iterations << '\n';
}
}  // namespace

namespace NonLinearEquation {
void StartApp() {
    char continueExecution = 'y';

    while (continueExecution == 'y') {
        ChooseTask();

        continueExecution = EnterContinueExecution();
    }
}

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
            std::cout << "Неверно введены данные" << '\n';
            break;
    }
}

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
        std::swap(left, right);
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