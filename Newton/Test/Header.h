#ifndef NEWTON_METHOD_H
#define NEWTON_METHOD_H

// ��������� ��� �������� �����������
struct Result {
    double initial_x;
    double root;
    int iterations;
    bool converged;

    // �������� ������������
    Result& operator=(const Result& other) {
        if (this == &other) return *this; // ������ �� ����������������
        initial_x = other.initial_x;
        root = other.root;
        iterations = other.iterations;
        converged = other.converged;
        return *this;
    }
};

// ������� f(x) = Ax^2 - Bx - C
double f(double A, double B, double C, double x);

// ����������� f'(x) = 2Ax - B
double df(double A, double B, double C, double x);

// ����� ������� (� ����������� �� ���������)
Result newton_method(double A, double B, double C, double initial_x,
    double tolerance = 1e-10, int max_iterations = 1000);

#endif  // NEWTON_METHOD_H
