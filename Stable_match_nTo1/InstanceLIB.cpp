#include "stdafx.h"

#include <fstream>
#include <cmath>
#include "InstanceLIB.h"

//Ϊ��vector���Զ��������������Ҫ����һ��ȫ�ֱȽϺ���
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
	//���ȣ�����route��preferList
	for (int i = 0; i < routeNum; i++)
	{
		Conroutes[i].preferList = vector<RoutePerList>(Conf::MaxNum_preferList);
	}

	//����shipment��preferList
	for (int i = 0; i < shipmentNum; i++)
	{
		Conshipments[i].feasiRoute_Num = 0;
		Conshipments[i].preferList = vector<ShipmentPerList>(routeNum + 1);
		//����ÿ��route
		for (int j = 0; j < routeNum; j++)
		{
			//����,Conshipments[i]�����յ��Ƿ���Conroutes[j]��passNode�У�
			if (1==Conroutes[j].passNode_indicator[Conshipments[i].origin] && 1 == Conroutes[j].passNode_indicator[Conshipments[i].destination])
			{
				//Ȼ��Conshipments[i]��demand�Ƿ�С��Conroutes[j]��capacity��
				if (Conshipments[i].demand<= Conroutes[j].capacity)
				{
					Conshipments[i].preferList[Conshipments[i].feasiRoute_Num].matched_route = Conroutes[j].Rindex;
					//�ܳɱ�=����ɱ�+ʱ��ɱ�
					double travelCost = Conroutes[j].freight[Conroutes[j].getNodePosi(Conshipments[i].origin)]* Conshipments[i].demand*Conshipments[i].cooperation[j];
					double timeCost = Conroutes[j].Length[Conroutes[j].getNodePosi(Conshipments[i].origin)]* Conshipments[i].demand*Conshipments[i].time_value;
					Conshipments[i].preferList[Conshipments[i].feasiRoute_Num].matched_cost = travelCost + timeCost;
					
					Conshipments[i].feasiRoute_Num = Conshipments[i].feasiRoute_Num + 1;

					//ͬʱ������Conroutes[j]�ܹ����˵�shipment
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
		//��Conshipments[i].preferList����Conshipments[i].preferList.matched_cost��������
		sort(Conshipments[i].preferList.begin(), Conshipments[i].preferList.begin()+ Conshipments[i].feasiRoute_Num, compareBycost);
		//Conshipments[i].preferList���������ķ��������Լ�ƥ��
		Conshipments[i].scheme_Num = Conshipments[i].feasiRoute_Num +1;
	}

	//����route�а���2��shipment���ϵ�scheme��preferList
	for (int i = 0; i < routeNum; i++)
	{
		//���ɰ���2��shipment����scheme�Ĺ���
		//��k=2������[��Conroutes[i].feasiShipment_Num���ҵ�k��]��������ϣ����ж��Ƿ�����Conroutes[i].capacity
		//�����k����û���κ������������Conroutes[i].capacit��˵������scheme�Ѿ�ȫ������
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
						//�����jָ����Conroutes[i].preferList�еĵ�j����������shipment�����
						temp_arr[Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_shipmentNum] = j;
						Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_shipmentNum ++;
						tempCapacity = tempCapacity + Conroutes[i].preferList[j].scheme_totaldemand;
					}
				}
				//�жϸ÷����Ƿ�����Capacity
				if (tempCapacity<= Conroutes[i].capacity)
				{
					//�ȿ���Conroutes[i].preferList[Conroutes[i].scheme_Num]��matched_shipment��matched_Sindicator
					Conroutes[i].preferList[Conroutes[i].scheme_Num].matched_shipment = vector<int>(Conroutes[i].preferList[Conroutes[i].scheme_Num].scheme_shipmentNum, -1);
					Conroutes[i].preferList[Conroutes[i].scheme_Num].matched_Sindicator= vector<int>(shipmentNum,0);

					//����Conroutes[i]�ĵ�Conroutes[i].scheme_Num+1��preferList
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
		//��Conroutes[i].preferList����Conroutes[i].preferList.scheme_revenue��������
		sort(Conroutes[i].preferList.begin(), Conroutes[i].preferList.begin()+ Conroutes[i].scheme_Num, compareByrevenue);
		//Conroutes[i].preferList���������ķ��������Լ�ƥ��
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
	//Conshipments��ÿ�����嶼�������
	for (int i = 0; i < shipmentNum; i++)
	{
		//����proposed_route
		proposed_route[i]=Conshipments[i].Propose();
	}
}

bool InstanceLIB::update_prestateInS()
{
	//˼·
	//refused_shipment�б����Ϊ1��shipment�ĵ�ǰѡ�񷽰���preferList_state[matched_scheme]�����Ϊ1
	//refused_shipment�б����Ϊ1��shipmentѡ��������ƥ��
	
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
	//˼·
	//ÿ��shipment����ܾ�ǰn������
	//�ٵ���generateSolution()

	//������ÿ��shipment�ľܾ��б�
	reset_selection();
	//ÿ��shipment����ܾ�ǰn������
	int random_reject;
	for (int i = 0; i < shipmentNum; i++)
	{
		random_reject = rand() % (Conshipments[i].feasiRoute_Num);
		for(int j = 0; j < random_reject; j++)
		{
			Conshipments[i].preferList_state[j] = 1;
		}
	}
	//�������-�ܾ�����
	generateSolution();
}

void InstanceLIB::generateSolution()
{
	//��ʼ��˼·��
	//������ÿ��shipment���preferList��δ���ܾ�����÷���
	//ÿ��route�յ���������preferList�ҵ���õķ��������ܾ����ڸ÷����е�shipment
	//ÿ��shipment��preferList�бȵ�ǰѡ�񷽰����õķ����������Ϊ�ܾ�������ֹ

	bool propose_ornot = true;

	while (true==propose_ornot)
	{
		//ÿ��shipment�������
		propose_byShipment();
		//ÿ��route�ռ�����
		for (int i = 0; i < shipmentNum; i++)
		{
			//ע����Conroutes�ܾ�֮ǰ���refused_shipment
			refused_shipment[i] = 0;
			//��proposed_route����Conroutes��buffer������
			if (proposed_route[i] >= 0)
			{
				Conroutes[proposed_route[i]].Add_Tobuffer(i);
			}
		}
		//ÿ��routeѡ�񷽰����ܾ�����
		for (int i = 0; i < routeNum; i++)
		{
			Conroutes[i].Accept_refuse(refused_shipment);
		}
		//����Conshipments��preferList_state��matched_scheme
		propose_ornot = update_prestateInS();
	}
}


InstanceLIB::InstanceLIB(string pathInstance_network, string pathInstance_shipment, string pathInstance_route, string pathInstance_cooperation)
{
	string content;

	//����csv�е�����
	//��ȡnetwork
	nodeNum = 0;
	ifstream inputFile_network(pathInstance_network);
	if (inputFile_network.is_open())
	{
		//��1�е�1�в���
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

	//��ȡshipment
	shipmentNum = 0;
	Total_containerNum = 0;
	ifstream inputFile_shipment(pathInstance_shipment);
	if (inputFile_shipment.is_open())
	{
		//��1�е�1�в���
		getline(inputFile_shipment, content, ',');
		//shipmentNum
		getline(inputFile_shipment, content);
		shipmentNum= atoi(content.c_str());
		//��2�в���
		getline(inputFile_shipment, content);

		if (shipmentNum <= 0) throw string("Number of shipments is undefined");

		//����Conshipments
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
			//��һ�в���
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
	
	//��ȡroute
	routeNum = 0;
	Total_capacity = 0;
	ifstream inputFile_route(pathInstance_route);
	if (inputFile_route.is_open())
	{
		//��1�е�1�в���
		getline(inputFile_route, content, ',');
		//routeNum
		getline(inputFile_route, content);
		routeNum = atoi(content.c_str());
		//��2�в���
		getline(inputFile_route, content);

		if (routeNum <= 0) throw string("Number of routes is undefined");

		//����Conroutes
		Conroutes = vector<Route>(routeNum);

		for (int i = 0; i <routeNum; i++)
		{
			Conroutes[i].Rindex = i;
			getline(inputFile_route, content, ',');
			//��һ�в���
			getline(inputFile_route, content, ',');
			//passNode_Num
			getline(inputFile_route, content, ',');
			Conroutes[i].passNode_Num = atof(content.c_str());
			//����
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

	//��ȡcooperation parameter
	ifstream inputFile_cooperation(pathInstance_cooperation);
	if (inputFile_cooperation.is_open())
	{
		//��1�в���
		getline(inputFile_cooperation, content);

		for (int i = 0; i <shipmentNum; i++)
		{
			//����Conshipments[i]�е�cooperation����
			Conshipments[i].cooperation = vector<double>(routeNum);
			//��1�в���
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

	//����
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

	//����buffer�з���ֻҪ��preferList[schemeOrder].scheme_shipmentNum��Ԫ����preferList[schemeOrder].matched_Sindicator�б��Ϊ1����÷�������buffer���Ӽ�
	for (int i = 0; i < buffer_num; i++)
	{
		if (1 == preferList[schemeOrder].matched_Sindicator[buffer[i]])
		{
			check_num = check_num + 1;
			if (check_num >= preferList[schemeOrder].scheme_shipmentNum)
			{
				//preferList�е�schemeOrder��������buffer���Ӽ�
				belongto = true;
				break;
			}
		}
	}

	return belongto;
}

void Route::Accept_refuse(vector<int>& refusedS)
{
	//˼·��
	//��preferList���������¼���buffer�з����Ƿ�����ȫ����preferList.matched_Sindicator�����Ϊ1��,һ��������ѡ��÷���matched_scheme
	//��buffer�е�����preferList[matched_scheme].matched_Sindicator�б��Ϊ0��shipment���ܾ�
	//���ܾ���shipment������refusedS

	matched_scheme = scheme_Num-1;
	//��һ������preferList
	for (int i = 0; i < scheme_Num; i++)
	{
		//�ڶ�����֤preferList[i].matched_Sindicator�б��Ϊ1��shipment�Ƿ�Ϊbuffer���Ӽ�
		//����buffer_num��>=preferList[i].scheme_shipmentNum
		if (buffer_num>= preferList[i].scheme_shipmentNum)
		{
			//���ż���buffer�з���ֻҪ��preferList[i].scheme_shipmentNum��Ԫ����preferList[i].matched_Sindicator�б��Ϊ1�����÷�����ѡ��
			if (true== check_scheme_Belongtobuffer(i))
			{
				//preferList�е�i��������ѡ��
				matched_scheme = i;
				break;
			}
		}
	}

	//�ڶ����ҵ�buffer����preferList[matched_scheme].matched_Sindicator�б��Ϊ0��shipment
	for (int i = 0; i < buffer_num; i++)
	{
		if (0 == preferList[matched_scheme].matched_Sindicator[buffer[i]])
		{
			//���������ܾ���shipment������refusedS
			refusedS[buffer[i]] = 1;
		}
	}
	//���buffer
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

	//�ҵ��Լ�preferList����Ini_preferList_state�����Ϊ0���ұ�matched_scheme�����ߵ���÷���
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
