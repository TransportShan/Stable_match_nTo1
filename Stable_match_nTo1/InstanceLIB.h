#ifndef INSTANCELIB_H
#define INSTANCELIB_H

#include<string>
#include<vector>
#include <climits>
#include <algorithm>

#include "Conf.h"

using namespace std;

struct RoutePerList		//Route��ƫ���б�
{
	int scheme_shipmentNum = 0;								//preferList��ĳһ�����а�����shipment������
	double scheme_revenue = 0.0;							//preferList��ĳһ�����ṩ������
	double scheme_totaldemand = 0.0;						//preferList��ĳһ�����ṩ��������
	vector<int> matched_shipment;							//��i��Ԫ�ر�ʾ�÷����е�i��shipment�����,��СΪ[scheme_shipmentNum]
	vector<int> matched_Sindicator;							//��i��Ԫ��=1,��ʾ�÷����������Ϊi��shipment��=0ʱ�򲻰�����СΪ[InstanceLIB.shipmentNum]
};

struct ShipmentPerList		//Shipment��ƫ���б�
{
	int matched_route = -1;					//preferList��ĳһ����ƥ���route�����
	double matched_cost = MY_MAXNUM;		//preferList��ĳһ����ƥ��route�������ĳɱ�
};

class Route
{
public:
	//�������飬����ʹ��vector��������Ҫ�ڹ��캯����ȷ��vector���Ͻ磬��ֹ��������
	//�̶�������Ϣ//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int passNode_Num;										//��route�Ͼ����Ľڵ�����
	int origin;												//��route�����ڵ����(����)
	int destination;										//��route���յ�ڵ����(����)
	double capacity;										//��route��capacity����
	vector<int> passNode;									//��route������ÿ���ڵ����飬���ٴ�СΪInstanceLIB.nodeNum����Ч��СΪpassNode_Num
	vector<int> passNode_indicator;							//��i��Ԫ��=1����ʾ��route�������Ϊi�Ľڵ㣬=0ʱ�򲻾��������ٴ�СΪInstanceLIB.nodeNum
	vector<double> freight;									//��route�ϴ�passNode[i]װ������destinationж�µ�ÿ��TEU���˷ѣ����ٴ�СΪInstanceLIB.nodeNum����Ч��СΪpassNode_Num
	vector<double> Length;									//��route�ϴ�passNode[i]װ������destinationж�µ�����ʱ�䣬���ٴ�СΪInstanceLIB.nodeNum����Ч��СΪpassNode_Num
	int Rindex;												//��route�����(����)

	//������Ϣ
	int feasiShipment_Num=0;								//��route�ܹ����˵�shipment������
	int scheme_Num = 0;										//��route��preference list���ܹ�ƥ��ķ�������,���������ǲ�����ƥ��
	vector<RoutePerList> preferList;						//��route��preference list�����ٴ�СΪInstanceLIB.routeNum+1����СΪ[Conf::MaxNum_preferList]

	//�䶯������Ϣ//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int matched_scheme=-1;									//��route��ǰ��preferListѡ���matched_scheme������
	vector<int> preferList_state;							//��i��Ԫ��=1����ʾ������i�������Ѿ����ܾ���,����Ϊ0,��СΪ[scheme_Num]
	int buffer_num = 0;										//����ĵ�ǰ��������ÿ��Accept_refuse������Ϊ0
	vector<int> buffer;										//�������route��������shipment����ţ���СΪ[InstanceLIB.shipmentNum]

	//����node�����ظ�node�ڸ�route��passNode�ĵڼ���λ��
	int getNodePosi(int node);
	//��ʼ��preferList_state������Ԫ��Ϊ0
	void Ini_preferList_state();
	//����preferList_state
	void Reset_preferList_state();
	//��buffer�����һ��shipment
	void Add_Tobuffer(int shipmentindex);
	//�ж�preferList[schemeOrder].matched_shipment�Ƿ�Ϊbuffer���Ӽ���������Ӽ��򷵻�true,���򷵻�false
	bool check_scheme_Belongtobuffer(int schemeOrder);
	//��routeѡ��buffer��shipment�ܹ���ɵ���÷���������buffer�е����ڸ÷����е�shipment���Ϊ�ܾ�
	void Accept_refuse(vector<int> &refusedS);
	//Route();
public:

};

