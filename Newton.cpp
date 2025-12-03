#include <iostream>
#include <cmath>
#include <vector>
#include <omp.h>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <windows.h>
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

int run_newton_openmp_demo() {
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    double A, B, C;

    std::cout << "Метод Ньютона для решения уравнения Ax^2 - Bx = C\n\n";

    // Ввод параметров уравнения с проверкой, что вводятся числа
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
    std::cout << "Или: " << A << "x^2 - " << B << "x - " << C << " = 0\n\n";

    // Начальные приближения от -4 до 4 с шагом 1
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
    std::cout << "Результаты для начальных точек от -4 до 4:\n";
    std::cout << std::setw(10) << "x0"
        << std::setw(15) << "Корень"
        << std::setw(12) << "Итерации"
        << std::setw(12) << "Сходимость"
        << std::setw(15) << "Причина\n";
    std::cout << std::string(64, '-') << "\n";

    for (size_t i = 0; i < seq_results.size(); i++) {
        const auto& r = seq_results[i];

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

        if (r.converged) {
            std::cout << std::setw(10) << r.initial_x
                << std::setw(15) << std::fixed << std::setprecision(8) << r.root
                << std::setw(12) << r.iterations
                << std::setw(12) << conv_str
                << std::setw(15) << reason_str << "\n";
        }
        else {
            std::cout << std::setw(10) << r.initial_x
                << std::setw(15) << "--"
                << std::setw(12) << r.iterations
                << std::setw(12) << conv_str
                << std::setw(15) << reason_str << "\n";
        }
    }

    std::cout << "\nВремя последовательного выполнения: "
        << std::fixed << std::setprecision(6) << seq_time << " с\n\n";

    // Параллельная версия с разным количеством потоков
    std::vector<int> threads_to_test = { 1, 2, 4, 8 };
    std::vector<double> parallel_times;
    std::vector<double> speedups;

    std::cout << "Производительность при разном количестве потоков:\n";
    std::cout << std::setw(10) << "Потоки"
        << std::setw(15) << "Время (с)"
        << std::setw(15) << "Ускорение"
        << std::setw(15) << "Эффективность\n";
    std::cout << std::string(60, '-') << "\n";

    for (int num_threads : threads_to_test) {
        omp_set_num_threads(num_threads);

        auto start_par = std::chrono::high_resolution_clock::now();
        std::vector<Result> par_results(initial_points.size());

#pragma omp parallel for default(none) shared(A, B, C, initial_points, par_results)
        for (int i = 0; i < static_cast<int>(initial_points.size()); i++) {
            par_results[i] = newton_method(A, B, C, initial_points[i]);
        }

        auto end_par = std::chrono::high_resolution_clock::now();
        double par_time = std::chrono::duration<double>(end_par - start_par).count();

        double speedup = seq_time / par_time;
        double efficiency = speedup / num_threads;

        parallel_times.push_back(par_time);
        speedups.push_back(speedup);

        std::cout << std::setw(10) << num_threads
            << std::setw(15) << std::fixed << std::setprecision(6) << par_time
            << std::setw(15) << std::fixed << std::setprecision(2) << speedup
            << std::setw(15) << std::fixed << std::setprecision(2) << efficiency * 100 << "%\n";
    }

    // Сохранение данных для графика
    std::ofstream data_file("speedup_data.txt");
    if (data_file.is_open()) {
        data_file << "# Потоки Время Ускорение Линейное_ускорение\n";
        for (size_t i = 0; i < threads_to_test.size(); i++) {
            data_file << threads_to_test[i] << " "
                << parallel_times[i] << " "
                << speedups[i] << " "
                << threads_to_test[i] << "\n";
        }
        data_file.close();
        std::cout << "\nДанные для графика сохранены в файл: speedup_data.txt\n";
    }

    // Генерация скрипта для построения графика
    std::ofstream gnuplot_script("plot_speedup.gp");
    if (gnuplot_script.is_open()) {
        gnuplot_script << "set terminal pngcairo size 800,600 enhanced font 'Verdana,10'\n";
        gnuplot_script << "set output 'speedup_comparison.png'\n";
        gnuplot_script << "set title 'Ускорение метода Ньютона с OpenMP\\nУравнение: "
            << A << "x^2 - " << B << "x = " << C << "'\n";
        gnuplot_script << "set xlabel 'Количество потоков'\n";
        gnuplot_script << "set ylabel 'Ускорение'\n";
        gnuplot_script << "set grid\n";
        gnuplot_script << "set key top left\n";
        gnuplot_script << "set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5\n";
        gnuplot_script << "set style line 2 lc rgb '#dd181f' lt 1 lw 2 dt 2\n";
        gnuplot_script << "plot 'speedup_data.txt' using 1:3 title 'Реальное ускорение' with linespoints ls 1, \\\n";
        gnuplot_script << "     'speedup_data.txt' using 1:4 title 'Линейное ускорение' with lines ls 2\n";
        gnuplot_script.close();

        std::cout << "Скрипт для построения графика сохранен в файл: plot_speedup.gp\n\n";
        std::cout << "Для построения графика выполните команду:\n";
        std::cout << "gnuplot plot_speedup.gp\n";
        std::cout << "График будет сохранен в файле 'speedup_comparison.png'\n";
    }

    return 0;
}
