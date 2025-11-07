#include <cmath>
#include <iomanip>
#include <iostream>

namespace NonLinearEquation {

enum class EquationMethod {
    Iteration = 1,
    Newton,
    HalfDivision
};

struct EquationResult {
    double root = 0.0;
    int iterations = 0;
    bool solution = true;
};

void StartApp();
void ChooseTask();

void StartIterationMethod();
void StartNewtonMethod();
void StartHalfDivisionMethod();

[[nodiscard]] EquationResult CalculateIterationMethod(double coefficient, double epsilon, double x0);
[[nodiscard]] EquationResult CalculateNewtonMethod(double coefficient, double epsilon, double x0);
[[nodiscard]] EquationResult CalculateHalfDivisionMethod(double coefficient, double epsilon, double left, double right);
}  // namespace NonLinearEquation