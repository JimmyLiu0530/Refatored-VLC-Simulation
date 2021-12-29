#ifndef DYNAMIC_LB_H
#define DYNAMIC_LB_H

#include "My_UE_Node.h"
#include "My_UE_List.h"s

void Benchmark_DynamicLB(
    int &state,
    NodeContainer &RF_AP_Nodes,
    NodeContainer &VLC_AP_Nodes,
    NodeContainer &UE_Nodes,
    std::vector<std::vector<double>> &RF_Channel_Gain_Matrix,
    std::vector<std::vector<double>> &VLC_Channel_Gain_Matrix,
    std::vector<std::vector<double>> &RF_SINR_Matrix,
    std::vector<std::vector<double>> &VLC_SINR_Matrix,
    std::vector<std::vector<double>> &RF_DataRate_Matrix,
    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
    std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
    std::vector<std::vector<int>> &AP_Association_Matrix,
    MyUeList &my_UE_list);

void PreCalculation(
    NodeContainer &RF_AP_Node,
    NodeContainer &VLC_AP_Nodes,
    NodeContainer &UE_Nodes,
    std::vector<std::vector<double>> &RF_Channel_Gain_Matrix,
    std::vector<std::vector<double>> &VLC_Channel_Gain_Matrix,
    std::vector<std::vector<double>> &RF_SINR_Matrix,
    std::vector<std::vector<double>> &VLC_SINR_Matrix,
    std::vector<std::vector<double>> &RF_DataRate_Matrix,
    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
    std::vector<std::vector<double>> &Handover_Efficiency_Matrix);

void Benchmark_LB_stateN(
    std::vector<std::vector<double>> &RF_DataRate_Matrix,
    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
    std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
    std::vector<std::vector<int>> &AP_Association_Matrix,
    MyUeList &my_UE_list);

void Benchmark_RSS_state0(
    std::vector<std::vector<double>> &RF_DataRate_Matrix,
    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
    std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
    std::vector<std::vector<int>> &AP_Association_Matrix,
    MyUeList &my_UE_list);

void Benchmark_Update_APSelection_Result(MyUeList &my_UE_list, std::vector<int> &beta_u, std::vector<int> &served_UE_Num, std::vector<std::vector<int>> &AP_Association_Matrix);
void Benchmark_Update_RA_Result(MyUeList &my_UE_list, std::vector<double> &AchievableRate);
#endif