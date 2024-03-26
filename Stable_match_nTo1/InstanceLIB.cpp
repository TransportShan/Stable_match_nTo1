#include "stdafx.h"

#include <fstream>
#include <cmath>
#include "InstanceLIB.h"

//为对vector中自定义类进行排序，需要定义一个全局比较函数
bool compareBycost(ShipmentPerList sp1, ShipmentPerList sp2)
{
	return sp1.matched_cost < sp2.matched_cost;
}

bool compareByrevenue(RoutePerList rp1, RoutePerList rp2)
{
	return rp1.scheme_revenue > rp2.scheme_revenue;
}


void InstanceLIB::construct_preferList()
{
	//首先，开辟route的preferList
	for (int i = 0; i < routeNum; i++)
	{
		Conroutes[i].preferList = vector<RoutePerList>(Conf::MaxNum_preferList);
	}

	//生成shipment的preferList
	for (int i = 0; i < shipmentNum; i++)
	{
		Conshipments[i].feasiRoute_Num = 0;
		Conshipments[i].preferList = vector<ShipmentPerList>(routeNum + 1);
		//检验每个route
		for (int j = 0; j < routeNum; j++)
		{
			//首先,Conshipments[i]的起终点是否都在Conroutes[j]的passNode中？
			if (1==Conroutes[j].passNode_indicator[Conshipments[i].origin] && 1 == Conroutes[j].passNode_indicator[Conshipments[i].destination])
			{
				//然后，Conshipments[i]的demand是否小于Conroutes[j]的capacity？
				if (Conshipments[i].demand<= Conroutes[j].capacity)
				{
					Conshipments[i].preferList[Conshipments[i].feasiRoute_Num].matched_route = Conroutes[j].Rindex;
					//总成本=运输成本+时间成本
					double travelCost = Conroutes[j].freight[Conroutes[j].getNodePosi(Conshipments[i].origin)]* Conshipments[i].demand*Conshipments[i].cooperation[j];
					double timeCost = Conroutes[j].Length[Conroutes[j].getNodePosi(Conshipments[i].origin)]* Conshipments[i].demand*Conshipments[i].time_value;
					Conshipments[i].preferList[Conshipments[i].feasiRoute_Num].matched_cost = travelCost + timeCost;
					
					Conshipments[i].feasiRoute_Num = Conshipments[i].feasiRoute_Num + 1;

					//同时，生成Conroutes[j]能够承运的shipment
					Conroutes[j].feasiShipment_Num = Conroutes[j].feasiShipment_Num + 1;
					Conroutes[j].scheme_Num = Conroutes[j].feasiShipment_Num;

					Conroutes[j].preferList[Conroutes[j].feasiShipment_Num - 1].scheme_shipmentNum = 1;
					Conroutes[j].preferList[Conroutes[j].feasiShipment_Num - 1].scheme_revenue = travelCost;
					Conroutes[j].preferList[Conroutes[j].feasiShipment_Num - 1].scheme_totaldemand = Conshipments[i].demand;

					Conroutes[j].preferList[Conroutes[j].feasiShipment_Num - 1].matched_shipment = vector<int>(Conroutes[j].preferList[Conroutes[j].feasiShipment_Num - 1].scheme_shipmentNum, Conshipments[i].Sindex);
					Conroutes[j].preferList[Conroutes[j].feasiShipment_Num - 1].matched_Sindicator = vector<int>(shipmentNum,0);
					Conroutes[j].preferList[Conroutes[j].feasiShipment_Num - 1].matched_Sindicator[Conshipments[i].Sindex] = 1;
				}
			}
		}
		//对Conshipments[i].preferList按照Conshipments[i].preferList.matched_cost升序排序
		sort(Conshipments[i].preferList.begin(), Conshipments[i].preferList.begin()+ Conshipments[i].feasiRoute_Num, compareBycost);
		//Conshipments[i].preferList中排名最后的方案是与自己匹配
		Conshipments[i].scheme_Num = Conshipments[i].feasiRoute_Num +1;
	}

	//生成route中包含2种shipment以上的scheme的preferList
	for (int i = 0; i < routeNum; i++)
	{
		//生成包含2种shipment以上scheme的规则
		//从k=2起，生成[在Conroutes[i].feasiShipment_Num中找到k个]的所有组合，并判断是否满足Conroutes[i].capacity
		//如果在k代，没有任何组合满足满足Conroutes[i].capacit则说明所有scheme已经全部生成
		bool newScheme = true;
		int scheme_length = 2;
		vector<int> temp_arr(shipmentNum,-1);
		while (true== newScheme && scheme_length<= Conroutes[i].feasiShipment_Num)
		{
			newScheme = false;
			//select scheme_length shipments from [Conroutes[i].feasiShipment_Num] shipments
			string bitmask(scheme_length, 1);						// scheme_length leading 1's
			bitmask.resize(Conroutes[i].feasiShipment_Num, 0);		// Conroutes[i].feasiShipment_Num - scheme_length trailing 0's
			do
			{
				double tempCapacity = 0.0;
				double tempRevenue= 0.0;
				Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_shipmentNum = 0;

				for (int j = 0; j < Conroutes[i].feasiShipment_Num; ++j)
				{
					if (1==bitmask[j])
					{
						//这里的j指的是Conroutes[i].preferList中的第j个，而不是shipment的序号
						temp_arr[Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_shipmentNum] = j;
						Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_shipmentNum ++;
						tempCapacity = tempCapacity + Conroutes[i].preferList[j].scheme_totaldemand;
					}
				}
				//判断该方案是否满足Capacity
				if (tempCapacity<= Conroutes[i].capacity)
				{
					//先开辟Conroutes[i].preferList[Conroutes[i].scheme_Num]的matched_shipment和matched_Sindicator
					Conroutes[i].preferList[Conroutes[i].scheme_Num].matched_shipment = vector<int>(Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_shipmentNum, -1);
					Conroutes[i].preferList[Conroutes[i].scheme_Num].matched_Sindicator= vector<int>(shipmentNum,0);

					//生成Conroutes[i]的第Conroutes[i].scheme_Num+1个preferList
					for (int j = 0; j <scheme_length; j++)
					{
						tempRevenue = tempRevenue + Conroutes[i].preferList[temp_arr[j]].scheme_revenue;
						Conroutes[i].preferList[Conroutes[i].scheme_Num].matched_shipment[j] = Conroutes[i].preferList[temp_arr[j]].matched_shipment[0];
						Conroutes[i].preferList[Conroutes[i].scheme_Num].matched_Sindicator[Conroutes[i].preferList[Conroutes[i].scheme_Num].matched_shipment[j]] = 1;
					}
					Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_revenue = tempRevenue;
					Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_totaldemand = tempCapacity;

					Conroutes[i].scheme_Num = Conroutes[i].scheme_Num + 1;
					if(Conroutes[i].scheme_Num>=Conf::MaxNum_preferList-2)
					{	
						scheme_length = Conroutes[i].feasiShipment_Num + 1;
						break;
					}
					newScheme = true;
				}
			} while (prev_permutation(bitmask.begin(), bitmask.end()));

			scheme_length++;
		}
		//对Conroutes[i].preferList按照Conroutes[i].preferList.scheme_revenue降序排序
		sort(Conroutes[i].preferList.begin(), Conroutes[i].preferList.begin()+ Conroutes[i].scheme_Num, compareByrevenue);
		//Conroutes[i].preferList中排名最后的方案是与自己匹配
		Conroutes[i].scheme_Num = Conroutes[i].scheme_Num + 1;
		Conroutes[i].preferList[Conroutes[i].scheme_Num - 1].scheme_shipmentNum = 0;
	}

}

