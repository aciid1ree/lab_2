#include <iostream>
#include <cmath>
#include <vector>
#include <omp.h>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <limits>
#include <string>

// Struct to store results
struct Result {
    double initial_x{};
    double root{};
    int iterations{};
    bool converged{};
    bool derivative_zero{};
};

// Function f(x) = Ax^2 - Bx - C
double f(double A, double B, double C, double x) {
    return A * x * x - B * x - C;
}

// Derivative f'(x) = 2Ax - B
double df(double A, double B, double x) {
    return 2.0 * A * x - B;
}

// Newton's method
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

        // Convergence check
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

// Safe input with validation
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
        std::cout << "Input error. Please enter a valid number.\n";
    }
}

// This function contains all the logic previously in main()
// You can call this function from another console application.
int run_newton_openmp_demo() {
    double A, B, C;

    std::cout << "Newton's Method for solving the equation A*x^2 - B*x = C\n\n";

    // Read equation parameters with validation
    if (!read_double("Enter coefficient A: ", A)) {
        std::cerr << "Error: input stream closed. Exiting.\n";
        return 1;
    }
    if (!read_double("Enter coefficient B: ", B)) {
        std::cerr << "Error: input stream closed. Exiting.\n";
        return 1;
    }
    if (!read_double("Enter coefficient C: ", C)) {
        std::cerr << "Error: input stream closed. Exiting.\n";
        return 1;
    }

    std::cout << "\nSolving equation: " << A << "x^2 - " << B << "x = " << C << "\n";
    std::cout << "Or: " << A << "x^2 - " << B << "x - " << C << " = 0\n\n";

    // Initial guesses from -4 to 4 with step 1
    std::vector<double> initial_points;
    for (double x = -4.0; x <= 4.0; x += 1.0) {
        initial_points.push_back(x);
    }

    // Sequential version
    auto start_seq = std::chrono::high_resolution_clock::now();
    std::vector<Result> seq_results(initial_points.size());

    for (size_t i = 0; i < initial_points.size(); i++) {
        seq_results[i] = newton_method(A, B, C, initial_points[i]);
    }

    auto end_seq = std::chrono::high_resolution_clock::now();
    auto seq_time = std::chrono::duration<double>(end_seq - start_seq).count();

    // Print results
    std::cout << "Results for initial points from -4 to 4:\n";
    std::cout << std::setw(10) << "x0"
              << std::setw(15) << "Root"
              << std::setw(12) << "Iterations"
              << std::setw(12) << "Converged"
              << std::setw(15) << "Reason\n";
    std::cout << std::string(64, '-') << "\n";

    for (size_t i = 0; i < seq_results.size(); i++) {
        const auto& r = seq_results[i];

        std::string conv_str = r.converged ? "yes" : "no";
        std::string reason_str = "-";
        if (!r.converged) {
            if (r.derivative_zero) {
                reason_str = "df=0";
            } else {
                reason_str = "max iterations";
            }
        }

        if (r.converged) {
            std::cout << std::setw(10) << r.initial_x
                      << std::setw(15) << std::fixed << std::setprecision(8) << r.root
                      << std::setw(12) << r.iterations
                      << std::setw(12) << conv_str
                      << std::setw(15) << reason_str << "\n";
        } else {
            std::cout << std::setw(10) << r.initial_x
                      << std::setw(15) << "--"
                      << std::setw(12) << r.iterations
                      << std::setw(12) << conv_str
                      << std::setw(15) << reason_str << "\n";
        }
    }

    std::cout << "\nSequential execution time: "
              << std::fixed << std::setprecision(6) << seq_time << " s\n\n";

    // Parallel version with different number of threads
    std::vector<int> threads_to_test = { 1, 2, 4, 8 };
    std::vector<double> parallel_times;
    std::vector<double> speedups;

    std::cout << "Performance with different number of threads:\n";
    std::cout << std::setw(10) << "Threads"
              << std::setw(15) << "Time (s)"
              << std::setw(15) << "Speedup"
              << std::setw(15) << "Efficiency\n";
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

    // Save data for plotting
    std::ofstream data_file("speedup_data.txt");
    if (data_file.is_open()) {
        data_file << "# Threads Time Speedup Linear_speedup\n";
        for (size_t i = 0; i < threads_to_test.size(); i++) {
            data_file << threads_to_test[i] << " "
                      << parallel_times[i] << " "
                      << speedups[i] << " "
                      << threads_to_test[i] << "\n";
        }
        data_file.close();
        std::cout << "\nData for the speedup plot saved to: speedup_data.txt\n";
    }

    // Generate gnuplot script
    std::ofstream gnuplot_script("plot_speedup.gp");
    if (gnuplot_script.is_open()) {
        gnuplot_script << "set terminal pngcairo size 800,600 enhanced font 'Verdana,10'\n";
        gnuplot_script << "set output 'speedup_comparison.png'\n";
        gnuplot_script << "set title 'Speedup of Newton\\'s Method with OpenMP\\nEquation: "
                       << A << "x^2 - " << B << "x = " << C << "'\n";
        gnuplot_script << "set xlabel 'Number of threads'\n";
        gnuplot_script << "set ylabel 'Speedup'\n";
        gnuplot_script << "set grid\n";
        gnuplot_script << "set key top left\n";
        gnuplot_script << "set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5\n";
        gnuplot_script << "set style line 2 lc rgb '#dd181f' lt 1 lw 2 dt 2\n";
        gnuplot_script << "plot 'speedup_data.txt' using 1:3 title 'Actual speedup' with linespoints ls 1, \\\n";
        gnuplot_script << "     'speedup_data.txt' using 1:4 title 'Linear speedup' with lines ls 2\n";
        gnuplot_script.close();

        std::cout << "Gnuplot script saved to: plot_speedup.gp\n\n";
        std::cout << "To build the plot, run:\n";
        std::cout << "gnuplot plot_speedup.gp\n";
        std::cout << "The plot will be saved as 'speedup_comparison.png'\n";
    }

    return 0;
}

// Simple main that just calls the demo function.
// If you want to use this file as a library, you can remove this main()
// and just call run_newton_openmp_demo() from another project.
int main() {
    return run_newton_openmp_demo();
}
