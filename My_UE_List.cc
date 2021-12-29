#include <vector>

#include "My_UE_List.h";
#include "ns3/mobility-module.h"

using namespace ns3;



MyUeList::MyUeList(NodeContainer &UE_nodes) {
    std::normal_distribution<double> low(low_demand, 2);
	std::normal_distribution<double> high(high_demand, 2);
	std::default_random_engine re(std::chrono::system_clock::now().time_since_epoch().count());

    for (int i = 0; i < UE_Num; i++) {
        double demand = (i < UE_Num * ratioHDU) ? high(re) : low(re);

        Ptr<MobilityModel> UE_mobility_model = (UE_nodes.Get(i))->GetObject<MobilityMode>();
        Vector pos = UE_mobility_model->GetPosition();

        UE_list.push_back(My_UE_Node(i, pos, demand));
    }
}

std::vector<My_UE_Node> MyUeList::getList() {
    return UE_list;
}

My_UE_Node MyUeList::getElement(int i) {
    return UE_list[i];
}

void MyUeList::setElement(int i, My_UE_Node &new_UE_node) {
    UE_list[i] = new_UE_node;
}

int MyUeList::getSize() {
    return UE_list.size();
}

std::iterator<My_UE_Node> begin() {
    return UE_list.begin();
}

std::iterator<My_UE_Node> end() {
    return UE_list.end();
}