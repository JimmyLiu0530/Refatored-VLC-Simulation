#ifndef BENCHMARK_LOAD_BALANCE_METHOD_H
#define BENCHMARK_LOAD_BALANCE_METHOD_H

#include "My_UE_List.h"
#include "LoadBalanceMethod.h"


class BenchmarkLoadBalanceMethod : public LoadBalanceMethod
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
    

    void ApAssociateState0(RfDataRateMatrix &RF_data_rate_matrix,
                            VlcDataRateMatrix &VLC_data_rate_matrix,
                            AssociationMatrix &AP_association_matrix,
                            MyUeList &my_UE_list);

    void ApAssociateStateN(RfDataRateMatrix &RF_data_rate_matrix,
                            VlcDataRateMatrix &VLC_data_rate_matrix,
                            AssociationMatrix &AP_association_matrix,
                            HandoverEfficiencyMatrix &handover_efficiency_matrix,
                            MyUeList &my_UE_list);

    void ResourceAllocateState0(RfDataRateMatrix &RF_data_rate_matrix,
                                VlcDataRateMatrix &VLC_data_rate_matrix,
                                MyUeList &my_UE_list,
                                std::vector<int> &beta_u,
                                std::vector<int> &served_UE_Num);

    void ResourceAllocateStateN(RfDataRateMatrix &RF_data_rate_matrix,
                                VlcDataRateMatrix &VLC_data_rate_matrix,
                                HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                MyUeList &my_UE_list,
                                std::vector<int> &beta_u,
                                std::vector<int> &served_UE_Num);

    void Benchmark_Update_APSelection_Result(MyUeList &my_UE_list, std::vector<int> &beta_u, std::vector<int> &served_UE_Num, AssociationMatrix &AP_association_matrix);
    void Benchmark_Update_RA_Result(MyUeList &my_UE_list, std::vector<double> &AchievableRate);
};



#endif