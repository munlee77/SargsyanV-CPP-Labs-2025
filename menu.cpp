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

// итерационная функция для нового уравнения
[[nodiscard]] double CalculateIterationF_New(double x) {
    return x - 0.5 * CalculateF_New(x);
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

[[nodiscard]] bool IsFunctionsSignsEqual_New(double left, double right) {
    return ((CalculateF_New(left) > 0.) == (CalculateF_New(right) > 0.));
}

[[nodiscard]] int ConvertAccuracyToPrecision(double accuracy) {
    double epsilon = log10(accuracy);
    int precision = static_cast<int>(epsilon);
    return abs(precision);
}

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

[[nodiscard]] int ChooseEquationType() {
    int choice{};
    cout << "Выберите уравнение:\n";
    cout << "1. Оригинальное (x - k*cos(x) = 0)\n";
    cout << "2. Новое (sin(x) - (x/2+2)^(1/4) + 2 = 0)\n";
    cin >> choice;
    return choice;
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
        exit(1);
    }
    cout << fixed << setprecision(ConvertAccuracyToPrecision(accuracy)) << "Корень " << rez.root << '\t' << "Количество итераций " << rez.iterations
         << '\n';
}
}  // namespace


// namespace для имён и функций
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
            exit(1);
    }
}

// МЕТОД ИТЕРАЦИЙ
void StartIterationMethod() {
    int eqChoice = ChooseEquationType();
    if (eqChoice == 1) {
        double coefficient = EnterCoefficient();
        double accuracy = EnterAccuracy();
        double x0 = EnterX();
        EquationResult result = CalculateIterationMethod(coefficient, accuracy, x0);
        PrintEquationResult(result, accuracy);
    } else if (eqChoice == 2) {
        double accuracy = EnterAccuracy();
        double x0 = EnterX();
        EquationResult result = CalculateIterationMethodNewEquation(accuracy, x0);
        PrintEquationResult(result, accuracy);
    } else {
        cerr << "Неверный выбор уравнения\n";
        exit(1);
    }
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

EquationResult CalculateIterationMethodNewEquation(double accuracy, double x0) {
    EquationResult res;
    res.root = CalculateIterationF_New(x0);

    while (fabs(res.root - x0) > accuracy) {
        if (res.iterations == kMaxIterations) {
            res.solution = false;
            break;
        }
        x0 = res.root;
        res.root = CalculateIterationF_New(x0);
        ++res.iterations;
    }

    if (fabs(res.root - x0) > accuracy) {
        res.solution = false;
    }
    return res;
}

// МЕТОД НЬЮТОНА
void StartNewtonMethod() {
    int eqChoice = ChooseEquationType();
    if (eqChoice == 1) {
        double coefficient = EnterCoefficient();
        double accuracy = EnterAccuracy();
        double x0 = EnterX();
        EquationResult result = CalculateNewtonMethod(coefficient, accuracy, x0);
        PrintEquationResult(result, accuracy);
    } else if (eqChoice == 2) {
        double accuracy = EnterAccuracy();
        double x0 = EnterX();
        EquationResult result = CalculateNewtonMethodNewEquation(accuracy, x0);
        PrintEquationResult(result, accuracy);
    } else {
        cerr << "Неверный выбор уравнения\n";
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
    res.root = x0 - (CalculateF_New(x0) / CalculateDerivativeF_New(x0));

    while (fabs(res.root - x0) > accuracy) {
        if (res.iterations >= kMaxIterations) {
            res.solution = false;
            break;
        }
        x0 = res.root;
        double derivative = CalculateDerivativeF_New(x0);
        if (fabs(derivative) < kDivisionThreshold) {
            res.solution = false;
            break;
        }
        res.root = x0 - (CalculateF_New(x0) / derivative);
        ++res.iterations;
    }

    if (fabs(res.root - x0) > accuracy) {
        res.solution = false;
    }
    return res;
}

// МЕТОД ПОЛОВИННОГО ДЕЛЕНИЯ
void StartHalfDivisionMethod() {
    int eqChoice = ChooseEquationType();
    if (eqChoice == 1) {
        double coefficient = EnterCoefficient();
        double accuracy = EnterAccuracy();
        double left = EnterBeginRange();
        double right = EnterEndRange();
        EquationResult result = CalculateHalfDivisionMethod(coefficient, accuracy, left, right);
        PrintEquationResult(result, accuracy);
    } else if (eqChoice == 2) {
        double accuracy = EnterAccuracy();
        double left = EnterBeginRange();
        double right = EnterEndRange();
        EquationResult result = CalculateHalfDivisionMethodNewEquation(accuracy, left, right);
        PrintEquationResult(result, accuracy);
    } else {
        cerr << "Неверный выбор уравнения\n";
        exit(1);
    }
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

EquationResult CalculateHalfDivisionMethodNewEquation(double accuracy, double left, double right) {
    EquationResult res;
    if (left > right) {
        swap(left, right);
    }
    if (IsFunctionsSignsEqual_New(left, right)) {
        res.solution = false;
        return res;
    }

    double x{};
    while (right - left > accuracy) {
        if (res.iterations >= kMaxIterations) {
            res.solution = false;
            break;
        }
        x = (left + right) / kCoefficient;
        if (IsFunctionsSignsEqual_New(x, right)) {
            right = x;
        } else {
            left = x;
        }
        ++res.iterations;
    }
    res.root = (left + right) / kCoefficient;
    return res;
}
}  // namespace NonLinearEquation
   // namespace NonLinearEquation
