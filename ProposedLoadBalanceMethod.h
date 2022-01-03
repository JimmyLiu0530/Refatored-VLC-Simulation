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
                                HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                TDMAMatrix &TDMA_Matrix,
                                MyUeList &my_UE_list);

    virtual void DoAllocateStateN(int &state,
                                RfDataRateMatrix &RF_data_rate_matrix,
                                VlcDataRateMatrix &VLC_data_rate_matrix,
                                AssociationMatrix &AP_association_matrix,
                                HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                TDMAMatrix &TDMA_matrix,
                                MyUeList &my_UE_list);
    
    void ResidualResourceAllocate(int &state,
                                RfDataRateMatrix &RF_data_rate_matrix,
                                VlcDataRateMatrix &VLC_data_rate_matrix,
                                AssociationMatrix &AP_association_matrix,
                                HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                TDMAMatrix &TDMA_matrix,
                                MyUeList &my_UE_list);

    void MST(int &state,
            RfDataRateMatrix &RF_data_rate_matrix,
            VlcDataRateMatrix &VLC_data_rate_matrix,
            AssociationMatrix &AP_association_matrix,
            HandoverEfficiencyMatrix &handover_efficiency_matrix,
            TDMAMatrix &TDMA_matrix,
            MyUeList &my_UE_list);

    void DP(int &state,
            RfDataRateMatrix &RF_data_rate_matrix,
            VlcDataRateMatrix &VLC_data_rate_matrix,
            AssociationMatrix &AP_association_matrix,
            HandoverEfficiencyMatrix &handover_efficiency_matrix,
            TDMAMatrix &TDMA_matrix,
            MyUeList &my_UE_list);

        
};



#endif