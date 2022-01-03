#ifndef LOAD_BALANCE_METHOD_H
#define LOAD_BALANCE_METHOD_H

#include <vector>
#include "My_UE_List.h"
#include "RfDataRateMatrix.h"
#include "VlcDataRateMatrix.h"
#include "AssociationMatrix.h"
#include "HandoverEfficiencyMatrix.h"
#include "TDMAMatrix.h"

class LoadBalanceMethod 
{
public:
    void execute(int &state,
                NodeContainer &RF_AP_Nodes,
                NodeContainer &VLC_AP_Nodes,
                NodeContainer &UE_Nodes,
                std::vector<std::vector<double>> &RF_Channel_Gain_Matrix,
                std::vector<std::vector<double>> &VLC_Channel_Gain_Matrix,
                std::vector<std::vector<double>> &RF_SINR_Matrix,
                std::vector<std::vector<double>> &VLC_SINR_Matrix,
                RfDataRateMatrix &RF_data_rate_matrix,
                VlcDataRateMatrix &VLC_data_rate_matrix,
                HandoverEfficiencyMatrix &handover_efficiency_matrix,
                AssociationMatrix &AP_association_matrix,
                TDMAMatrix &TDMA_matrix,
                MyUeList &my_UE_list);
    
private:
    void precalculate(NodeContainer &RF_AP_Node,
                        NodeContainer &VLC_AP_Nodes,
                        NodeContainer &UE_Nodes,
                        std::vector<std::vector<double>> &RF_Channel_Gain_Matrix,
                        std::vector<std::vector<double>> &VLC_Channel_Gain_Matrix,
                        std::vector<std::vector<double>> &RF_SINR_Matrix,
                        std::vector<std::vector<double>> &VLC_SINR_Matrix,
                        std::vector<std::vector<double>> &RF_DataRate_Matrix,
                        std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                        std::vector<std::vector<double>> &Handover_Efficiency_Matrix);

    virtual void DoAllocateState0(int &state,
                                    RfDataRateMatrix &RF_DataRate_Matrix,
                                    VlcDataRateMatrix &VLC_DataRate_Matrix,
                                    AssociationMatrix &AP_Association_Matrix,
                                    HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                    TDMAMatrix &TDMA_Matrix,
                                    MyUeList &my_UE_list) = 0;
                                
    virtual void DoAllocateStateN(int &state,
                                    RfDataRateMatrix &RF_DataRate_Matrix,
                                    VlcDataRateMatrix &VLC_DataRate_Matrix,
                                    AssociationMatrix &AP_Association_Matrix,
                                    HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                    TDMAMatrix &TDMA_Matrix,
                                    MyUeList &my_UE_list) = 0;
};

#endif