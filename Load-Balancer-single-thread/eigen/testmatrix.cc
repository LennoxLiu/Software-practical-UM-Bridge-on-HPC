#include "matrix.cc"
#include <iostream>
#include <cassert>
#include <cmath>
#include <array>
#include <vector>
#include <complex>

#define IS_EQUAL(x, y) abs((x) - (y)) < 0.0001
#define IS_COMPLEX_EQUAL(x, y) abs(complex(x) - complex(y)) < 0.0001

int testStdVector()
{
  // define matrix
  Matrix A(4, 6, 0.);
  for (int i = 0; i < A.rows(); ++i)
    A[i][i] = 2.;
  for (int i = 0; i < A.rows() - 1; ++i)
    A[i + 1][i] = A[i][i + 1] = -1.;
  Matrix B(6, 4, 0.);
  for (int i = 0; i < B.cols(); ++i)
    B[i][i] = 2.;
  for (int i = 0; i < B.cols() - 1; ++i)
    B[i + 1][i] = B[i][i + 1] = -1.;
  // print matrix
  A.print();
  assert(A[0][0] == 2 && A[0][1] == -1 && A[1][0] == -1 && A[1][1] == 2);
  assert(A[0][5] == 0 && A[3][5] == 0);

  B.print();
  assert(B[0][0] == 2 && B[0][1] == -1 && B[1][0] == -1 && B[1][1] == 2);
  assert(B[0][3] == 0 && B[5][3] == 0);

  Matrix C(A);
  A = 2. * C; // since we may assume assume that scalars, vectors and matrices all use the same element type
  A.print();
  assert(A[0][0] == 4 && A[0][1] == -2 && A[1][0] == -2 && A[1][1] == 4);
  assert(A[0][5] == 0 && A[3][5] == 0);

  A = C * 2.;
  A.print();
  assert(A[0][0] == 4 && A[0][1] == -2 && A[1][0] == -2 && A[1][1] == 4);
  assert(A[0][5] == 0 && A[3][5] == 0);

  A = C + A;
  A.print();
  assert(A[0][0] == 6 && A[0][1] == -3 && A[1][0] == -3 && A[1][1] == 6);
  assert(A[0][5] == 0 && A[3][5] == 0);

  Matrix A2(5, 5, 0.); // A.resize(5, 5, 0.);
  for (int i = 0; i < A2.rows(); ++i)
    A2(i, i) = 2.;
  for (int i = 0; i < A2.rows() - 1; ++i)
    A2(i + 1, i) = A2(i, i + 1) = -1.;
  // define vector b
  std::vector<double> b(5);
  b[0] = b[4] = 5.;
  b[1] = b[3] = -4.;
  b[2] = 4.;
  auto x = A2 * b;
  std::cout << "A2*b = ( ";
  for (int i = 0; i < x.rows(); ++i)
    std::cout << x(i, 0) << "  ";
  std::cout << ")" << std::endl;
  assert(IS_EQUAL(x(0, 0), 14) && IS_EQUAL(x(1, 0), -17) && IS_EQUAL(x(2, 0), 16) && IS_EQUAL(x(3, 0), -17) && IS_EQUAL(x(4, 0), 14));
  std::cout << std::endl;

  auto x2 = A2.dot(b);
  std::cout << "A2*b = ( ";
  for (int i = 0; i < x2.size(); ++i)
    std::cout << x2[i] << "  ";
  std::cout << ")" << std::endl;
  assert(IS_EQUAL(x2[0], 14) && IS_EQUAL(x2[1], -17) && IS_EQUAL(x2[2], 16) && IS_EQUAL(x2[3], -17) && IS_EQUAL(x2[4], 14));
  std::cout << std::endl;

  return 0;
}

int main()
{
  assert(testStdVector() == 0);
  std::cout << "testStdVector() success." << std::endl;
}