void InstanceLIB::initial_selection()
{
	for (int i = 0; i < shipmentNum; i++)
	{
		Conshipments[i].Ini_preferList_state();
	}

	for (int i = 0; i < routeNum; i++)
	{
		Conroutes[i].Ini_preferList_state();
		Conroutes[i].buffer = vector<int>(shipmentNum, -1);
		Conroutes[i].buffer_num = 0;
	}
}

void InstanceLIB::reset_selection()
{
	for (int i = 0; i < shipmentNum; i++)
	{
		Conshipments[i].Reset_preferList_state();
	}
	for (int i = 0; i < routeNum; i++)
	{
		Conroutes[i].Reset_preferList_state();
	}
}

void InstanceLIB::assign_scheme(int routeid, int schemeid)
{
	Conroutes[routeid].matched_scheme = schemeid;
	for (int i = 0; i < Conroutes[routeid].preferList[schemeid].scheme_shipmentNum; i++)
	{
		Conshipments[Conroutes[routeid].preferList[schemeid].matched_shipment[i]].matched_scheme = Conshipments[Conroutes[routeid].preferList[schemeid].matched_shipment[i]].get_schemeOrder(routeid);
	}
}

void InstanceLIB::propose_byShipment()
{
	//Conshipments中每个个体都提出邀请
	for (int i = 0; i < shipmentNum; i++)
	{
		//更新proposed_route
		proposed_route[i]=Conshipments[i].Propose();
	}
}

