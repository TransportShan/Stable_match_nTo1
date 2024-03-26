#ifndef STABLEMATCHING_H_
#define STABLEMATCHING_H_

#include <string>

#include "InstanceLIB.h"

using namespace std;

struct Blockscheme		//ĳ��MatchingSol��Blocking scheme
{
	int route_index = -1;									//��Blocking scheme��route�����
	int scheme_order = -1;									//��Blocking scheme��route_index������scheme_order�ķ���
};

class MatchingSol
{
public:
	bool stable_orNot=false;								//=true��ʾ�ý��Ƿ�Ϊ�ȶ�ƥ�䣬����Ϊfalse
	int ConNum_Unmatched=0;									//�ý���ƥ��û�ϵ�container����
	int ConNum_Inblocking=0;								//�ý������blocking scheme������container����
	int matchedShipment_Num=0;								//�ý���ƥ���ϵ�shipment����
	int matchedRoute_Num=0;									//�ý���ƥ���ϵ�route����
	vector<int> route_matchedScheme;						//��i��Ԫ�ر�ʾ���Ϊi��routeƥ�䵽�ķ������,��СΪ[StableMatching.instance.routeNum]
	vector<int> shipment_matchedScheme;						//��i��Ԫ�ر�ʾ���Ϊi��shipmentƥ�䵽�ķ������,��СΪ[StableMatching.instance.shipmentNum]

	//�й�blocking scheme����
	int blockingNum=0;										//�ý������blocking scheme������
	vector<Blockscheme> Blocking_Schemes;					//�ý������blocking scheme,��СΪ[StableMatching.instance.routeNum]
public:
};


class StableMatching
{
public:
	int iteration_Num=0;									//�����Ĵ���
	int restart_Num=0;										//�ҵ��ȶ���ĵ�������

	InstanceLIB & instance;									//���������еĲ���
															//����ʹ�����ã�&����ʾ����instance��һ��������ǰ���Ѿ������һ��ʵ���ˣ�û�б�Ҫ�ظ����壩��ʵ���Ͼ������ʵ��ĵ�ַ
															//���õĺô�����ֻռһ����ַλ�����Ҳ����ظ����ٸ�ֵ
	MatchingSol CurrentSolution;							//��ǰƥ��⣨��һ���ȶ���
	MatchingSol BestSolution;								//����������ý�
	MatchingSol TempSolution;								//�����洢�м��

	//Ѱ��CurrentSolution��Blockscheme,�����Blockscheme����true�������ȶ��ⷵ��false
	bool search_Blockscheme(MatchingSol & targetSol);
	//��ȡƥ��⣬����Ŀ�꺯��
	void get_MatchingSol(MatchingSol & targetSol);
	//��blocking(instance.Conroutes[bs.route_index].preferList[bs.scheme_order])����CurrentSolution,�γ��µ�ƥ���
	void Insertblock_intoSol(Blockscheme bs);
	// ��ʼ��-��������blocking-�ﵽ����������ֹ
	void run();

	//��������
	StableMatching(InstanceLIB & ins);
public:
	//����fromSol��Ϣ��toSol
	void Copy_Sol(MatchingSol &fromSol, MatchingSol &toSol);
	// Exports the BestSolution in csv
	void exportBestSolution(string fileName, double cpuTime);
};

#endif
