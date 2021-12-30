/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

//
// Network topology
//
//           10Mb/s, 10ms       10Mb/s, 10ms
//       n0-----------------n1-----------------n2
//
//
// - Tracing of queues and packet receptions to file
//   "tcp-large-transfer.tr"
// - pcap traces also generated in the following files
//   "tcp-large-transfer-$n-$i.pcap" where n and i represent node and interface
// numbers respectively
//  Usage (e.g.): ./waf --run tcp-large-transfer

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "global_environment.h"
#include "install_mobility.h"
#include "My_UE_Node.h"
#include "My_UE_Node_List.h"
#include "Channel.h"
#include "print.h"
#include "DynamicLB.h"
#include "ProposedMethod.h"
#include "My_UE_List.h"
#include "BenchmarkLoadBalanceMethod.h"
#include "ProposedLoadBalanceMethod.h"

using namespace ns3;
// using namespace std;

NS_LOG_COMPONENT_DEFINE("TcpLargeTransfer");
std::vector<double> Received(1, 0);
std::vector<double> theTime(1, 0);

std::vector<std::vector<int>> AP_Association_Matrix(RF_AP_Num + VLC_AP_Num, std::vector<int>(UE_Num + 1, 0)); //χ(i,u)
std::vector<std::vector<double>> TDMA_Matrix(RF_AP_Num + VLC_AP_Num, std::vector<double>(UE_Num + 1, 0));	  //ρ(i,u)

std::vector<std::vector<double>> Handover_Efficiency_Matrix(RF_AP_Num + VLC_AP_Num, std::vector<double>(RF_AP_Num + VLC_AP_Num, 0)); //η(i,j)

std::vector<std::vector<double>> RF_Channel_Gain_Matrix(RF_AP_Num, std::vector<double>(UE_Num, 0));
std::vector<std::vector<double>> VLC_Channel_Gain_Matrix(VLC_AP_Num, std::vector<double>(UE_Num, 0));

std::vector<std::vector<double>> RF_SINR_Matrix(RF_AP_Num, std::vector<double>(UE_Num, 0));
std::vector<std::vector<double>> VLC_SINR_Matrix(VLC_AP_Num, std::vector<double>(UE_Num, 0));

std::vector<std::vector<double>> RF_DataRate_Matrix(RF_AP_Num + VLC_AP_Num, std::vector<double>(UE_Num, 0));
std::vector<std::vector<double>> VLC_DataRate_Matrix(VLC_AP_Num, std::vector<double>(UE_Num, 0));

std::vector<double> recorded_avg_throughput_per_UE(UE_Num, 0);
std::vector<double> recorded_avg_satisfaction_level_per_UE(UE_Num, 0);

static const uint32_t totalTxBytes = 10000000;
static uint32_t currentTxBytes = 0;
static const uint32_t writeSize = 1040;
static int state = 0;
uint8_t data[writeSize];

BenchmarkLoadBalanceMethod benchmark;
ProposedLoadBalanceMethod proposed;

void StartFlow(Ptr<Socket>, Ipv4Address, uint16_t);
void WriteUntilBufferFull(Ptr<Socket>, uint32_t);

