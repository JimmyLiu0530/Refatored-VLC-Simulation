#include "TDMAMatrix.h"
#include "global_environment.h"




void TDMAMatrix::resetToOne()
{
    for (int i = 0; i < RF_AP_Num + VLC_AP_Num; i++) {
        //forall i ,ρ(i,0) = 1 代表每個AP目前都還沒被人連(故剩餘時間比例 = 1)
        setElement(i ,0 , 1);  

        //目前都還沒有分時間
        for (int j = 1 ; j < UE_Num +1 ; j++)
            setElement(i ,j , 0); 
    }
}

void TDMAMatrix::allocateTimeFraction(int u, int curr_AP, double max_residual, MyUeList &my_UE_list)
{
    double allocated_resource = getElement(curr_AP, 0) - max_residual;

    setElement(curr_AP, my_UE_list.getElement(u).GetID()+1, allocated_resource);
    my_UE_list.getElement(u).Set_Time_Fraction(allocated_resource);
    
    // update remaining resource of current AP
    double remaining_resource = getElement(curr_AP, 0) - allocated_resource;
    setElement(curr_AP, 0, remaining_resource);
}