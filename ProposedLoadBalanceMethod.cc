#include <algorithm>
#include <map>

#include "ProposedLoadBalanceMethod.h"
#include "TDMAMatrix.h"
#include "AssociationMatrix.h"
#include "AvailableResourceMatrix.h"
#include "HandoverEfficiencyMatrix.h"



void ProposedLoadBalanceMethod::DoAllocateState0(int &state,
                                                RfDataRateMatrix &RF_data_rate_matrix,
                                                VlcDataRateMatrix &VLC_data_rate_matrix,
                                                AssociationMatrix &AP_association_matrix,
                                                HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                                TDMAMatrix &TDMA_matrix,
                                                MyUeList &my_UE_list) 
{
    //clear AP association matrix （新一輪的開始要重新算）
    AP_association_matrix.resetToZero();

    //clear TDMA matrix （新一輪的開始要重新算）
    TDMA_matrix.resetToOne();

    //並根據Required datarate做大到小排序
    //爲了讓demand大的先挑
    std::sort(my_UE_list.begin(), my_UE_list.end(), [](My_UE_Node a, My_UE_Node b)
         { return a.Get_Required_DataRate() > b.Get_Required_DataRate(); });


    ////////////////////////////////
    ////////////////////////////////
    ////                        ////
    ////      stage 1 HDFDA     ////
    ////                        ////
    ////////////////////////////////
    ////////////////////////////////

    //初步先配置盡可能滿足demand的量
    //若ap有多餘資源則stage2再分配

    /* AP selection Phase */
    //照demand 大->小順序,給每個UE挑
    for (int u = 0; u < my_UE_list.getSize(); u++)
    {

        //Avaliable_Resource_Matrix:即我論文提到所謂"max A-Q"的A Matrix
        AvailableResourceMatrix available_resource_matrix(2, std::max(RF_AP_Num, VLC_AP_Num));

        //每當一個UE挑完AP的後,A Matrix的情況都會變化，所以每次都要更新最新的情況
        available_resource_matrix.update(TDMA_matrix);

        /** 計算A-Q 這裏我們用A matrix扣掉相對應的Q **/
        //其意義：如果UE u 連到AP i且分得恰可滿足demand的資源後, AP i還會剩下多少residual resource
        //Note : 注意sort過後的index u未必是原本的UE Node ID
        available_resource_matrix.allocate(u, my_UE_list, RF_data_rate_matrix, VLC_data_rate_matrix);
    
        //接下來要選max A-Q的AP，因爲我剩的資源越多，越有機會服務更多人
        double max_residual = 0;
        max_residual = available_resource_matrix.findBestAP(u, RF_data_rate_matrix, VLC_data_rate_matrix, my_UE_list, TDMA_matrix, AP_association_matrix);


        /* Resource Allocation Phase */
        //先盡可能配置滿足其demand的資源給UE u
        //如果無法滿足demand，則chosen ap剩的全給ue u
        
        //如果剩餘資源量爲負，則代表無法滿足ue u的demand.故剩下的資源全給UE u,導致分配後剩餘資源 = 0
        if (max_residual < 0)
            max_residual = 0;

        int curr_AP = my_UE_list.getElement(u).Get_Now_Associated_AP();
        TDMA_matrix.allocateTimeFraction(u, curr_AP, max_residual, my_UE_list);
    }

    ////////////////////////////////
    ////////////////////////////////
    ////                        ////
    ////    stage 2   RRA開始    ////
    ////                        ////
    ////////////////////////////////
    ////////////////////////////////
    ResidualResourceAllocate(state, 
                            RF_data_rate_matrix,
                            VLC_data_rate_matrix,
                            AP_association_matrix,
                            handover_efficiency_matrix,
                            TDMA_matrix,
                            my_UE_list);
}

