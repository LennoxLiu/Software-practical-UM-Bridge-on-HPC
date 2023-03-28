#ifndef SOLVER_HH
#define SOLVER_HH

#include "matrix.hh"
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>

template <typename Iter_T>
long double vectorNorm(Iter_T first, Iter_T last)
{
    return std::sqrt(std::inner_product(first, last, first, 0.0L));
}

std::vector<double> get_eigenvector(const Matrix &A, int iteration_step)
{
    std::vector<double> b(A.cols(), 1.0);

    for (int i = 0; i < iteration_step; ++i)
    {
        b = A.dot(b);

        auto norm_b = vectorNorm(b.begin(), b.end());

        for (auto it = b.begin(); it != b.end(); ++it)
        {
            *it = *it / norm_b;
        }
    }

    return b;
}

double get_eigenvalue(const Matrix &A, std::vector<double> &b)
{
    auto b_temp = A.left_dot(b);
    return std::inner_product(b_temp.begin(), b_temp.end(), b.begin(), 0.0) /
           std::inner_product(b.begin(), b.end(), b.begin(), 0.0);
}

void printVec(std::vector<double> &vec)
{
    std::cout << "(";
    for (auto v : vec)
    {
        std::cout << v << " ";
    }
    std::cout << ")" << std::endl;
}

void check(const Matrix &A, const std::vector<double> &b_k, const double &lambda_k)
{
    auto left = A.dot(b_k);

    std::cout << "A * b_k = ";
    printVec(left);

    std::cout << "lambda_k * b_k = (";
    for (auto it = b_k.begin(); it != b_k.end(); ++it)
    {
        std::cout << *it * lambda_k << " ";
    }
    std::cout << ")" << std::endl;
}

// int main(void)
// {
//     int col = 10;
//     Matrix A(col, col);
//     for (int i = 0; i < col; ++i)
//     {
//         A(i, i) = i;
//     }
//     auto eigenvector = get_eigenvector(A, 10000);
//     auto eigenvalue = get_eigenvalue(A, eigenvector);

//     check(A, eigenvector, eigenvalue);

//     return 0;
// }

#endif