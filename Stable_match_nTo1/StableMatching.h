#ifndef STABLEMATCHING_H_
#define STABLEMATCHING_H_

#include <string>

#include "InstanceLIB.h"

using namespace std;

struct Blockscheme		//某个MatchingSol的Blocking scheme
{
	int route_index = -1;									//该Blocking scheme中route的序号
	int scheme_order = -1;									//该Blocking scheme是route_index中排名scheme_order的方案
};

class MatchingSol
{
public:
	bool stable_orNot=false;								//=true表示该解是否为稳定匹配，否则为false
	int ConNum_Unmatched=0;									//该解中匹配没上的container数量
	int ConNum_Inblocking=0;								//该解的所有blocking scheme包含的container数量
	int matchedShipment_Num=0;								//该解中匹配上的shipment数量
	int matchedRoute_Num=0;									//该解中匹配上的route数量
	vector<int> route_matchedScheme;						//第i个元素表示序号为i的route匹配到的方案序号,大小为[StableMatching.instance.routeNum]
	vector<int> shipment_matchedScheme;						//第i个元素表示序号为i的shipment匹配到的方案序号,大小为[StableMatching.instance.shipmentNum]

	//有关blocking scheme属性
	int blockingNum=0;										//该解的所有blocking scheme的数量
	vector<Blockscheme> Blocking_Schemes;					//该解的所有blocking scheme,大小为[StableMatching.instance.routeNum]
public:
};


class StableMatching
{
public:
	int iteration_Num=0;									//迭代的次数
	int restart_Num=0;										//找到稳定解的迭代次数

	InstanceLIB & instance;									//输入算例中的参数
															//这里使用引用（&）表示声明instance的一个副本（前面已经定义过一个实体了，没有必要重复定义），实际上就是这个实体的地址
															//引用的好处就是只占一个地址位，而且不会重复开辟赋值
	MatchingSol CurrentSolution;							//当前匹配解（不一定稳定）
	MatchingSol BestSolution;								//迭代以来最好解
	MatchingSol TempSolution;								//用来存储中间解

	//寻找CurrentSolution的Blockscheme,如果有Blockscheme返回true，否则即稳定解返回false
	bool search_Blockscheme(MatchingSol & targetSol);
	//获取匹配解，更新目标函数
	void get_MatchingSol(MatchingSol & targetSol);
	//将blocking(instance.Conroutes[bs.route_index].preferList[bs.scheme_order])插入CurrentSolution,形成新的匹配解
	void Insertblock_intoSol(Blockscheme bs);
	// 初始解-邻域搜索blocking-达到迭代次数终止
	void run();

	//构建函数
	StableMatching(InstanceLIB & ins);
public:
	//复制fromSol信息到toSol
	void Copy_Sol(MatchingSol &fromSol, MatchingSol &toSol);
	// Exports the BestSolution in csv
	void exportBestSolution(string fileName, double cpuTime);
};

#endif
