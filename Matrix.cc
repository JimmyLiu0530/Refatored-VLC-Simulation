#include "matrix.h"
#include <vector>



Matrix::Matrix(int row_size, int col_size) : row_size(row_size), col_size(col_size) 
{
    matrix.resize(row_size, vector<double>(col_size, 0));
}


void Matrix::setElement(int i, int j, double value)
{
    matrix[i][j] = value;
}


double Matrix::getElement(int i, int j) 
{
    return matrix[i][j];
}