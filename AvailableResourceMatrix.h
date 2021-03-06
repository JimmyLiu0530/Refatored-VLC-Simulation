#ifndef AVAILABLE_RESOURCE_MATRIX_H
#define AVAILABLE_RESOURCE_MATRIX_H

#include "Matrix.h"
#include "TDMAMatrix.h"
#include "My_UE_List.h"
#include "RfDataRateMatrix.h"
#include "VlcDataRateMatrix.h"
#include "AssociationMatrix.h"
#include "HandoverEfficiencyMatrix.h"

class AvailableResourceMatrix : public Matrix
{
public:
    using Matrix::Matrix;
    void update(TDMAMatrix &TDMA_matrix);
    void allocate(int u, MyUeList &my_UE_list, RfDataRateMatrix &RF_data_rate_matrix, VlcDataRateMatrix &VLC_data_rate_matrix);
    void allocate(int u, MyUeList &my_UE_list, RfDataRateMatrix &RF_data_rate_matrix, VlcDataRateMatrix &VLC_data_rate_matrix, HandoverEfficiencyMatrix &handover_efficiency_matrix);
    double findBestAP(int u, RfDataRateMatrix &RF_data_rate_matrix, VlcDataRateMatrix &VLC_data_rate_matrix, MyUeList &my_UE_list, TDMAMatrix &TDMA_matrix, 
                        AssociationMatrix &AP_association_matrix);
    double findBestAP(int u, RfDataRateMatrix &RF_data_rate_matrix, VlcDataRateMatrix &VLC_data_rate_matrix, MyUeList &my_UE_list, TDMAMatrix &TDMA_matrix, 
                        AssociationMatrix &AP_association_matrix, HandoverEfficiencyMatrix &handover_efficiency_matrix);

private:
    int findMaxResidualRfAP();
    int findMaxResidualVlcAP();
    double estimateSatisfaction(int u, int Beta, Matrix &data_rate_matrix, MyUeList &my_UE_list, TDMAMatrix &TDMA_matrix);
    double estimateSatisfaction(int u, int Beta, Matrix &data_rate_matrix, MyUeList &my_UE_list, TDMAMatrix &TDMA_matrix, HandoverEfficiencyMatrix &handover_efficiency_matrix);
};

#endif