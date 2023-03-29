#include "matrix.hh"
#include <iostream>
#include <cassert>

using std::cerr;
using std::cout;
using std::endl;

Matrix::Matrix()
{
    numRows = 0;
    numCols = 0;

    entries.push_back(vector<double>(0)); // entries should not be empty
}
Matrix::Matrix(const Matrix &otherM)
{
    numRows = otherM.numRows;
    numCols = otherM.numCols;

    entries.assign(otherM.entries.begin(), otherM.entries.end());
}
Matrix::Matrix(const vector<vector<double>> &oldEntries)
{
    entries.assign(oldEntries.begin(), oldEntries.end());

    numRows = entries.size();
    numCols = entries[0].size();
}
Matrix::Matrix(int squareSize)
{ // construct a square identity matrix
    numRows = squareSize;
    numCols = squareSize;

    vector<double> tempVec;
    tempVec.reserve(squareSize);
    for (int row = 0; row < squareSize; ++row)
    {
        tempVec.clear(); // Leaves the capacity() of the vector unchanged
        for (int col = 0; col < squareSize; ++col)
        {
            tempVec.push_back(row == col);
        }
        entries.push_back(tempVec);
    }
}

Matrix::Matrix(int numRow, int numCol, double val)
{ // construct null matrix with size numRow * numCol
    numRows = numRow;
    numCols = numCol;

    vector<double> tempVec(numCols, val);
    for (int row = 0; row < numRows; ++row)
    {
        entries.push_back(tempVec);
    }
}

Matrix &Matrix::operator=(const Matrix &otherM)
{
    numRows = otherM.numRows;
    numCols = otherM.numCols;

    entries.assign(otherM.entries.begin(), otherM.entries.end());

    return *this;
}

double &Matrix::operator()(int i, int j)
{ // for accessing the(i, j)-th element of objects of class Matrix
    if (i >= numRows || i < 0 || j >= numCols || j < 0)
    {
        cerr << "Accessing invalid index:(" << i << "," << j << ")" << endl;
        throw std::invalid_argument( "received invalid index" );
    }
    // assert(0 <= i && i < numRows && 0 < j && j < numCols);

    return entries[i][j];
}

std::vector<double> &Matrix::operator[](int i)
{ // to return the i-th row of a matrix object
    if (i < 0 || i >= numRows)
    {
        cerr << "Accessing invalid index:(" << i << ")" << endl;
        throw std::invalid_argument("received invalid index");
    }

    return entries[i];
}

void Matrix::clear()
{
    numCols = 0;
    numRows = 0;

    entries.clear();
}
void Matrix::erase()
{
    numCols = 0;
    numRows = 0;

    entries.clear();
    entries.shrink_to_fit(); // free the memory
}
void Matrix::swap(Matrix &otherM)
{
    int tempCols = numCols;
    int tempRows = numRows;

    numCols = otherM.numCols;
    numRows = otherM.numRows;

    otherM.numCols = tempCols;
    otherM.numRows = tempRows;

    entries.swap(otherM.entries);
}

Matrix Matrix::operator+(const Matrix& otherM)
{
    if (numRows != otherM.numRows || numCols != otherM.numCols)
    {
        cerr << "Invalid matrix using operator+" << endl;
        throw std::invalid_argument("received invalid matrix shape");
    }

    Matrix resultM(entries);

    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            resultM.entries[row][col] += otherM.entries[row][col];
        }
    }

    return resultM;
}
Matrix Matrix::operator-(const Matrix &otherM)
{ // + -1*(otherM)
    Matrix resultM;
    resultM = *this + (-1) * otherM;

    return resultM;
}
Matrix Matrix::operator*(double scalar)
{
    Matrix resultM(*this);

    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            resultM.entries[row][col] *= scalar;
        }
    }

    return resultM;
}

Matrix operator*(double scalar, const Matrix &M)
{
    Matrix resultM(M);

    resultM = resultM * scalar;

    return resultM;
}

Matrix Matrix::operator*(const Matrix &otherM)
{
    if (numRows != otherM.numCols || numCols != otherM.numRows)
    {
        cerr << "Invalid matrix size in operator *" << endl;
        exit(EXIT_FAILURE);
    }

    Matrix resultM(numRows, otherM.numCols);
    double tempSum = 0;
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < otherM.numCols; ++col)
        {
            tempSum = 0;
            for (int i = 0; i < numCols; ++i)
            {
                tempSum += entries[row][i] * otherM.entries[i][col];
            }
            resultM(row, col) = tempSum;
        }
    }

    return resultM;
}
Matrix Matrix::operator*(const vector<double>& vec)
{
    if (int(vec.size()) != numCols)
    {
        cerr << "Invalid vector size" << endl;
        throw std::invalid_argument("Invalid vector size.");
    }

    Matrix resultM(numRows, 1);
    double tempSum = 0;
    for (int row = 0; row < numRows; ++row)
    {
        tempSum = 0;
        for (int i = 0; i < numCols; ++i)
        {
            tempSum += entries[row][i] * vec[i];
        }
        resultM(row, 0) = tempSum;
    }

    return resultM;
}

vector<double> Matrix::dot(const vector<double>& vec) const
{
    if (int(vec.size()) != numCols)
    {
        cerr << "Invalid vector size" << endl;
        throw std::invalid_argument("Invalid vector size.");
    }

    vector<double> resultVec(numRows);
    double tempSum = 0;
    for (int row = 0; row < numRows; ++row)
    {
        tempSum = 0;
        for (int i = 0; i < numCols; ++i)
        {
            tempSum += entries[row][i] * vec[i];
        }
        resultVec[row] = tempSum;
    }

    return resultVec;
}

vector<double> Matrix::left_dot(const vector<double>& vec) const{
    if (int(vec.size()) != numRows)
    {
        cerr << "Invalid vector size" << endl;
        throw std::invalid_argument("Invalid vector size.");
    }

    vector<double> resultVec(numCols);
    double tempSum = 0;
    for (int col = 0; col < numCols; ++col)
    {
        tempSum = 0;
        for (int i = 0; i < numRows; ++i)
        {
            tempSum += entries[i][col] * vec[col];
        }
        resultVec[col] = tempSum;
    }

    return resultVec;
}

void Matrix::print()
{ // print matrix
    cout << "Matrix:" << endl;

    for (auto row : entries)
    {
        for (auto item : row)
        {
            cout << item << " ";
        }
        cout << ";" << endl;
    }

    cout << endl;
}