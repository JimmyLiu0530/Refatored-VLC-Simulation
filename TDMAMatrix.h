#ifndef TDMA_MATRIX_H
#define TDMA_MATRIX_H

#include "Matrix.h"
#include "My_UE_List.h"

class TDMAMatrix : public Matrix
{
public:
    using Matrix::Matrix;
    void resetToOne();
    void allocateTimeFraction(int u, int curr_AP, double max_residual, MyUeList &my_UE_list);
};

#endif