std::string intToString(const int &num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

/* 
  used for tracing and calculating throughput
 */
static void RxEndAddress(Ptr<const Packet> p, const Address &address)
{
	// appends on the received packet to the received data up until that packet
	// and adds that total to the end of the vector
	Received.push_back(Received.back() + p->GetSize());

	//total += p->GetSize();
	theTime.push_back(Simulator::Now().GetSeconds());			  // keeps track of the time during simulation that a packet is received
	double throughput = ((Received.back() * 8)) / theTime.back(); //goodput calculation
	std::cout << "Received.back() is :" << Received.back() << std::endl;
	std::cout << "Rx throughput value is :" << throughput << std::endl;
	std::cout << "Current time is :" << theTime.back() << std::endl;
	std::cout << "Received size: " << p->GetSize() << " at: " << Simulator::Now().GetSeconds() << "s"
			  << "IP: " << InetSocketAddress::ConvertFrom(address).GetIpv4() << std::endl;
}

// void change_dev_rate(NetDeviceContainer & channel){
//   DynamicCast<PointToPointNetDevice>(channel.Get(0))->SetDataRate(DataRate("500Mbps"));
//   DynamicCast<PointToPointNetDevice>(channel.Get(1))->SetDataRate(DataRate("500Mbps"));
// }

void Dynamic_Update_to_NextState(NodeContainer &RF_AP_Nodes, NodeContainer &VLC_AP_Nodes,
								 NodeContainer &UE_Nodes, MyUeList &my_UE_list)
{

#if (PROPOSED_METHOD) //我的做法

	proposed.execute(state, RF_AP_Nodes, VLC_AP_Nodes, UE_Nodes,
					   RF_Channel_Gain_Matrix, VLC_Channel_Gain_Matrix,
					   RF_SINR_Matrix, VLC_SINR_Matrix,
					   RF_DataRate_Matrix, VLC_DataRate_Matrix,
					   Handover_Efficiency_Matrix, AP_Association_Matrix, TDMA_Matrix, my_UE_list);

#else //Benchmark的做法

	benchmark.execute(state, RF_AP_Nodes, VLC_AP_Nodes, UE_Nodes,
						RF_Channel_Gain_Matrix, VLC_Channel_Gain_Matrix,
						RF_SINR_Matrix, VLC_SINR_Matrix,
						RF_DataRate_Matrix, VLC_DataRate_Matrix,
						Handover_Efficiency_Matrix, AP_Association_Matrix, my_UE_list);

#endif

	sort(myUElist.begin(), myUElist.end(), [](My_UE_Node a, My_UE_Node b){ return a.GetID() < b.GetID(); });

	// 再多用recorded_avg_throughput_per_UE記錄每個UE的歷史平均速率
	// 此舉是爲了提供給Simulator::Run()之後的程式取得data rate
	// 否則直接從myUElist去取用不知爲何都是0
	for (int i = 0; i < myUElist.size(); i++)
	{
		recorded_avg_throughput_per_UE[i] = myUElist[i].Get_Avg_Throughput();

		double avg_of_satislevel = 0;
		for (int j = 0; j < myUElist[i].Get_satisfication_level_History().size(); j++)
			avg_of_satislevel += myUElist[i].Get_satisfication_level_History()[j];

		avg_of_satislevel /= myUElist[i].Get_satisfication_level_History().size();
		recorded_avg_satisfaction_level_per_UE[i] = avg_of_satislevel;
	}

	// 用遞迴呼叫scheduler來模擬周期性的檢查
	if (!Simulator::IsFinished())
		// 每隔Tp呼叫一次
		Simulator::Schedule(MilliSeconds(Tp), &Dynamic_Update_to_NextState, RF_AP_Nodes, VLC_AP_Nodes, UE_Nodes, myUElist);
}

int main(int argc, char *argv[])
{
	// Users may find it convenient to turn on explicit debugging
	// for selected modules; the below lines suggest how to do this
	//  LogComponentEnable("TcpL4Protocol", LOG_LEVEL_ALL);
	//  LogComponentEnable("TcpSocketImpl", LOG_LEVEL_ALL);
	//  LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
	//  LogComponentEnable("TcpLargeTransfer", LOG_LEVEL_ALL);

	CommandLine cmd;
	cmd.Parse(argc, argv);

	Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1000));
	Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(1000000000));
	Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(1000000000));

	// initialize the Tx buffer.
	for (uint32_t i = 0; i < writeSize; ++i)
	{
		char m = toascii('a' + i % 26);
		data[i] = m;
	}

	/** Create RF AP Node **/
	NodeContainer RF_AP_Nodes;
	RF_AP_Nodes.Create(RF_AP_Num);
	install_RF_AP_mobility(RF_AP_Nodes);

#if DEBUG_MODE
	print_RF_AP_position(RF_AP_Nodes); //for debug use
#endif

	/** Create VLC AP Nodes **/
	NodeContainer VLC_AP_Nodes;
	VLC_AP_Nodes.Create(VLC_AP_Num);
	install_VLC_AP_mobility(VLC_AP_Nodes);

#if DEBUG_MODE
	print_VLC_AP_position(VLC_AP_Nodes); //for debug use
#endif

	/** Create UE Nodes **/
	NodeContainer UE_Nodes;
	UE_Nodes.Create(UE_Num);
	install_UE_mobility(UE_Nodes);

#if DEBUG_MODE
	print_UE_position(UE_Nodes); //for debug use
