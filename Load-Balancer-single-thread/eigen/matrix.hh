#ifndef MATRIX_HH
#define MATRIX_HH

#include <vector>

using std::vector;

typedef class Matrix
{
public:
    Matrix();
    Matrix(const Matrix &otherM);
    Matrix(const vector<vector<double>> &oldEntries);
    Matrix(int squareSize);                         // construct a square matrix
    Matrix(int numRow, int numCol, double val = 1); // construct all 1 matrix with size numRow * numCol
    ~Matrix() {}                                    // using container, no need to free memory

    Matrix &operator=(const Matrix &otherM);
    // assign();
    // get_allocator();

    // Element access
    double &operator()(int i, int j);       // for accessing the(i, j)-th element of objects of class Matrix
    std::vector<double> &operator[](int i); // to return the i-th row of a matrix object

    // Modifiers
    void clear();
    void erase(); // clear and free the memory
    void swap(Matrix &otherM);

    // Operations
    Matrix operator+(const Matrix &otherM);
    Matrix operator-(const Matrix &otherM); // + -1*(otherM)
    Matrix operator*(double scalar);
    Matrix operator*(const Matrix &otherM);
    Matrix operator*(const vector<double> &vec);
    vector<double> dot(const vector<double>& vec) const;
    vector<double> left_dot(const vector<double>& vec) const;
    

    // template<T>
    // Matrix& operator*(std::vector<T> vec); //what is Vector? Maybe it's Matrix?

    void print(); // print matrix
    int cols() const { return numCols; };
    int rows() const { return numRows; };

private:
    int numRows;
    int numCols;

    // iterator
    // const_iterator
    // __cpp_lib_make_reverse_iteratorconst_reverse_terator
    std::vector<std::vector<double>> entries;
} Matrix;

Matrix operator*(double scalar, const Matrix &M);

#endif