#ifndef PROPOSED_LOAD_BALANCE_METHOD_H
#define PROPOSED_LOAD_BALANCE_METHOD_H

#include "My_UE_List.h"
#include "LoadBalanceMethod.h"


class ProposedLoadBalanceMethod : public LoadBalanceMethod
{
private:
    virtual void DoAllocateState0(int &state,
                                RfDataRateMatrix &RF_DataRate_Matrix,
                                VlcDataRateMatrix &VLC_DataRate_Matrix,
                                AssociationMatrix &AP_Association_Matrix,
                                HandoverEfficiencyMatrix &Handover_Efficiency_Matrix,
                                TDMAMatrix &TDMA_Matrix,
                                MyUeList &my_UE_list);
    virtual void DoAllocateStateN(int &state,
                                std::vector<std::vector<double>> &RF_DataRate_Matrix,
                                std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                                std::vector<std::vector<int>> &AP_Association_Matrix,
                                std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
                                std::vector<std::vector<double>> &TDMA_Matrix,
                                MyUeList &my_UE_list);
    
    void ResidualResourceAllocate(int &state,
                                std::vector<std::vector<double>> &RF_DataRate_Matrix,
                                std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                                std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
                                std::vector<std::vector<int>> &AP_Association_Matrix,
                                std::vector<std::vector<double>> &TDMA_Matrix,
                                MyUeList &my_UE_list);

    void MST(int &state,
            std::vector<std::vector<double>> &RF_DataRate_Matrix,
            std::vector<std::vector<double>> &VLC_DataRate_Matrix,
            std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
            std::vector<std::vector<int>> &AP_Association_Matrix,
            std::vector<std::vector<double>> &TDMA_Matrix,
            MyUeList &my_UE_list);
    
    void DP(int &state,
            std::vector<std::vector<double>> &RF_DataRate_Matrix,
            std::vector<std::vector<double>> &VLC_DataRate_Matrix,
            std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
            std::vector<std::vector<int>> &AP_Association_Matrix,
            std::vector<std::vector<double>> &TDMA_Matrix,
            MyUeList &my_UE_list);

        
};



#endif