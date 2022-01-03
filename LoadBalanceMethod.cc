#include <algorithm>
#include <iomanip> 

#include "LoadBalanceMethod.h"
#include "global_environment.h"
#include "Channel.h"



void LoadBalanceMethod::execute(int &state,
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
            MyUeList &my_UE_list) 
{
    precalculate(RF_AP_Nodes, VLC_AP_Nodes, UE_Nodes,
                   RF_Channel_Gain_Matrix, VLC_Channel_Gain_Matrix,
                   RF_SINR_Matrix, VLC_SINR_Matrix,
                   RF_DataRate_Matrix, VLC_DataRate_Matrix,
                   Handover_Efficiency_Matrix);

    if (state == 0)
        DoAllocateState0(state, 
                        RF_DataRate_Matrix,
                        VLC_DataRate_Matrix,
                        AP_Association_Matrix,
                        TDMA_Matrix,
                        my_UE_list);
        
    else
        DoAllocateStateN(state, 
                        RF_DataRate_Matrix,
                        VLC_DataRate_Matrix,
                        AP_Association_Matrix,
                        Handover_Efficiency_Matrix,
                        TDMA_Matrix,
                        my_UE_list);
        

    std::cout << std::setiosflags(std::ios::fixed);


    std::sort(my_UE_list.begin(), my_UE_list.end(), [](My_UE_Node a, My_UE_Node b)
         { return a.Get_Required_DataRate() > b.Get_Required_DataRate(); });

    state++;
}


void LoadBalanceMethod::precalculate(NodeContainer &RF_AP_Node,
                                    NodeContainer &VLC_AP_Nodes,
                                    NodeContainer &UE_Nodes,
                                    std::vector<std::vector<double>> &RF_Channel_Gain_Matrix,
                                    std::vector<std::vector<double>> &VLC_Channel_Gain_Matrix,
                                    std::vector<std::vector<double>> &RF_SINR_Matrix,
                                    std::vector<std::vector<double>> &VLC_SINR_Matrix,
                                    std::vector<std::vector<double>> &RF_DataRate_Matrix,
                                    std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                                    std::vector<std::vector<double>> &Handover_Efficiency_Matrix)
{
    /** Algo1 , Line3 **/
	//算RF/VLC的Channel gain
	Calculate_RF_Channel_Gain_Matrix(RF_AP_Node, UE_Nodes, RF_Channel_Gain_Matrix);
	Calculate_VLC_Channel_Gain_Matrix(VLC_AP_Nodes, UE_Nodes, VLC_Channel_Gain_Matrix);

#if DEBUG_MODE
	print_RF_Channel_Gain_Matrix(RF_Channel_Gain_Matrix);
	print_VLC_Channel_Gain_Matrix(VLC_Channel_Gain_Matrix);
#endif

	//算RF/VLC的SINR
	Calculate_RF_SINR_Matrix(RF_Channel_Gain_Matrix, RF_SINR_Matrix);
	Calculate_VLC_SINR_Matrix(VLC_Channel_Gain_Matrix, VLC_SINR_Matrix);

#if DEBUG_MODE
	print_RF_SINR_Matrix(RF_SINR_Matrix);
	print_VLC_SINR_Matrix(VLC_SINR_Matrix);
#endif

	/** Algo1 , Line4 **/
	//算RF/VLC的DataRate
	Calculate_RF_DataRate_Matrix(RF_SINR_Matrix, RF_DataRate_Matrix);
	Calculate_VLC_DataRate_Matrix(VLC_SINR_Matrix, VLC_DataRate_Matrix);

#if DEBUG_MODE
	print_RF_DataRate_Matrix(RF_DataRate_Matrix);
	print_VLC_DataRate_Matrix(VLC_DataRate_Matrix);
#endif

	/** Algo1 , Line5 **/
	//算Handover_Efficiency_Matrix
	Calculate_Handover_Efficiency_Matrix(Handover_Efficiency_Matrix);

#if DEBUG_MODE
	print_Handover_Efficiency_Matrix(Handover_Efficiency_Matrix);
#endif

}

