#include <algorithm>
#include <map>

#include "ProposedLoadBalanceMethod.h"
#include "TDMAMatrix.h"
#include "AssociationMatrix.h"
#include "AvailableResourceMatrix.h"



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
        available_resource_matrix.allocate(u, my_UE_list, RF_data_rate_matrix);
        available_resource_matrix.allocate(u, my_UE_list, VLC_data_rate_matrix);
    
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
}

void ProposedLoadBalanceMethod::DoAllocateStateN(int &state,
                                                std::vector<std::vector<double>> &RF_DataRate_Matrix,
                                                std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                                                std::vector<std::vector<int>> &AP_Association_Matrix,
                                                std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
                                                std::vector<std::vector<double>> &TDMA_Matrix,
                                                MyUeList &my_UE_list) 
{
    //clear AP association matrix （新一輪的開始要重新算）
    for (int i = 0; i < RF_AP_Num + VLC_AP_Num; i++)
        for (int j = 0; j < UE_Num; j++)
            AP_Association_Matrix[i][j] = 0;

    //clear TDMA matrix （新一輪的開始要重新算）
    for (int i = 0; i < RF_AP_Num + VLC_AP_Num; i++)
    {

        //forall i ,ρ(i,0) = 1 代表每個AP目前都還沒被人連(故剩餘時間比例 = 1)
        TDMA_Matrix[i][0] = 1;

        //目前都還沒有分配時間
        for (int j = 1; j < UE_Num + 1; j++)
            TDMA_Matrix[i][j] = 0;
    }

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
        std::vector<double> Avaliable_Resource_Matrix(RF_AP_Num + VLC_AP_Num);
        for (int i = 0; i < RF_AP_Num; i++)
            Avaliable_Resource_Matrix[i] = TDMA_Matrix[i][0]; //copy RF AP部分
        for (int i = 0; i < VLC_AP_Num; i++)
            Avaliable_Resource_Matrix[i + RF_AP_Num] = TDMA_Matrix[i + RF_AP_Num][0]; //copy VLC AP部分

        //計算如果UE u 連到AP i且分得恰可滿足demand的資源後, AP i還會剩下多少residual resource
        //Note : 注意sort過後的index u未必是原本的UE Node ID
        //Note2 : 現在要考慮handover

        //算WiFi 部分
        for (int i = 0; i < RF_AP_Num; i++)
        
            Avaliable_Resource_Matrix[i] -= my_UE_list.getElement(u).Get_Required_DataRate() /
                                            (Handover_Efficiency_Matrix[my_UE_list.getElement(u).Get_Now_Associated_AP()][i] * RF_DataRate_Matrix[i][my_UE_list.getElement(u).GetID()]);
        //用get_now_associated_ap是因爲這一輪還沒更新AP

        //算LiFi部分
        for (int i = 0; i < VLC_AP_Num; i++)

            Avaliable_Resource_Matrix[i + RF_AP_Num] -= my_UE_list.getElement(u).Get_Required_DataRate() /
                                                        (Handover_Efficiency_Matrix[my_UE_list.getElement(u).Get_Now_Associated_AP()][i + RF_AP_Num] * VLC_DataRate_Matrix[i][my_UE_list.getElement(u).GetID()]);

        //接下來要選max A-Q的AP，因爲我剩的資源越多，越有機會服務更多人
        //用Beta記錄max A-Q的AP
        int Beta = 0;

        //max_residual記錄max A-Q的值
        double max_residual = Avaliable_Resource_Matrix[0];

        /* 從RF AP set找出max A-Q的RF AP 即論文的Eq 17 */

        //Beta_w 記錄剩餘資源最大的RF AP
        int Beta_w = 0;

        //記錄該資源剩餘量
        double max_RF_residual = Avaliable_Resource_Matrix[0];

        //遍歷RF AP更新最佳選擇
        for (int i = 0; i < RF_AP_Num; i++)
            if (Avaliable_Resource_Matrix[i] > max_RF_residual)
            {

                Beta_w = i;

                max_RF_residual = Avaliable_Resource_Matrix[i];
            }

        /* 從VLC AP set找出max A-Q的VLC AP 即論文的Eq 16 */

        //記錄剩餘資源最大的VLC AP
        int Beta_l = RF_AP_Num;

        //記錄該資源剩餘量
        double max_VLC_residual = Avaliable_Resource_Matrix[RF_AP_Num];

        //遍歷VLC AP更新最佳選擇
        for (int i = 0; i < VLC_AP_Num; i++)
            if (Avaliable_Resource_Matrix[i + RF_AP_Num] > max_VLC_residual)
            {

                Beta_l = i + RF_AP_Num;

                max_VLC_residual = Avaliable_Resource_Matrix[i + RF_AP_Num];
            }

        /*接下來要決定到底要連Beta_l還是Beta_w ： 看誰能提供的滿意度高就連誰，如果一樣大優先連lifi*/
        /*又因爲同一個UE demand一樣，所以比較滿意度等同於比throughput 我們這邊簡單一點 比throughput就好*/

        //連到Beta_l可提供的throughput
        double potential_throughput_of_Beta_l = TDMA_Matrix[Beta_l][0] * Handover_Efficiency_Matrix[my_UE_list.getElement(u).Get_Now_Associated_AP()][Beta_l] * VLC_DataRate_Matrix[Beta_l - RF_AP_Num][my_UE_list.getElement(u).GetID()];

        //連到Beta_l可提供的滿意度,上限爲1
        double potential_satisfaction_of_Beta_l = potential_throughput_of_Beta_l / my_UE_list.getElement(u).Get_Required_DataRate();

        if (potential_satisfaction_of_Beta_l > 1)
            potential_satisfaction_of_Beta_l = 1;

        //連到Beta_w可提供的throughput
        double potential_throughput_of_Beta_w = TDMA_Matrix[Beta_w][0] * Handover_Efficiency_Matrix[my_UE_list.getElement(u).Get_Now_Associated_AP()][Beta_w] * RF_DataRate_Matrix[Beta_w][my_UE_list.getElement(u).GetID()];

        //連到Beta_w可提供的滿意度,上限爲1
        double potential_satisfaction_of_Beta_w = potential_throughput_of_Beta_w / my_UE_list.getElement(u).Get_Required_DataRate();

        if (potential_satisfaction_of_Beta_w > 1)
            potential_satisfaction_of_Beta_w = 1;

        //找能提供較大滿意度的人連
        if (potential_satisfaction_of_Beta_l >= potential_satisfaction_of_Beta_w)
        {

            Beta = Beta_l;

            max_residual = max_VLC_residual;
        }
        else
        {

            Beta = Beta_w;

            max_residual = max_RF_residual;
        }

        //確定AP selection結果
        //連到Beta
        AP_Association_Matrix[Beta][my_UE_list.getElement(u).GetID()] = 1;

        //更新：新的AP到myUElist
        my_UE_list.getElement(u).Set_Now_Associated_AP(Beta);

        /* Resource Allocation Phase */

        //先盡可能配置滿足其demand的資源給ue u
        //如果無法滿足demand，則chosen ap剩的全給ue u

        //如果剩餘資源量爲負，則代表無法滿足ue u的demand.故剩下的資源全給ue u,導致分配後剩餘資源 = 0
        if (max_residual < 0)
            max_residual = 0;

        //這行的想法是 ρ(Beta,u) (要分配的資源量) =   ρ(Beta,0)(目前所剩資源量) - max_residual(分配後的剩餘資源量)
        TDMA_Matrix[Beta][my_UE_list.getElement(u).GetID() + 1] = TDMA_Matrix[Beta][0] - max_residual;

        //在myUElist[u]中更新分得時間比例
        //Note : 此時的值未必是最終值，之後再做residual resource allocation時也許會有變化
        my_UE_list.getElement(u).Set_Time_Fraction(TDMA_Matrix[Beta][my_UE_list.getElement(u).GetID() + 1]);

        //更新ρ(Beta,0)(目前所剩資源量) ，即要再扣掉剛剛分配的量，
        TDMA_Matrix[Beta][0] -= TDMA_Matrix[Beta][my_UE_list.getElement(u).GetID() + 1];
    }

    ResidualResourceAllocate(state, 
                            RF_DataRate_Matrix,
                            VLC_DataRate_Matrix,
                            Handover_Efficiency_Matrix,
                            AP_Association_Matrix,
                            TDMA_Matrix,
                            my_UE_list);
}


