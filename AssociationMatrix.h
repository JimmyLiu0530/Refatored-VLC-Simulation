#ifndef ASSOCIATION_MATRIX_H
#define ASSOCIATION_MATRIX_H
#include "Matrix.h"

class AssociationMatrix : public Matrix
{
public:
    using Matrix::Matrix;
    void resetToZero();
    void associate(int AP_num, int UE_num);
};

#endif