class Shipment
{
public:
	//�������飬����ʹ��vector��������Ҫ�ڹ��캯����ȷ��vector���Ͻ磬��ֹ��������
	//�̶�������Ϣ//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int origin;												//��shipment�����ڵ����(����)
	int destination;										//��shipment���յ�ڵ����(����)
	double demand;											//��shipment��������
	double time_value;										//��shipmentÿ��TEUÿ��day��������ĳɱ�
	vector<double> cooperation;								//��shipment��ÿ��route��cooperation parameter,��СΪ[InstanceLIB.routeNum]
	int Sindex;												//��shipment�����(����)

	//������Ϣ
	int feasiRoute_Num=0;									//�ܹ����˸�shipment��route����
	int scheme_Num=0;										//��shipment��preference list���ܹ�ƥ��ķ���(route)����,���������ǲ�����ƥ��
	vector<ShipmentPerList> preferList;						//��shipment��preference list�����ٴ�СΪInstanceLIB.routeNum+1����Ч��СΪscheme_Num
	
	//�䶯������Ϣ//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int matched_scheme = -1;								//��shipment��ǰ��preferListѡ���matched_scheme������
	vector<int> preferList_state;							//��i��Ԫ��=1����ʾ������i�������Ѿ����ܾ���,����Ϊ0,��СΪ[scheme_Num]

	//��ʼ��preferList_state������Ԫ��Ϊ0
	void Ini_preferList_state();
	//����matched_scheme��preferList_state
	void Reset_preferList_state();
	//��shipmentѡ��preferList�б�matched_scheme����������Ini_preferList_state�����Ϊ0�ķ���(δ���ܾ�)
	//���ظ�shipmentѡ���route�����,���������ƥ���򷵻�-1
	int Propose();
	//�������Ϊroute_index��route���ڵķ����ڸ�shipment�е�preferList˳�����û�и÷������򷵻�-1
	int get_schemeOrder(int route_index);
	//�ж����Ϊroute_index��·�������Ƿ����ڵ�ǰƥ��matched_scheme������������򷵻�true,���򷵻�false
	bool check_schemeOrder(int route_index);
	//Shipment();
public:

};

class InstanceLIB
{
public:
	int nodeNum;													//�����нڵ������
	int shipmentNum;												//������shipment������
	int routeNum;													//������route������
	int Total_containerNum;											//����shipment������container����
	int Total_capacity;												//����route������capacity����

	vector< Route > Conroutes;										//����һ��Route�࣬��СΪ[routeNum]
	vector< Shipment > Conshipments;								//����һ��Shipment�࣬��СΪ[shipmentNum]

	vector<int> proposed_route;										//ÿ��Shipment��������Route��ţ�-1��ʾ��shipmentѡ�����Լ�ƥ�䣬��СΪ[shipmentNum]
	vector<int> refused_shipment;									//����Routeѡ��󣬵�i��Ԫ��=1��ʾ���Ϊi��Shipment���ܾ�,����Ϊ0����СΪ[shipmentNum]

	void construct_preferList();									//����Conroutes��Conshipments��preferList��ƫ���б�
	void initial_selection();										//��ʼ��Conroutes��Conshipments�ı䶯������Ϣ
	void reset_selection();											//����Conroutes��Conshipments�ĵľܾ��б�
	void assign_scheme(int routeid,int schemeid);					//ָ��Conroutes[routeid].preferList[schemeid]��Conroutes��Conshipmentsѡ��
	void propose_byShipment();										//Conshipments��ÿ�������������
	bool update_prestateInS();										//����refused_shipment������Conshipments�е�preferList_state��matched_scheme,���û�пͻ����ܾ��򷵻�false,����true
	void generateSolution_Byrandom();								//�������һ��ƥ���
	void generateSolution();										//����һ��ƥ���
public:
	// Constructor
	InstanceLIB(string pathInstance_network, string pathInstance_shipment, string pathInstance_route, string pathInstance_cooperation);
};


#endif //INSTANCELIB_H