void ProposedLoadBalanceMethod::DoAllocateStateN(int &state,
                                                RfDataRateMatrix &RF_data_rate_matrix,
                                                VlcDataRateMatrix &VLC_data_rate_matrix,
                                                AssociationMatrix &AP_association_matrix,
                                                HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                                TDMAMatrix &TDMA_matrix,
                                                MyUeList &my_UE_list) 
{
    //clear AP association matrix （新一輪的開始要重新算）
    AP_association_matrix.resetToZero();

    //clear TDMA matrix （新一輪的開始要重新算）
    TDMA_matrix.resetToOne();

    //並根據Required datarate做大到小排序
    //爲了讓demand大的先挑
    std::sort(my_UE_list.begin(), my_UE_list.end(), [](My_UE_Node a, My_UE_Node b)
         { return a.Get_Required_DataRate() > b.Get_Required_DataRate(); });

    ////////////////////////////////
    ////////////////////////////////
    ////                        ////
    ////   stage 1    HDFDA     ////
    ////                        ////
    ////////////////////////////////
    ////////////////////////////////

    //初步先配置盡可能滿足demand的量
    //若ap有多餘資源則stage2再分配

    /* AP selection Phase */

    //照demand 大->小順序,給每個UE挑
    for (int u = 0; u < my_UE_list.getSize(); u++)
    {

        //先copy每個AP還剩下多少時間比例可以分給別人
        AvailableResourceMatrix available_resource_matrix(2, std::max(RF_AP_Num, VLC_AP_Num));
        available_resource_matrix.update(TDMA_matrix);

        //計算如果UE u 連到AP i且分得恰可滿足demand的資源後, AP i還會剩下多少residual resource
        //Note : 注意sort過後的index u未必是原本的UE Node ID
        //Note2 : 現在要考慮handover
        available_resource_matrix.allocate(u, my_UE_list, RF_data_rate_matrix, VLC_data_rate_matrix, handover_efficiency_matrix);

        //接下來要選max A-Q的AP，因爲我剩的資源越多，越有機會服務更多人
        double max_residual = 0;
        max_residual = available_resource_matrix.findBestAP(u, RF_data_rate_matrix, VLC_data_rate_matrix, my_UE_list, TDMA_matrix, AP_association_matrix, handover_efficiency_matrix);


        /* Resource Allocation Phase */
        //先盡可能配置滿足其demand的資源給ue u
        //如果無法滿足demand，則chosen ap剩的全給ue u

        //如果剩餘資源量爲負，則代表無法滿足ue u的demand.故剩下的資源全給ue u,導致分配後剩餘資源 = 0
        if (max_residual < 0)
            max_residual = 0;

        int curr_AP = my_UE_list.getElement(u).Get_Now_Associated_AP();
        TDMA_matrix.allocateTimeFraction(u, curr_AP, max_residual, my_UE_list);
    }

    ////////////////////////////////
    ////////////////////////////////
    ////                        ////
    ////    stage 2   RRA開始    ////
    ////                        ////
    ////////////////////////////////
    ////////////////////////////////
    ResidualResourceAllocate(state, 
                            RF_data_rate_matrix,
                            VLC_data_rate_matrix,
                            AP_association_matrix,
                            handover_efficiency_matrix,
                            TDMA_matrix,
                            my_UE_list);
}


