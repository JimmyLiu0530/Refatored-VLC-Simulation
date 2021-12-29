#ifndef BENCHMARK_LOAD_BALANCE_METHOD_H
#define BENCHMARK_LOAD_BALANCE_METHOD_H

#include "My_UE_List.h"
#include "LoadBalanceMethod.h"


class BenchmarkLoadBalanceMethod : public LoadBalanceMethod
{
private:
    virtual void DoAllocateState0(int &state,
                                std::vector<std::vector<double>> &RF_DataRate_Matrix,
                                std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                                std::vector<std::vector<int>> &AP_Association_Matrix,
                                MyUeList &my_UE_list);
    virtual void DoAllocateStateN(int &state,
                                std::vector<std::vector<double>> &RF_DataRate_Matrix,
                                std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                                std::vector<std::vector<int>> &AP_Association_Matrix,
                                std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
                                MyUeList &my_UE_list);
   

    void ApAssociateState0(std::vector<std::vector<double>> &RF_DataRate_Matrix,
                            std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                            std::vector<std::vector<int>> &AP_Association_Matrix,
                            MyUeList &my_UE_list);
    void ApAssociateStateN(std::vector<std::vector<double>> &RF_DataRate_Matrix,
                            std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                            std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
                            std::vector<std::vector<int>> &AP_Association_Matrix,
                            MyUeList &my_UE_list);
    void ResourceAllocateState0(std::vector<std::vector<double>> &RF_DataRate_Matrix,
                                std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                                MyUeList &my_UE_list,
                                std::vector<int> &beta_u,
                                std::vector<int> &served_UE_Num);
    void ResourceAllocateStateN(std::vector<std::vector<double>> &RF_DataRate_Matrix,
                                std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                                std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
                                MyUeList &my_UE_list,
                                std::vector<int> &beta_u,
                                std::vector<int> &served_UE_Num);

    void Benchmark_Update_APSelection_Result(MyUeList &my_UE_list, std::vector<int> &beta_u, std::vector<int> &served_UE_Num, std::vector<std::vector<int>> &AP_Association_Matrix);
    void Benchmark_Update_RA_Result(MyUeList &my_UE_list, std::vector<double> &AchievableRate);
};



#endif