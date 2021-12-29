#ifndef PROPOSED_METHOD_H
#define PROPOSED_METHOD_H

#include "My_UE_Node.h"
#include "MY_UE_List.h"

void Proposed_DynamicLB(
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
    std::vector<std::vector<double>> &TDMA_Matrix,
    MyUeList &my_UE_list);

void Proposed_LB_state0(
    int &state,
    std::vector<std::vector<double>> &RF_DataRate_Matrix,
    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
    std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
    std::vector<std::vector<int>> &AP_Association_Matrix,
    std::vector<std::vector<double>> &TDMA_Matrix,
    MyUeList &my_UE_list);

void Proposed_LB_stateN(
    int &state,
    std::vector<std::vector<double>> &RF_DataRate_Matrix,
    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
    std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
    std::vector<std::vector<int>> &AP_Association_Matrix,
    std::vector<std::vector<double>> &TDMA_Matrix,
    MyUeList &my_UE_list);

void MST(
    int &state,
    std::vector<std::vector<double>> &RF_DataRate_Matrix,
    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
    std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
    std::vector<std::vector<int>> &AP_Association_Matrix,
    std::vector<std::vector<double>> &TDMA_Matrix,
    MyUeList &my_UE_list);

void DP(
    int &state,
    std::vector<std::vector<double>> &RF_DataRate_Matrix,
    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
    std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
    std::vector<std::vector<int>> &AP_Association_Matrix,
    std::vector<std::vector<double>> &TDMA_Matrix,
    MyUeList &my_UE_list);

#endif