void ResidualResourceAllocate(int &state,
                                RfDataRateMatrix &RF_data_rate_matrix,
                                VlcDataRateMatrix &VLC_data_rate_matrix,
                                AssociationMatrix &AP_association_matrix,
                                HandoverEfficiencyMatrix &handover_efficiency_matrix,
                                TDMAMatrix &TDMA_matrix,
                                MyUeList &my_UE_list) 
{
    //此階段要做的事：對於尚有資源未分配完的AP，將其資源分配給底下的UE，不要留下冗餘資源
    //有2種分配方法：
    //1. maximize throughtput : 把剩餘資源全部分給datarate最快的UE，目的是想要衝throughput
    //2. demand propotional ： 剩餘資源依照demand的比例來分,demand大的人分得的throughput多一點

    //先將UElist 依照NodeID 小到大 sort
    //這樣是爲了for loop的index和Matrix的index對應，例如：index u = k 即代表 NodeID = k
    std::sort(my_UE_list.begin(), my_UE_list.end(), [](My_UE_Node a, My_UE_Node b)
         { return a.GetID() < b.GetID(); });

//用參數 RESIDUAL_RA_METHOD 來控制要採用何種分配方法

//1. maximize throughtput(MST)
#if (RESIDUAL_RA_METHOD == 1)
    MST(state, RF_DataRate_Matrix, VLC_DataRate_Matrix, Handover_Efficiency_Matrix, AP_Association_Matrix, TDMA_Matrix, my_UE_list);
#endif

//2. demand proportional(DP)
#if (RESIDUAL_RA_METHOD == 2)
    DP(state, RF_DataRate_Matrix, VLC_DataRate_Matrix, Handover_Efficiency_Matrix, AP_Association_Matrix, TDMA_Matrix, my_UE_list);
#endif

#if DEBUG_MODE
    std::cout << "TDMA Matrix after stage 2" << std::endl;
    print_TDMA_Matrix(TDMA_Matrix);
#endif
}

void MST(int &state,
        RfDataRateMatrix &RF_data_rate_matrix,
        VlcDataRateMatrix &VLC_data_rate_matrix,
        AssociationMatrix &AP_association_matrix,
        HandoverEfficiencyMatrix &handover_efficiency_matrix,
        TDMAMatrix &TDMA_matrix,
        MyUeList &my_UE_list)
{

    //檢查所有WiFi AP
    for (int i = 0; i < RF_AP_Num; i++)
    {

        //若有資源未分配完
        if (TDMA_matrix.getElement(i, 0) > 0)
        {

            //記錄有最大速度的UE
            int best_rate_user = -1;

            //記錄best_rate_user的速度
            double best_rate = 0;

            //檢查所有UE
            for (int ue = 0; ue < UE_Num; ue++)
            {

                //若AP i有連到這個ue
                if ((int)AP_association_matrix.getElement(i, ue) == 1)
                {
                    //初始state不必考慮handover
                    if (state == 0)
                    {
                        //則要檢查其速度是否比之前的best_rate_user高，若有利則更新
                        double curr_data_rate = RF_data_rate_matrix.getElement(i, ue);
                        if (curr_data_rate > best_rate)
                        {
                            best_rate_user = ue;
                            best_rate = curr_data_rate;
                        }
                    }
                    //非初始state則需要多考慮handover
                    else
                    {
                        int prev_AP = my_UE_list.getElement(ue).Get_Prev_Associated_AP();
                        double curr_data_rate = handover_efficiency_matrix.getElement(prev_AP, i) * RF_data_rate_matrix.getElement(i, ue);
                        if (curr_data_rate > best_rate)
                        {
                            best_rate_user = ue;
                            best_rate = curr_data_rate;
                        }
                    }
                }
            }

            //若的確存在這種user才做
            //Note : 因爲可能這個ap不服務任何人，這樣best_rate_user就會是 -1，但是實際上不存在user -1，如需要防止這樣情況發生
            if (best_rate_user > -1)
            {
                //剩下的資源全數分給best_rate_user
                //ue best_rate_user 對應到 在TDMA_Matrix的index是 best_rate_user+1，因爲TDMA_Matrix[i][0]另作它用，TDMA_Matrix[i][1]才是屬於 my_UE_list[0]
                double data_rate = TDMA_matrix.getElement(i, 0) + TDMA_matrix.getElement(i, best_rate_user + 1);
                TDMA_matrix.setElement(i, best_rate_user + 1, data_rate);

                //在myUElist[best_rate_user]中更新分得時間比例
                //Note : 此時的值即是最終值
                my_UE_list.getElement(best_rate_user).Set_Time_Fraction(TDMA_matrix.getElement(i, best_rate_user + 1));
                
                //分完就沒了
                TDMA_matrix.setElement(i, 0, 0);
            }
        }
    }

    //同理，檢查所有LiFi AP
    for (int i = 0; i < VLC_AP_Num; i++)
    {

        //若有資源未分配完
        if (TDMA_matrix.getElement(i + RF_AP_Num, 0) > 0)
        {

            //記錄有最大速度的UE
            int best_rate_user = -1;

            //記錄best_rate_user的速度
            double best_rate = 0;

            //檢查所有UE
            for (int ue = 0; ue < UE_Num; ue++)
            {

                //若AP i有連到這個ue
                if ((int)AP_association_matrix.getElement(i + RF_AP_Num, ue) == 1)
                {

                    if (state == 0)
                    {
                        //則要檢查其速度是否比之前的best_rate_user高，若有利則更新
                        double curr_data_rate = VLC_data_rate_matrix.getElement(i, ue);
                        if (curr_data_rate > best_rate)
                        {
                            best_rate_user = ue;
                            best_rate = curr_data_rate;
                        }
                    }
                    else
                    {
                        int prev_AP = my_UE_list.getElement(ue).Get_Prev_Associated_AP();
                        double curr_data_rate = handover_efficiency_matrix.getElement(prev_AP, i + RF_AP_Num) * VLC_data_rate_matrix.getElement(i, ue);
                        if (curr_data_rate > best_rate)
                        {
                            best_rate_user = ue;
                            best_rate = curr_data_rate;
                        }
                    }
                }
            }

            //若的確存在這種user才做
            //Note : 因爲可能這個ap不服務任何人，這樣best_rate_user就會是-1，但是實際上不存在user -1，如需要防止這樣情況發生
            if (best_rate_user > -1)
            {
                //剩下的資源全數分給best_rate_user
                double data_rate = TDMA_matrix.getElement(i + RF_AP_Num, 0) + TDMA_matrix.getElement(i + RF_AP_Num, best_rate_user + 1);
                TDMA_matrix.setElement(i + RF_AP_Num, best_rate_user + 1, data_rate);

                //在myUElist[best_rate_user]中更新分得時間比例
                //Note : 此時的值即是最終值
                my_UE_list.getElement(best_rate_user).Set_Time_Fraction(TDMA_matrix.get(i + RF_AP_Num, best_rate_user + 1));

                //分完就沒了
                TDMA_matrix.setElement(i + RF_AP_Num, 0, 0);
            }
        }
    }
}
/*
    根據demand比例分配剩餘資源
    例子可以看我論文
*/

