#ifndef NEWTON_METHOD_H
#define NEWTON_METHOD_H

// Структура для хранения результатов вычислений
struct Result {
    double initial_x;
    double root;
    int iterations;
    bool converged;
    bool derivative_zero;

    // Оператор присваивания
    Result& operator=(const Result& other) {
        if (this == &other) return *this; // Проверка на самоприсваивание
        initial_x = other.initial_x;
        root = other.root;
        iterations = other.iterations;
        converged = other.converged;
        derivative_zero = other.derivative_zero;
        return *this;
    }
};

// Функция f(x) = Ax^2 - Bx - C
double f(double A, double B, double C, double x);

// Производная f'(x) = 2Ax - B
double df(double A, double B, double x);

// Метод Ньютона (с параметрами по умолчанию)
Result newton_method(double A, double B, double C, double initial_x,
    double tolerance = 1e-10, int max_iterations = 1000);

#endif  // NEWTON_METHOD_H