bool InstanceLIB::update_prestateInS()
{
	//思路
	//refused_shipment中被标记为1的shipment的当前选择方案在preferList_state[matched_scheme]被标记为1
	//refused_shipment中被标记为1的shipment选择与自身匹配
	
	bool refuseornot = false;
	for (int i = 0; i < shipmentNum; i++)
	{
		if (1==refused_shipment[i])
		{
			Conshipments[i].preferList_state[Conshipments[i].matched_scheme] = 1;
			Conshipments[i].matched_scheme = Conshipments[i].scheme_Num -1;
			refuseornot = true;
		}
	}
	return refuseornot;
}

void InstanceLIB::generateSolution_Byrandom()
{
	//思路
	//每个shipment随机拒绝前n个方案
	//再调用generateSolution()

	//先重置每个shipment的拒绝列表
	reset_selection();
	//每个shipment随机拒绝前n个方案
	int random_reject;
	for (int i = 0; i < shipmentNum; i++)
	{
		random_reject = rand() % (Conshipments[i].feasiRoute_Num);
		for(int j = 0; j < random_reject; j++)
		{
			Conshipments[i].preferList_state[j] = 1;
		}
	}
	//进入提出-拒绝过程
	generateSolution();
}

void InstanceLIB::generateSolution()
{
	//初始解思路：
	//首先由每个shipment提出preferList中未被拒绝的最好方案
	//每个route收到方案后，在preferList找到最好的方案，并拒绝不在该方案中的shipment
	//每个shipment的preferList中比当前选择方案更好的方案都被标记为拒绝，则终止

	bool propose_ornot = true;

	while (true==propose_ornot)
	{
		//每个shipment提出邀请
		propose_byShipment();
		//每个route收集邀请
		for (int i = 0; i < shipmentNum; i++)
		{
			//注意在Conroutes拒绝之前清空refused_shipment
			refused_shipment[i] = 0;
			//将proposed_route传给Conroutes的buffer缓存器
			if (proposed_route[i] >= 0)
			{
				Conroutes[proposed_route[i]].Add_Tobuffer(i);
			}
		}
		//每个route选择方案并拒绝其他
		for (int i = 0; i < routeNum; i++)
		{
			Conroutes[i].Accept_refuse(refused_shipment);
		}
		//更新Conshipments的preferList_state和matched_scheme
		propose_ornot = update_prestateInS();
	}
}


