#ifndef MY_UE_LIST_H
#define MY_UE_LIST_H

#include <iostream>
#include <fstream>
#include <string>
#include <chrono> // seed

#include "My_UE_node.h";
#include "ns3/core-module.h"



class MyUeList
{
public:
    MyUeList(NodeContainer &UE_nodes);
    std::vector<My_UE_Node> getList();
    
    My_UE_Node getElement(int i);
    void setElement(int i, My_UE_Node &new_UE_node);

    int getSize();
    
    std::iterator<My_UE_Node> begin();
    std::iterator<My_UE_Node> end();

private: 
    std::vector<My_UE_Node> UE_list;
    
};

#endif