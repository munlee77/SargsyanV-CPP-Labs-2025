#include <cmath>
#include <iomanip>
#include <iostream>

// именованное пространство для имён и функций
namespace NonLinearEquation {

enum class EquationMethod {
    // метод итераций (только для уравнения x - k*cos(x) = 0)
    Iteration = 1,
    // метод Ньютона
    Newton,
    // метод половинного деления
    HalfDivision
};

// cтруктура для возврата результатов решения
struct EquationResult {
    // найденный корень
    double root = 0.0;
    // кол-во итераций
    int iterations = 0;
    // флаг успешности решения
    bool solution = true;
};

// интерфейсные функции
void StartApp();
void ChooseTask();

// функции запуска методов
void StartIterationMethod();     // Метод итераций (только для уравнения x - k*cos(x) = 0)
void StartNewtonMethod();        // Метод Ньютона (для обоих уравнений)
void StartHalfDivisionMethod();  // Метод половинного деления (для обоих уравнений)

// вычислительные функции (с атрибутом nodiscard)
[[nodiscard]] EquationResult CalculateIterationMethod(double coefficient, double epsilon, double x0);

// Методы для первого уравнения: x - k*cos(x) = 0
[[nodiscard]] EquationResult CalculateNewtonMethod(double coefficient, double epsilon, double x0);
[[nodiscard]] EquationResult CalculateHalfDivisionMethod(double coefficient, double epsilon, double left, double right);

// Методы для второго уравнения: sin(x) - (x/2+2)^(1/4) + 2 = 0
[[nodiscard]] EquationResult CalculateNewtonMethodNewEquation(double epsilon, double x0);
[[nodiscard]] EquationResult CalculateHalfDivisionMethodNewEquation(double epsilon, double left, double right);
}  // namespace NonLinearEquation