InstanceLIB::InstanceLIB(string pathInstance_network, string pathInstance_shipment, string pathInstance_route, string pathInstance_cooperation)
{
	string content;

	//读入csv中的算例
	//读取network
	nodeNum = 0;
	ifstream inputFile_network(pathInstance_network);
	if (inputFile_network.is_open())
	{
		//第1行第1列不读
		getline(inputFile_network, content, ',');
		//nodeNum
		getline(inputFile_network, content);
		nodeNum = atoi(content.c_str());
		inputFile_network.close();
	}
	else
	{
		throw string("Impossible to open instance file: " + pathInstance_network);
	}

	//读取shipment
	shipmentNum = 0;
	Total_containerNum = 0;
	ifstream inputFile_shipment(pathInstance_shipment);
	if (inputFile_shipment.is_open())
	{
		//第1行第1列不读
		getline(inputFile_shipment, content, ',');
		//shipmentNum
		getline(inputFile_shipment, content);
		shipmentNum= atoi(content.c_str());
		//第2行不读
		getline(inputFile_shipment, content);

		if (shipmentNum <= 0) throw string("Number of shipments is undefined");

		//声明Conshipments
		Conshipments = vector<Shipment>(shipmentNum);

		for (int i = 0; i <shipmentNum; i++)
		{
			Conshipments[i].Sindex = i;
			getline(inputFile_shipment, content, ',');
			//Origin
			getline(inputFile_shipment, content, ',');
			Conshipments[i].origin = atof(content.c_str());
			//destination
			getline(inputFile_shipment, content, ',');
			Conshipments[i].destination = atof(content.c_str());
			//Demand
			getline(inputFile_shipment, content, ',');
			Conshipments[i].demand= atof(content.c_str());
			Total_containerNum = Total_containerNum + Conshipments[i].demand;
			//下一列不读
			getline(inputFile_shipment, content, ',');
			//Time value
			getline(inputFile_shipment, content);
			Conshipments[i].time_value = atof(content.c_str());
		}
		inputFile_shipment.close();
	}
	else
	{
		throw string("Impossible to open instance file: " + pathInstance_shipment);
	}
	
	//读取route
	routeNum = 0;
	Total_capacity = 0;
	ifstream inputFile_route(pathInstance_route);
	if (inputFile_route.is_open())
	{
		//第1行第1列不读
		getline(inputFile_route, content, ',');
		//routeNum
		getline(inputFile_route, content);
		routeNum = atoi(content.c_str());
		//第2行不读
		getline(inputFile_route, content);

		if (routeNum <= 0) throw string("Number of routes is undefined");

		//声明Conroutes
		Conroutes = vector<Route>(routeNum);

		for (int i = 0; i <routeNum; i++)
		{
			Conroutes[i].Rindex = i;
			getline(inputFile_route, content, ',');
			//下一列不读
			getline(inputFile_route, content, ',');
			//passNode_Num
			getline(inputFile_route, content, ',');
			Conroutes[i].passNode_Num = atof(content.c_str());
			//声明
			Conroutes[i].passNode = vector<int>(nodeNum,-1);
			Conroutes[i].passNode_indicator = vector<int>(nodeNum, 0);
			Conroutes[i].freight = vector<double>(nodeNum, MY_MAXNUM);
			Conroutes[i].Length = vector<double>(nodeNum, MY_MAXNUM);
			//passNode
			for (int j = 0; j <nodeNum; j++)
			{
				getline(inputFile_route, content, ',');
				Conroutes[i].passNode[j] = atof(content.c_str());
				if (Conroutes[i].passNode[j]>=0)
				{
					Conroutes[i].passNode_indicator[Conroutes[i].passNode[j]] = 1;
				}
			}
			//freight
			for (int j = 0; j <nodeNum; j++)
			{
				getline(inputFile_route, content, ',');
				Conroutes[i].freight[j] = atof(content.c_str());
			}
			//Length
			for (int j = 0; j <nodeNum; j++)
			{
				getline(inputFile_route, content, ',');
				Conroutes[i].Length[j] = atof(content.c_str());
			}
			//capacity
			getline(inputFile_route, content);
			Conroutes[i].capacity = atof(content.c_str());
			Total_capacity = Total_capacity + Conroutes[i].capacity;
			//origin
			Conroutes[i].origin = Conroutes[i].passNode[0];
			//destination
			Conroutes[i].destination = Conroutes[i].passNode[Conroutes[i].passNode_Num-1];
		}
		inputFile_route.close();
	}
	else
	{
		throw string("Impossible to open instance file: " + pathInstance_route);
	}

	//读取cooperation parameter
	ifstream inputFile_cooperation(pathInstance_cooperation);
	if (inputFile_cooperation.is_open())
	{
		//第1行不读
		getline(inputFile_cooperation, content);

		for (int i = 0; i <shipmentNum; i++)
		{
			//声明Conshipments[i]中的cooperation数组
			Conshipments[i].cooperation = vector<double>(routeNum);
			//第1列不读
			getline(inputFile_cooperation, content, ',');
			//cooperation
			for (int j = 0; j <routeNum-1; j++)
			{
				getline(inputFile_cooperation, content, ',');
				Conshipments[i].cooperation[j] = atof(content.c_str());
			}
			getline(inputFile_cooperation, content);
			Conshipments[i].cooperation[routeNum - 1] = atof(content.c_str());
		}
		inputFile_cooperation.close();
	}
	else
	{
		throw string("Impossible to open instance file: " + pathInstance_cooperation);
	}

	//声明
	proposed_route = vector<int>(shipmentNum,-1);
	refused_shipment = vector<int>(shipmentNum,0);

	construct_preferList();
	initial_selection();
}

