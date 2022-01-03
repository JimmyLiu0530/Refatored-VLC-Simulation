#include "AvailableResourceMatrix.h"
#include "TDMAMatrix.h"
#include "RfDataRateMatrix.h"
#include "VlcDataRateMatrix.h"
#include "AssociationMatrix.h"


void AvailableResourceMatrix::update(TDMAMatrix &TDMA_matrix)
{
    //每當一個UE挑完AP的後,A Matrix的情況都會變化，所以每次都要更新最新的情況
    for (int i = 0; i < RF_AP_Num; i++)
        setElement(0, i, TDMA_matrix.getElement(i, 0));

    for (int i = 0; i < VLC_AP_Num; i++)
        setElement(1, i, TDMA_matrix.getElement(i + RF_AP_Num, 0));

}

void AvailableResourceMatrix::allocate(int u, MyUeList &my_UE_list, RfDataRateMatrix &RF_data_rate_matrix, VlcDataRateMatrix &VLC_data_rate_matrix)
{
    for (int i = 0; i < RF_AP_Num; i++) {
        double tmp = my_UE_list.getElement(u).Get_Required_DataRate() / RF_data_rate_matrix.getElement(i, my_UE_list.getElement(u).GetID());

        setElement(0, i, (getElement(0, i) - tmp));
    }

    for (int i = 0; i < VLC_AP_Num; i++) {
        double tmp = my_UE_list.getElement(u).Get_Required_DataRate() / VLC_data_rate_matrix.getElement(i, my_UE_list.getElement(u).GetID());

        setElement(1, i, (getElement(1, i) - tmp));
    }
}

double AvailableResourceMatrix::findBestAP(int u, RfDataRateMatrix &RF_data_rate_matrix, VlcDataRateMatrix &VLC_data_rate_matrix, MyUeList &my_UE_list, TDMAMatrix &TDMA_matrix, AssociationMatrix &AP_association_matrix)
{
    int Beta_w = findMaxResidualRfAP();
    int Beta_l = findMaxResidualVlcAP();

    double potential_satisfaction_of_Beta_w = estimateSatisfaction(u, Beta_w, RF_data_rate_matrix, my_UE_list, TDMA_matrix);
    double potential_satisfaction_of_Beta_l = estimateSatisfaction(u, Beta_l, VLC_data_rate_matrix, my_UE_list, TDMA_matrix);

    int best_AP = -1;
    int max_residual = 0;
    if (potential_satisfaction_of_Beta_w > potential_satisfaction_of_Beta_l) {
        best_AP = Beta_w;
        max_residual = getElement(0, Beta_w);
    }
    else {
        best_AP = Beta_l;
        max_residual = getElement(1, Beta_l);
    }

    AP_association_matrix.associate(best_AP, my_UE_list.getElement(u).GetID());
    my_UE_list.getElement(u).Set_Now_Associated_AP(best_AP);

    return max_residual;
}



int AvailableResourceMatrix::findMaxResidualRfAP()
{
    //Beta_w 記錄剩餘資源最大的RF AP
    int Beta_w = 0;
        
    //記錄該資源剩餘量
    double max_RF_residual = getElement(0, 0);
        
    //遍歷RF AP更新最佳選擇
    for(int i = 0 ; i < RF_AP_Num ; i++) {
        double curr_available_resource = getElement(0, i);
        if(curr_available_resource > max_RF_residual) {
            Beta_w = i;
            max_RF_residual = curr_available_resource;
        }
    }

    return Beta_w;   
}

int AvailableResourceMatrix::findMaxResidualVlcAP()
{
    int Beta_l = 0;
            
    //記錄該資源剩餘量
    double max_VLC_residual = getElement(1, 0);
        
    //遍歷VLC AP更新最佳選擇
    for(int i = 0 ; i < VLC_AP_Num ; i++) {
        double curr_available_resource = getElement(1, i);
        if(curr_available_resource > max_VLC_residual) {
            Beta_l = i;
            max_VLC_residual = curr_available_resource;
        }
    }

    return Beta_l;
}

double AvailableResourceMatrix::estimateSatisfaction(int u, int Beta_w, RfDataRateMatrix &RF_data_rate_matrix, MyUeList &my_UE_list, TDMAMatrix &TDMA_matrix)
{
    double potential_throughput = TDMA_matrix.getElement(Beta_w, 0) * RF_data_rate_matrix.getElement(Beta_w, my_UE_list.getElement(u).GetID());
    double potential_satisfaction = potential_throughput / my_UE_list.getElement(u).Get_Required_DataRate();

    return ((potential_satisfaction > 1) ? 1 : potential_satisfaction);
}

double AvailableResourceMatrix::estimateSatisfaction(int u, int Beta_l, VlcDataRateMatrix &VLC_data_rate_matrix, MyUeList &my_UE_list, TDMAMatrix &TDMA_matrix)
{
    double potential_throughput = TDMA_matrix.getElement(Beta_l, 0) * VLC_data_rate_matrix.getElement(Beta_l, my_UE_list.getElement(u).GetID());
    double potential_satisfaction = potential_throughput / my_UE_list.getElement(u).Get_Required_DataRate();

    return ((potential_satisfaction > 1) ? 1 : potential_satisfaction);
}