#endif

	//生成自定義的UE list
	MyUeList my_UE_list(UE_Nodes);


	Simulator::Schedule(Seconds(0.0), &Dynamic_Update_to_NextState, RF_AP_Nodes, VLC_AP_Nodes, UE_Nodes, my_UE_list);
	Simulator::Stop(Minutes(2));
	Simulator::Run();

	///////////////////////////////////////////////////
	////                                           ////
	////  Average system throughput per iteration  ////
	////                                           ////
	///////////////////////////////////////////////////

	double sum_of_throughput = 0;
	for (int i = 0; i < UE_Num; i++)
		sum_of_throughput += recorded_avg_throughput_per_UE[i];

	////////////////////////////////
	////                        ////
	//// system fairness index  ////
	////                        ////
	////////////////////////////////

	//Note：我採用的是Jain's fairness index但是有另外做修改，詳情去看論文

	double fairness_index;
	//記錄avg throughput的和平方
	double square_of_sum = 0;
	//記錄avg throughput的平方和
	double sum_of_square = 0;

	for (int i = 0; i < UE_Num; i++)
	{
		square_of_sum += recorded_avg_throughput_per_UE[i] / my_UE_list[i].Get_Required_DataRate();
		sum_of_square += pow(recorded_avg_throughput_per_UE[i] / my_UE_list[i].Get_Required_DataRate(), 2);
	}
	square_of_sum = pow(square_of_sum, 2);

	fairness_index = square_of_sum / (UE_Num * sum_of_square);

	////////////////////////////////////////////////
	////                                        ////
	////   system avg. satisfication level       ////
	////                                        ////
	////////////////////////////////////////////////

	//記錄平均每個UE的滿意度
	double sys_avg_satis_level = 0;

	for (int i = 0; i < recorded_avg_satisfaction_level_per_UE.size(); i++)
		sys_avg_satis_level += recorded_avg_satisfaction_level_per_UE[i];

	sys_avg_satis_level /= UE_Num;

	////////////////////////////////
	////                        ////
	////   將模擬結果寫檔         ////
	////   output to .csv file  ////
	////                        ////
	////////////////////////////////

	std::fstream outFile;
	//寫檔路徑
	outFile.open("/home/hsnk/repos/ns-3-allinone/ns-3.25/scratch/thesis/output.csv", std::ios::out | std::ios::app);

	if (!outFile.is_open())
		std::cout << "file not open";
	else
	{
		outFile << sum_of_throughput << ',' << fairness_index << ',' << sys_avg_satis_level << ',';
		//這是我另外記錄的:每個AP的被佔用率(可以想做是loading的感覺)
		for (int i = 0; i < RF_AP_Num + VLC_AP_Num; i++)
			outFile << std::setiosflags(std::ios::fixed) << std::setprecision(4) << ((double)AP_Association_Matrix[i][UE_Num]) / (state * UE_Num) << ',';

		outFile << std::endl;
	}

	outFile.close();
	Simulator::Destroy();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//begin implementation of sending "Application"
void StartFlow(Ptr<Socket> localSocket,
			   Ipv4Address servAddress,
			   uint16_t servPort)
{
	NS_LOG_LOGIC("Starting flow at time " << Simulator::Now().GetSeconds());
	localSocket->Connect(InetSocketAddress(servAddress, servPort)); //connect

	// tell the tcp implementation to call WriteUntilBufferFull again
	// if we blocked and new tx buffer space becomes available
	//localSocket->SetSendCallback (MakeCallback (&WriteUntilBufferFull));
	WriteUntilBufferFull(localSocket, localSocket->GetTxAvailable());
	currentTxBytes = 0;
}

void WriteUntilBufferFull(Ptr<Socket> localSocket, uint32_t txSpace)
{
	while (currentTxBytes < totalTxBytes && localSocket->GetTxAvailable() > 0)
	{
		uint32_t left = totalTxBytes - currentTxBytes;
		uint32_t dataOffset = currentTxBytes % writeSize;
		uint32_t toWrite = writeSize - dataOffset;
		toWrite = std::min(toWrite, left);
		toWrite = std::min(toWrite, localSocket->GetTxAvailable());
		int amountSent = localSocket->Send(&data[dataOffset], toWrite, 0);
		if (amountSent < 0)
		{
			// we will be called again when new tx space becomes available.
			return;
		}
		currentTxBytes += amountSent;
	}
	localSocket->Close();
}