int Route::getNodePosi(int node)
{
	int Nodeposi = -1;
	for (int i = 0; i < passNode_Num; i++)
	{
		if (node== passNode[i])
		{
			Nodeposi = i;
			break;
		}
	}

	return Nodeposi;
}

void Route::Ini_preferList_state()
{
	preferList_state = vector<int>(scheme_Num,0);
	matched_scheme = scheme_Num - 1;
}

void Route::Reset_preferList_state()
{
	matched_scheme = scheme_Num - 1;
	for (int i = 0; i < scheme_Num; i++)
	{
		preferList_state[i] = 0;
	}
}

void Route::Add_Tobuffer(int shipmentindex)
{
	buffer[buffer_num] = shipmentindex;
	buffer_num = buffer_num + 1;
}

bool Route::check_scheme_Belongtobuffer(int schemeOrder)
{
	bool belongto = false;
	int check_num = 0;

	//检验buffer中方案只要有preferList[schemeOrder].scheme_shipmentNum个元素在preferList[schemeOrder].matched_Sindicator中标记为1，则该方案就是buffer的子集
	for (int i = 0; i < buffer_num; i++)
	{
		if (1 == preferList[schemeOrder].matched_Sindicator[buffer[i]])
		{
			check_num = check_num + 1;
			if (check_num >= preferList[schemeOrder].scheme_shipmentNum)
			{
				//preferList中第schemeOrder个方案是buffer的子集
				belongto = true;
				break;
			}
		}
	}

	return belongto;
}

void Route::Accept_refuse(vector<int>& refusedS)
{
	//思路：
	//在preferList中由上至下检索buffer中方案是否能完全满足preferList.matched_Sindicator（标记为1）,一旦满足则选择该方案matched_scheme
	//在buffer中但是在preferList[matched_scheme].matched_Sindicator中标记为0的shipment被拒绝
	//被拒绝的shipment被加入refusedS

	matched_scheme = scheme_Num-1;
	//第一，检索preferList
	for (int i = 0; i < scheme_Num; i++)
	{
		//第二，验证preferList[i].matched_Sindicator中标记为1的shipment是否为buffer的子集
		//首先buffer_num得>=preferList[i].scheme_shipmentNum
		if (buffer_num>= preferList[i].scheme_shipmentNum)
		{
			//接着检验buffer中方案只要有preferList[i].scheme_shipmentNum个元素在preferList[i].matched_Sindicator中标记为1，即该方案被选择
			if (true== check_scheme_Belongtobuffer(i))
			{
				//preferList中第i个方案被选择
				matched_scheme = i;
				break;
			}
		}
	}

	//第二，找到buffer中在preferList[matched_scheme].matched_Sindicator中标记为0的shipment
	for (int i = 0; i < buffer_num; i++)
	{
		if (0 == preferList[matched_scheme].matched_Sindicator[buffer[i]])
		{
			//第三，被拒绝的shipment被加入refusedS
			refusedS[buffer[i]] = 1;
		}
	}
	//清空buffer
	buffer_num = 0;
}

void Shipment::Ini_preferList_state()
{
	preferList_state = vector<int>(scheme_Num, 0);
	matched_scheme = scheme_Num-1;
}

void Shipment::Reset_preferList_state()
{
	matched_scheme = scheme_Num - 1;
	for (int i = 0; i < scheme_Num; i++)
	{
		preferList_state[i] = 0;
	}
}

int Shipment::Propose()
{
	int route_index = preferList[matched_scheme].matched_route;

	//找到自己preferList中在Ini_preferList_state被标记为0，且比matched_scheme排名高的最好方案
	for (int i = 0; i < matched_scheme; i++)
	{
		if (0==preferList_state[i])
		{
			route_index = preferList[i].matched_route;
			matched_scheme = i;
			break;
		}
	}

	return route_index;
}

int Shipment::get_schemeOrder(int route_index)
{
	int schemeOrder = -1;

	for (int i = 0; i <scheme_Num-1; i++)
	{
		if (preferList[i].matched_route == route_index)
		{
			schemeOrder = i;
			break;
		}
	}

	return schemeOrder;
}

bool Shipment::check_schemeOrder(int route_index)
{
	bool nolessthan = false;

	for (int i = 0; i <= matched_scheme; i++)
	{
		if (preferList[i].matched_route == route_index)
		{
			nolessthan = true;
			break;
		}
	}

	return nolessthan;
}
