#include "AssociationMatrix.h"
#include "global_environment.h"


void AssociationMatrix::resetToZero()
{
    for(int i = 0 ; i < RF_AP_Num + VLC_AP_Num ; i++)
        for(int j = 0 ; j < UE_Num ; j++)
            setElement(i ,j , 0); 
}


void AssociationMatrix::associate(int AP_num, int UE_num)
{
    setElement(AP_num, UE_num, 1);
}