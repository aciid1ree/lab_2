#include <iostream>
#include <cmath>
#include <vector>
#include <omp.h>
#include <chrono>
#include <iomanip>
#include <limits>
#include <string>

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

        // Проверка сходимости
        if (std::fabs(x_new - x) < tolerance && std::fabs(fx) < tolerance) {
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

// Безопасный ввод числа с проверкой
bool read_double(const std::string& prompt, double& value) {
    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.good()) {
            return true;
        }

        if (std::cin.eof()) {
            return false;
        }

        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Ошибка ввода. Пожалуйста, введите число.\n";
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    double A, B, C;

    std::cout << "Метод Ньютона для решения уравнения Ax^2 - Bx = C\n\n";

    // Ввод параметров уравнения с проверкой
    if (!read_double("Введите коэффициент A: ", A)) {
        std::cerr << "Ошибка: поток ввода закрыт. Завершение программы.\n";
        return 1;
    }
    if (!read_double("Введите коэффициент B: ", B)) {
        std::cerr << "Ошибка: поток ввода закрыт. Завершение программы.\n";
        return 1;
    }
    if (!read_double("Введите коэффициент C: ", C)) {
        std::cerr << "Ошибка: поток ввода закрыт. Завершение программы.\n";
        return 1;
    }

    std::cout << "\nРешение уравнения: " << A << "x^2 - " << B << "x = " << C << "\n";

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

    // Вывод результатов
    std::cout << "\nРезультаты для начальных точек от -4 до 4:\n";
    std::cout << std::setw(10) << "x0" << std::setw(15) << "Корень"
        << std::setw(12) << "Итерации" << std::setw(12) << "Сходимость"
        << std::setw(15) << "Причина\n";
    std::cout << std::string(64, '-') << "\n";

    for (const auto& r : seq_results) {
        std::string conv_str = r.converged ? "да" : "нет";
        std::string reason_str = "-";

        if (!r.converged) {
            if (r.derivative_zero) {
                reason_str = "df=0";
            }
            else {
                reason_str = "макс. итераций";
            }
        }

        std::cout << std::setw(10) << r.initial_x;
        if (r.converged) {
            std::cout << std::setw(15) << std::fixed << std::setprecision(8) << r.root
                << std::setw(12) << r.iterations
                << std::setw(12) << conv_str
                << std::setw(15) << reason_str << "\n";
        }
        else {
            std::cout << std::setw(15) << "--"
                << std::setw(12) << r.iterations
                << std::setw(12) << conv_str
                << std::setw(15) << reason_str << "\n";
        }
    }

    std::cout << "\nВремя последовательного выполнения: "
        << std::fixed << std::setprecision(6) << seq_time << " с\n";

    return 0;
}