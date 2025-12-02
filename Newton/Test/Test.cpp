#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>
#include "Header.h"  

struct TestCase {
    double A;
    double B;
    double C;
    double expected_root;
    double initial_guess;
    const char* description;
};

// Тестирование метода Ньютона на одном уравнении
void test_method(const TestCase& tc) {
    Result result = newton_method(tc.A, tc.B, tc.C,
                                 tc.initial_guess,
                                 1e-8,      // точность
                                 1000);     // максимум итераций

    std::cout << "Тест: " << tc.description << "\n";
    std::cout << "Уравнение: "
              << tc.A << "x^2 - " << tc.B << "x = " << tc.C << "\n";
    std::cout << "Начальное приближение x0 = " << tc.initial_guess << "\n";

    if (result.converged) {
        std::cout << "Ожидаемый корень: " << tc.expected_root << "\n";
        std::cout << "Найденный корень: " << result.root << "\n";

        // Проверка, что найденный корень близок к ожидаемому
        assert(std::fabs(result.root - tc.expected_root) < 1e-4);
        std::cout << "Тест пройден успешно.\n\n";
    } else {
        std::cout << "Метод не сошелся для данного уравнения.\n\n";
    }
}

// Запуск всех тестов
void run_tests() {
    std::vector<TestCase> tests = {
        // Тест 1: x^2 - 3x = -2  (корни 1 и 2)
        { 1.0, 3.0, -2.0, 1.0,  -4.0, "Уравнение x^2 - 3x = -2 (корень 1)" },

        // Тест 2: x^2 - 2x = 8  (корни -2 и 4)
        { 1.0, 2.0,  8.0, -2.0, -4.0, "Уравнение x^2 - 2x = 8 (корень -2)" },

        // Тест 3: x^2 - 2x = 3  (корни -1 и 3)
        { 1.0, 2.0,  3.0, -1.0, -4.0, "Уравнение x^2 - 2x = 3 (корень -1)" },

        // Тест 4: x^2 + 4x = 3  (корни -3 и -1)
        { 1.0, -4.0, -3.0, -3.0, -4.0, "Уравнение x^2 + 4x = 3 (корень -3)" },

        // Тест 5: x^2 - 4x = -4 (двойной корень 2)
        { 1.0, 4.0, -4.0,  2.0, -4.0, "Уравнение x^2 - 4x = -4 (двойной корень 2)" }
    };

    for (const auto& tc : tests) {
        test_method(tc);
    }

    std::cout << "Все тесты завершены.\n";
}

int main() {
    run_tests();
    return 0;
}