void DP(int &state,
        RfDataRateMatrix &RF_data_rate_matrix,
        VlcDataRateMatrix &VLC_data_rate_matrix,
        AssociationMatrix &AP_association_matrix,
        HandoverEfficiencyMatrix &handover_efficiency_matrix,
        TDMAMatrix &TDMA_matrix,
        MyUeList &my_UE_list)
{

    //檢查所有WiFi AP
    for (int i = 0; i < RF_AP_Num; i++)
    {

        //若有資源未分配完
        if (TDMA_matrix.getElement(i, 0) > 0)
        {

            //以demand/effective data rate爲權重分配剩餘資原
            //記錄分母 = 權重之總和
            double Denominator = 0.0;

            std::map<int, double> WeightofUE;

            //檢查所有UE
            for (int ue = 0; ue < UE_Num; ue++)
            {

                //若AP i有連到這個ue
                if ((int)AP_association_matrix.getElement(i, ue) == 1)
                {

                    //初始state不必考慮handover
                    if (state == 0)
                    {

                        //則更新分母
                        Denominator += my_UE_list.getElement(ue).Get_Required_DataRate() / RF_data_rate_matrix.getElement(i, ue);

                        //更新該ue的分子
                        WeightofUE[ue] = my_UE_list.getElement(ue).Get_Required_DataRate() / RF_data_rate_matrix.getElement(i, ue);
                    }
                    //非初始state則需要多考慮handover
                    else
                    {
                        int prev_AP = my_UE_list.getElement(ue).Get_Prev_Associated_AP();
                        //更新分母
                        Denominator += (my_UE_list.getElement(ue).Get_Required_DataRate() / handover_efficiency_matrix.getElement(prev_AP, i) * RF_data_rate_matrix.getElement(i, ue));

                        //更新該ue的分子
                        WeightofUE[ue] = (my_UE_list.getElement(ue).Get_Required_DataRate() / handover_efficiency_matrix.getElement(prev_AP, i) * RF_data_rate_matrix.getElement(i, ue));
                    }
                }
            }

            //通過WeightofUE取得每個UE的分子，再處剛剛算出來的分母，即爲每個UE可額外分得的資源量
            std::map<int, double>::iterator it;
            for (it = WeightofUE.begin(); it != WeightofUE.end(); it++)
            {
                double data_rate = it->second / Denominator * TDMA_matrix.getElement(i, 0) + TDMA_matrix.getElement(i, it->first + 1);
                TDMA_matrix.setElement(i, it->first + 1, data_rate);

                //在myUElist[it->first]中更新分得時間比例
                //Note : 此時的值即是最終值
                my_UE_list.getElement(it->first).Set_Time_Fraction(TDMA_matrix.getElement(i, it->first + 1));
            }
            
            //剩餘的資源皆分完
            TDMA_matrix.setElement(i, 0, 0);
        }
    }

    //同理，檢查所有LiFi AP
    for (int i = 0; i < VLC_AP_Num; i++)
    {

        //若有資源未分配完
        if (TDMA_matrix.getElement(i + RF_AP_Num, 0) > 0)
        {

            double Denominator = 0.0;

            std::map<int, double> WeightofUE;

            //檢查所有UE
            for (int ue = 0; ue < UE_Num; ue++)
            {

                //若AP i有連到這個ue
                if ((int)AP_association_matrix.getElement(i + RF_AP_Num, ue) == 1)
                {

                    //初始state不必考慮handover
                    if (state == 0)
                    {
                        
                        //更新分母
                        Denominator += my_UE_list.getElement(ue).Get_Required_DataRate() / VLC_data_rate_matrix.getElement(i, ue);

                        //更新該ue的分子
                        WeightofUE[ue] = my_UE_list.getElement(ue).Get_Required_DataRate() / VLC_data_rate_matrix.getElement(i, ue);
                    }
                    //非初始state則需要多考慮handover
                    else
                    {
                        int prev_AP = my_UE_list.getElement(ue).Get_Prev_Associated_AP();
                        //更新分母
                        Denominator += my_UE_list.getElement(ue).Get_Required_DataRate() / (handover_efficiency_matrix.getElement(prev_AP, i + RF_AP_Num) * VLC_data_rate_matrix.getElement(i, ue));

                        //更新該ue的分子
                        WeightofUE[ue] = my_UE_list.getElement(ue).Get_Required_DataRate() / (handover_efficiency_matrix.getElement(prev_AP, i + RF_AP_Num) * VLC_data_rate_matrix.getElement(i, ue));
                    }
                }
            }
            //通過WeightofUE取得每個UE的分子，再處剛剛算出來的分母，即爲每個UE可額外分得的資源量
            std::map<int, double>::iterator it;
            for (it = WeightofUE.begin(); it != WeightofUE.end(); it++)
            {
                //注意在TDMA_Matrix中，UEindex從1開始，故it->first要+1
                double data_rate = it->second / Denominator * TDMA_matrix.getElement(i + RF_AP_Num, 0) + TDMA_matrix.getElement(i + RF_AP_Num, it->first + 1);
                TDMA_matrix.setElement(i + RF_AP_Num, it->first + 1, data_rate);
                

                //在myUElist[it->first]中更新分得時間比例
                //Note : 此時的值即是最終值
                my_UE_list.getElement(it->first).Set_Time_Fraction(TDMA_matrix.getElement(i + RF_AP_Num, it->first + 1));
            }
        
            //剩餘的資源皆分完
            TDMA_matrix.setElement(i + RF_AP_Num, 0, 0);
        }
    }
}