void ResidualResourceAllocate(int &state,
                            std::vector<std::vector<double>> &RF_DataRate_Matrix,
                            std::vector<std::vector<double>> &VLC_DataRate_Matrix,
                            std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
                            std::vector<std::vector<int>> &AP_Association_Matrix,
                            std::vector<std::vector<double>> &TDMA_Matrix,
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
        std::vector<std::vector<double>> &RF_DataRate_Matrix,
        std::vector<std::vector<double>> &VLC_DataRate_Matrix,
        std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
        std::vector<std::vector<int>> &AP_Association_Matrix,
        std::vector<std::vector<double>> &TDMA_Matrix,
        MyUeList &my_UE_list)
{

    //檢查所有WiFi AP
    for (int i = 0; i < RF_AP_Num; i++)
    {

        //若有資源未分配完
        if (TDMA_Matrix[i][0] > 0)
        {

            //記錄有最大速度的UE
            int best_rate_user = -1;

            //記錄best_rate_user的速度
            double best_rate = 0;

            //檢查所有UE
            for (int ue = 0; ue < UE_Num; ue++)
            {

                //若AP i有連到這個ue
                if (AP_Association_Matrix[i][ue] == 1)
                {

                    //初始state不必考慮handover
                    if (state == 0)
                    {
                        //則要檢查其速度是否比之前的best_rate_user高，若有利則更新
                        if (RF_DataRate_Matrix[i][ue] > best_rate)
                        {
                            best_rate_user = ue;
                            best_rate = RF_DataRate_Matrix[i][ue];
                        }
                    }
                    //非初始state則需要多考慮handover
                    else
                    {

                        if (Handover_Efficiency_Matrix[my_UE_list.getElement(ue).Get_Prev_Associated_AP()][i] * RF_DataRate_Matrix[i][ue] > best_rate)
                        {
                            best_rate_user = ue;
                            best_rate = Handover_Efficiency_Matrix[my_UE_list.getElement(ue).Get_Prev_Associated_AP()][i] * RF_DataRate_Matrix[i][ue];
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
                TDMA_Matrix[i][best_rate_user + 1] += TDMA_Matrix[i][0];

                //在myUElist[best_rate_user]中更新分得時間比例
                //Note : 此時的值即是最終值
                my_UE_list.getElement(best_rate_user).Set_Time_Fraction(TDMA_Matrix[i][best_rate_user + 1]);
                
                //分完就沒了
                TDMA_Matrix[i][0] = 0;
            }
        }
    }

    //同理，檢查所有LiFi AP
    for (int i = 0; i < VLC_AP_Num; i++)
    {

        //若有資源未分配完
        if (TDMA_Matrix[i + RF_AP_Num][0] > 0)
        {

            //記錄有最大速度的UE
            int best_rate_user = -1;

            //記錄best_rate_user的速度
            double best_rate = 0;

            //檢查所有UE
            for (int ue = 0; ue < UE_Num; ue++)
            {

                //若AP i有連到這個ue
                if (AP_Association_Matrix[i + RF_AP_Num][ue] == 1)
                {

                    if (state == 0)
                    {
                        //則要檢查其速度是否比之前的best_rate_user高，若有利則更新
                        if (VLC_DataRate_Matrix[i][ue] > best_rate)
                        {
                            best_rate_user = ue;
                            best_rate = VLC_DataRate_Matrix[i][ue];
                        }
                    }
                    else
                    {
                        if (Handover_Efficiency_Matrix[my_UE_list.getElement(ue).Get_Prev_Associated_AP()][i + RF_AP_Num] * VLC_DataRate_Matrix[i][ue] > best_rate)
                        {
                            best_rate_user = ue;
                            best_rate = Handover_Efficiency_Matrix[my_UE_list.getElement(ue).Get_Prev_Associated_AP()][i + RF_AP_Num] * VLC_DataRate_Matrix[i][ue];
                        }
                    }
                }
            }

            //若的確存在這種user才做
            //Note : 因爲可能這個ap不服務任何人，這樣best_rate_user就會是-1，但是實際上不存在user -1，如需要防止這樣情況發生
            if (best_rate_user > -1)
            {
                //剩下的資源全數分給best_rate_user
                TDMA_Matrix[i + RF_AP_Num][best_rate_user + 1] += TDMA_Matrix[i + RF_AP_Num][0];

                //在myUElist[best_rate_user]中更新分得時間比例
                //Note : 此時的值即是最終值
                my_UE_list.getElement(best_rate_user).Set_Time_Fraction(TDMA_Matrix[i + RF_AP_Num][best_rate_user + 1]);

                //分完就沒了
                TDMA_Matrix[i + RF_AP_Num][0] = 0;
            }
        }
    }
}
/*
    根據demand比例分配剩餘資源
    例子可以看我論文
*/

void DP(int &state,
        std::vector<std::vector<double>> &RF_DataRate_Matrix,
        std::vector<std::vector<double>> &VLC_DataRate_Matrix,
        std::vector<std::vector<double>> &Handover_Efficiency_Matrix,
        std::vector<std::vector<int>> &AP_Association_Matrix,
        std::vector<std::vector<double>> &TDMA_Matrix,
        MyUeList &my_UE_list)
{

    //檢查所有WiFi AP
    for (int i = 0; i < RF_AP_Num; i++)
    {

        //若有資源未分配完
        if (TDMA_Matrix[i][0] > 0)
        {

            //以demand/effective data rate爲權重分配剩餘資原
            //記錄分母 = 權重之總和
            double Denominator = 0.0;

            std::map<int, double> WeightofUE;

            //檢查所有UE
            for (int ue = 0; ue < UE_Num; ue++)
            {

                //若AP i有連到這個ue
                if (AP_Association_Matrix[i][ue] == 1)
                {

                    //初始state不必考慮handover
                    if (state == 0)
                    {

                        //則更新分母
                        Denominator += my_UE_list.getElement(ue).Get_Required_DataRate() / RF_DataRate_Matrix[i][ue];

                        //更新該ue的分子
                        WeightofUE[ue] = my_UE_list.getElement(ue).Get_Required_DataRate() / RF_DataRate_Matrix[i][ue];
                    }
                    //非初始state則需要多考慮handover
                    else
                    {

                        //更新分母
                        Denominator += (my_UE_list.getElement(ue).Get_Required_DataRate() / Handover_Efficiency_Matrix[my_UE_list.getElement(ue).Get_Prev_Associated_AP()][i] * RF_DataRate_Matrix[i][ue]);

                        //更新該ue的分子
                        WeightofUE[ue] = (my_UE_list.getElement(ue).Get_Required_DataRate() / Handover_Efficiency_Matrix[my_UE_list.getElement(ue).Get_Prev_Associated_AP()][i] * RF_DataRate_Matrix[i][ue]);
                    }
                }
            }

            //通過WeightofUE取得每個UE的分子，再處剛剛算出來的分母，即爲每個UE可額外分得的資源量
            std::map<int, double>::iterator it;
            for (it = WeightofUE.begin(); it != WeightofUE.end(); it++)
            {

                TDMA_Matrix[i][it->first + 1] += it->second / Denominator * TDMA_Matrix[i][0];

                //在myUElist[it->first]中更新分得時間比例
                //Note : 此時的值即是最終值
                my_UE_list.getElement(it->first).Set_Time_Fraction(TDMA_Matrix[i][it->first + 1]);
            }
            
            //剩餘的資源皆分完
            TDMA_Matrix[i][0] = 0;
        }
    }

    //同理，檢查所有LiFi AP
    for (int i = 0; i < VLC_AP_Num; i++)
    {

        //若有資源未分配完
        if (TDMA_Matrix[i + RF_AP_Num][0] > 0)
        {

            double Denominator = 0.0;

            std::map<int, double> WeightofUE;

            //檢查所有UE
            for (int ue = 0; ue < UE_Num; ue++)
            {

                //若AP i有連到這個ue
                if (AP_Association_Matrix[i + RF_AP_Num][ue] == 1)
                {

                    //初始state不必考慮handover
                    if (state == 0)
                    {
                        
                        //更新分母
                        Denominator += my_UE_list.getElement(ue).Get_Required_DataRate() / VLC_DataRate_Matrix[i][ue];

                        //更新該ue的分子
                        WeightofUE[ue] = my_UE_list.getElement(ue).Get_Required_DataRate() / VLC_DataRate_Matrix[i][ue];
                    }
                    //非初始state則需要多考慮handover
                    else
                    {
                        //更新分母
                        Denominator += my_UE_list.getElement(ue).Get_Required_DataRate() / (Handover_Efficiency_Matrix[my_UE_list.getElement(ue).Get_Prev_Associated_AP()][i + RF_AP_Num] * VLC_DataRate_Matrix[i][ue]);

                        //更新該ue的分子
                        WeightofUE[ue] = my_UE_list.getElement(ue).Get_Required_DataRate() / (Handover_Efficiency_Matrix[my_UE_list.getElement(ue).Get_Prev_Associated_AP()][i + RF_AP_Num] * VLC_DataRate_Matrix[i][ue]);
                    }
                }
            }
            //通過WeightofUE取得每個UE的分子，再處剛剛算出來的分母，即爲每個UE可額外分得的資源量
            std::map<int, double>::iterator it;
            for (it = WeightofUE.begin(); it != WeightofUE.end(); it++)
            {
                //注意在TDMA_Matrix中，UEindex從1開始，故it->first要+1
                TDMA_Matrix[i + RF_AP_Num][it->first + 1] += it->second / Denominator * TDMA_Matrix[i + RF_AP_Num][0];

                //在myUElist[it->first]中更新分得時間比例
                //Note : 此時的值即是最終值
                my_UE_list.getElement(it->first).Set_Time_Fraction(TDMA_Matrix[i + RF_AP_Num][it->first + 1]);
            }
        
            //剩餘的資源皆分完
            TDMA_Matrix[i + RF_AP_Num][0] = 0;
        }
    }
}