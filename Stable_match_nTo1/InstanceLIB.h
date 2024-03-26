#ifndef INSTANCELIB_H
#define INSTANCELIB_H

#include<string>
#include<vector>
#include <climits>
#include <algorithm>

#include "Conf.h"

using namespace std;

struct RoutePerList		//Route的偏好列表
{
	int scheme_shipmentNum = 0;								//preferList中某一方案中包含的shipment的数量
	double scheme_revenue = 0.0;							//preferList中某一方案提供的收入
	double scheme_totaldemand = 0.0;						//preferList中某一方案提供的总需求
	vector<int> matched_shipment;							//第i个元素表示该方案中第i个shipment的序号,大小为[scheme_shipmentNum]
	vector<int> matched_Sindicator;							//第i个元素=1,表示该方案包含序号为i的shipment，=0时则不包含大小为[InstanceLIB.shipmentNum]
};

struct ShipmentPerList		//Shipment的偏好列表
{
	int matched_route = -1;					//preferList中某一方案匹配的route的序号
	double matched_cost = MY_MAXNUM;		//preferList中某一方案匹配route所付出的成本
};

class Route
{
public:
	//定义数组，可以使用vector，但是需要在构造函数中确定vector的上界，防止反复开辟
	//固定属性信息//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int passNode_Num;										//该route上经过的节点数量
	int origin;												//该route的起点节点序号(索引)
	int destination;										//该route的终点节点序号(索引)
	double capacity;										//该route的capacity限制
	vector<int> passNode;									//该route经过的每个节点数组，开辟大小为InstanceLIB.nodeNum，有效大小为passNode_Num
	vector<int> passNode_indicator;							//第i个元素=1，表示该route经过序号为i的节点，=0时则不经过，开辟大小为InstanceLIB.nodeNum
	vector<double> freight;									//该route上从passNode[i]装货，在destination卸下的每个TEU的运费，开辟大小为InstanceLIB.nodeNum，有效大小为passNode_Num
	vector<double> Length;									//该route上从passNode[i]装货，在destination卸下的运输时间，开辟大小为InstanceLIB.nodeNum，有效大小为passNode_Num
	int Rindex;												//该route的序号(索引)

	//排序信息
	int feasiShipment_Num=0;								//该route能够承运的shipment的数量
	int scheme_Num = 0;										//该route的preference list中能够匹配的方案数量,排名最后的是不参与匹配
	vector<RoutePerList> preferList;						//该route的preference list，开辟大小为InstanceLIB.routeNum+1，大小为[Conf::MaxNum_preferList]

	//变动属性信息//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int matched_scheme=-1;									//该route当前在preferList选择第matched_scheme个方案
	vector<int> preferList_state;							//第i个元素=1，表示排名第i个方案已经被拒绝过,否则为0,大小为[scheme_Num]
	int buffer_num = 0;										//数组的当前容量，在每次Accept_refuse后重置为0
	vector<int> buffer;										//缓存向该route提出邀请的shipment的序号，大小为[InstanceLIB.shipmentNum]

	//输入node，返回该node在该route的passNode的第几个位置
	int getNodePosi(int node);
	//初始化preferList_state中所有元素为0
	void Ini_preferList_state();
	//重置preferList_state
	void Reset_preferList_state();
	//向buffer中添加一个shipment
	void Add_Tobuffer(int shipmentindex);
	//判断preferList[schemeOrder].matched_shipment是否为buffer的子集，如果是子集则返回true,否则返回false
	bool check_scheme_Belongtobuffer(int schemeOrder);
	//该route选择buffer中shipment能够组成的最好方案，对在buffer中但不在该方案中的shipment标记为拒绝
	void Accept_refuse(vector<int> &refusedS);
	//Route();
public:

};

class Shipment
{
public:
	//定义数组，可以使用vector，但是需要在构造函数中确定vector的上界，防止反复开辟
	//固定属性信息//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int origin;												//该shipment的起点节点序号(索引)
	int destination;										//该shipment的终点节点序号(索引)
	double demand;											//该shipment的需求量
	double time_value;										//该shipment每个TEU每个day额外产生的成本
	vector<double> cooperation;								//该shipment对每个route的cooperation parameter,大小为[InstanceLIB.routeNum]
	int Sindex;												//该shipment的序号(索引)

	//排序信息
	int feasiRoute_Num=0;									//能够承运该shipment的route数量
	int scheme_Num=0;										//该shipment的preference list中能够匹配的方案(route)数量,排名最后的是不参与匹配
	vector<ShipmentPerList> preferList;						//该shipment的preference list，开辟大小为InstanceLIB.routeNum+1，有效大小为scheme_Num
	
	//变动属性信息//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int matched_scheme = -1;								//该shipment当前在preferList选择第matched_scheme个方案
	vector<int> preferList_state;							//第i个元素=1，表示排名第i个方案已经被拒绝过,否则为0,大小为[scheme_Num]

	//初始化preferList_state中所有元素为0
	void Ini_preferList_state();
	//重置matched_scheme和preferList_state
	void Reset_preferList_state();
	//该shipment选择preferList中比matched_scheme排名高且在Ini_preferList_state被标记为0的方案(未被拒绝)
	//返回该shipment选择的route的序号,如果不参与匹配则返回-1
	int Propose();
	//返回序号为route_index的route所在的方案在该shipment中的preferList顺序，如果没有该方案，则返回-1
	int get_schemeOrder(int route_index);
	//判断序号为route_index的路径方案是否不劣于当前匹配matched_scheme，如果不劣于则返回true,否则返回false
	bool check_schemeOrder(int route_index);
	//Shipment();
public:

};

class InstanceLIB
{
public:
	int nodeNum;													//网络中节点的数量
	int shipmentNum;												//网络中shipment的数量
	int routeNum;													//网络中route的数量
	int Total_containerNum;											//所有shipment包含的container数量
	int Total_capacity;												//所有route包含的capacity数量

	vector< Route > Conroutes;										//定义一个Route类，大小为[routeNum]
	vector< Shipment > Conshipments;								//定义一个Shipment类，大小为[shipmentNum]

	vector<int> proposed_route;										//每个Shipment提出邀请的Route序号，-1表示该shipment选择与自己匹配，大小为[shipmentNum]
	vector<int> refused_shipment;									//经过Route选择后，第i个元素=1表示序号为i的Shipment被拒绝,否则为0，大小为[shipmentNum]

	void construct_preferList();									//生成Conroutes和Conshipments的preferList（偏好列表）
	void initial_selection();										//初始化Conroutes和Conshipments的变动属性信息
	void reset_selection();											//重置Conroutes和Conshipments的的拒绝列表
	void assign_scheme(int routeid,int schemeid);					//指定Conroutes[routeid].preferList[schemeid]被Conroutes和Conshipments选择
	void propose_byShipment();										//Conshipments中每个个体提出邀请
	bool update_prestateInS();										//根据refused_shipment，更新Conshipments中的preferList_state和matched_scheme,如果没有客户被拒绝则返回false,否则true
	void generateSolution_Byrandom();								//随机生成一个匹配解
	void generateSolution();										//生成一个匹配解
public:
	// Constructor
	InstanceLIB(string pathInstance_network, string pathInstance_shipment, string pathInstance_route, string pathInstance_cooperation);
};


#endif //INSTANCELIB_H
