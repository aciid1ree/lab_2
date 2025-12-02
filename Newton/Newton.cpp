#include <iostream>
#include <cmath>
#include <vector>
#include <omp.h>
#include <chrono>
#include <iomanip>

// Класс для хранения результатов
struct Result {
    double initial_x{};
    double root{};
    int iterations{};
    bool converged{};
    bool derivative_zero{};
};

// Функция f(x) = Ax^2 - Bx - C
double f(double A, double B, double C, double x) {
    return A * x * x - B * x - C;
}

// Производная f'(x) = 2Ax - B
double df(double A, double B, double x) {
    return 2.0 * A * x - B;
}

// Метод Ньютона 
Result newton_method(double A, double B, double C, double initial_x,
    double tolerance = 1e-10, int max_iterations = 1000) {
    Result result;
    result.initial_x = initial_x;
    result.converged = false;
    result.derivative_zero = false;

    double x = initial_x;

    for (int i = 0; i < max_iterations; i++) {
        double fx = f(A, B, C, x);
        double dfx = df(A, B, x);

        if (std::fabs(dfx) < 1e-15) {
            result.iterations = i + 1;
            result.derivative_zero = true;
            return result;
        }

        double x_new = x - fx / dfx;

        if (std::fabs(x_new - x) < tolerance) {
            result.root = x_new;
            result.iterations = i + 1;
            result.converged = true;
            return result;
        }

        x = x_new;
    }

    result.iterations = max_iterations;
    return result;
}

int main() {
    setlocale(LC_ALL, "Russian");

    double A = 1.0, B = 3.0, C = 2.0; 

    std::cout << "Метод Ньютона для решения уравнения Ax^2 - Bx = C\n";
    std::cout << "Пример: " << A << "x^2 - " << B << "x = " << C << "\n\n";

    // Начальные приближения от -4 до 4
    std::vector<double> initial_points;
    for (double x = -4.0; x <= 4.0; x += 1.0) {
        initial_points.push_back(x);
    }

    // Последовательная версия
    auto start_seq = std::chrono::high_resolution_clock::now();
    std::vector<Result> seq_results(initial_points.size());

    for (size_t i = 0; i < initial_points.size(); i++) {
        seq_results[i] = newton_method(A, B, C, initial_points[i]);
    }

    auto end_seq = std::chrono::high_resolution_clock::now();
    auto seq_time = std::chrono::duration<double>(end_seq - start_seq).count();

    // Параллельная версия
    std::cout << "\nСравнение производительности:\n";
    std::cout << std::setw(10) << "Потоки" << std::setw(15) << "Время (с)"
        << std::setw(15) << "Ускорение\n";
    std::cout << std::string(40, '-') << "\n";

    std::vector<int> threads_to_test = { 1, 2, 4, 8 };

    for (int num_threads : threads_to_test) {
        omp_set_num_threads(num_threads);

        auto start_par = std::chrono::high_resolution_clock::now();
        std::vector<Result> par_results(initial_points.size());

#pragma omp parallel for
        for (int i = 0; i < static_cast<int>(initial_points.size()); i++) {
            par_results[i] = newton_method(A, B, C, initial_points[i]);
        }

        auto end_par = std::chrono::high_resolution_clock::now();
        double par_time = std::chrono::duration<double>(end_par - start_par).count();
        double speedup = seq_time / par_time;

        std::cout << std::setw(10) << num_threads
            << std::setw(15) << std::fixed << std::setprecision(6) << par_time
            << std::setw(15) << std::fixed << std::setprecision(2) << speedup << "\n";
    }

    return 0;
}