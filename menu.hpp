#include <cmath>
#include <iomanip>
#include <iostream>

// именованное пространство для имён и функций
namespace NonLinearEquation {

enum class EquationMethod {
    // метод итераций
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
void StartIterationMethod();
void StartNewtonMethod();
void StartHalfDivisionMethod();

// вычислительные функции (с атрибутом nodiscard)
[[nodiscard]] EquationResult CalculateIterationMethod(double coefficient, double epsilon, double x0);
[[nodiscard]] EquationResult CalculateNewtonMethod(double coefficient, double epsilon, double x0);
[[nodiscard]] EquationResult CalculateHalfDivisionMethod(double coefficient, double epsilon, double left, double right);
}  // namespace NonLinearEquation
// namespace NonLinearEquation
