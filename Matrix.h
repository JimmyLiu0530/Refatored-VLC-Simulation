#ifndef MATRIX_H
#define MATRIX_H


class Matrix 
{
protected:
    std::vector<std::vector<double>> matrix;
    int row_size;
    int col_size;

public:
    Matrix(int row_size, int col_size);
    ~Matrix();
    void setElement(int i, int j, double value);
    double getElement(int i, int j);
};


#endif