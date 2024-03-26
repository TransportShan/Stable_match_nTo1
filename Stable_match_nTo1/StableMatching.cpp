#include "stdafx.h"
#include "StableMatching.h"

#include <fstream>
#include <string>
#include <iostream>

bool StableMatching::search_Blockscheme(MatchingSol & targetSol)
{
	targetSol.blockingNum = 0;
	targetSol.ConNum_Inblocking = 0;
	//采用从route方检验每个可能方案
	bool block_ornot;
	for (int i = 0; i < instance.routeNum; i++)
	{
		//检验instance.Conroutes[i].preferList中比instance.Conroutes[i].preferList[instance.Conroutes[i].matched_scheme]更好的方案
		for (int j = 0; j < instance.Conroutes[i].matched_scheme; j++)
		{
			block_ornot = true;
			//如果该方案存在，那么首先该方案中route方是同意的
			//再校验该方案中instance.Conroutes[i].preferList[j].matched_shipment中的所有shipment是否同意
			for (int k = 0; k < instance.Conroutes[i].preferList[j].scheme_shipmentNum; k++)
			{
				if (false== instance.Conshipments[instance.Conroutes[i].preferList[j].matched_shipment[k]].check_schemeOrder(i))
				{
					//该方案中只要有一个shipment不同意，那么就不是Blockscheme
					block_ornot = false;
					break;
				}
			}
			if (true == block_ornot)
			{
				//方案instance.Conroutes[i].preferList[j]为Blockscheme
				targetSol.Blocking_Schemes[targetSol.blockingNum].route_index = instance.Conroutes[i].Rindex;
				targetSol.Blocking_Schemes[targetSol.blockingNum].scheme_order = j;
				targetSol.ConNum_Inblocking = targetSol.ConNum_Inblocking + instance.Conroutes[i].preferList[j].scheme_totaldemand;
				targetSol.blockingNum = targetSol.blockingNum + 1;
				//每个route下只记录一个Blockscheme
				break;
			}
		}
	}
	if (targetSol.blockingNum>0)
	{
		block_ornot = true;
		targetSol.stable_orNot = false;
	}
	else
	{
		block_ornot = false;
		targetSol.stable_orNot = true;
	}
	return block_ornot;
}

void StableMatching::get_MatchingSol(MatchingSol & targetSol)
{
	targetSol.ConNum_Unmatched = 0;
	targetSol.matchedShipment_Num = 0;
	targetSol.matchedRoute_Num = 0;
	//首先记录当前解
	for (int i = 0; i < instance.shipmentNum; i++)
	{
		targetSol.shipment_matchedScheme[i] = instance.Conshipments[i].matched_scheme;
		if (targetSol.shipment_matchedScheme[i]<instance.Conshipments[i].scheme_Num-1)
		{
			//与某个route匹配
			targetSol.matchedShipment_Num = targetSol.matchedShipment_Num + 1;
		}
		else
		{
			//没有参与匹配
			targetSol.ConNum_Unmatched = targetSol.ConNum_Unmatched + instance.Conshipments[i].demand;
		}
	}
	for (int i = 0; i < instance.routeNum; i++)
	{
		targetSol.route_matchedScheme[i] = instance.Conroutes[i].matched_scheme;
		if (targetSol.route_matchedScheme[i] < instance.Conroutes[i].scheme_Num-1)
		{
			//与某些shipment匹配
			targetSol.matchedRoute_Num = targetSol.matchedRoute_Num + 1;
		}
	}
	search_Blockscheme(targetSol);
}

void StableMatching::Insertblock_intoSol(Blockscheme bs)
{
	//思路
	//1，令instance.Conroutes[bs.route_index]选择第bs.scheme_order个方案，同时instance.Conroutes[bs.route_index].preferList[bs.scheme_order].matched_shipment中shipments
	//选择bs.route_index所在方案
	//2，令instance.Conroutes[bs.route_index].preferList[bs.scheme_order].matched_shipment中shipment的preferList中在bs.route_index所在方案之前的方案被标记为拒绝
	//3，其他的shipment的preferList将bs.route_index所在方案标记为拒绝
	//4，进入instance.generateSolution()过程

	//首先重置route和shipment的拒绝列表
	instance.reset_selection();

	//然后指定blocking bs中的shipment和route互相选择
	instance.assign_scheme(bs.route_index,bs.scheme_order);
	
	//接着更新blocking bs下所有shipment的拒绝列表
	for (int i = 0; i < instance.shipmentNum; i++)
	{
		if (1== instance.Conroutes[bs.route_index].preferList[bs.scheme_order].matched_Sindicator[i])
		{
			//如果instance.Conshipments[i]在blocking bs中，将比instance.Conshipments[i].matched_scheme排名更高的方案标记为拒绝
			for (int j = 0; j < instance.Conshipments[i].matched_scheme; j++)
			{
				instance.Conshipments[i].preferList_state[j] = 1;
			}
		}
		else
		{
			//其他的shipment
			int temp = instance.Conshipments[i].get_schemeOrder(bs.route_index);
			if (temp>=0)
			{
				instance.Conshipments[i].preferList_state[temp] = 1;
			}
		}
	}

	//最后开始shipment邀请+route拒绝过程
	instance.generateSolution();
}

void StableMatching::run()
{
	//第一，生成初始匹配解
	instance.generateSolution();
	get_MatchingSol(CurrentSolution);
	iteration_Num = 0;
	restart_Num = 0;
	Copy_Sol(CurrentSolution, BestSolution);
	if (false == BestSolution.stable_orNot)
	{
		BestSolution.ConNum_Unmatched = MY_MAXNUM;
		BestSolution.ConNum_Inblocking = MY_MAXNUM;
	}

	double RandomWalk_temp;
	int Random_block;
	//主循环
	do
	{
		//第二，如果已经找到全匹配的稳定匹配解则直接输出
		if (0==(CurrentSolution.ConNum_Unmatched + CurrentSolution.ConNum_Inblocking))
		{
			Copy_Sol(CurrentSolution, BestSolution);
			break;
		}
		RandomWalk_temp = double(rand() % 100) / 100;
		if (RandomWalk_temp <=Conf::probability_randomWalk && CurrentSolution.blockingNum>=1)
		{
			Random_block = rand() % CurrentSolution.blockingNum;
			//第三，将CurrentSolution.Blocking_Schemes[Random_block]插入CurrentSolution中，获得新的解
			Insertblock_intoSol(CurrentSolution.Blocking_Schemes[Random_block]);
			//记录该解到CurrentSolution
			get_MatchingSol(CurrentSolution);
		}
		else
		{
			//第四，当CurrentSolution为一个稳定匹配解	
			if (true== CurrentSolution.stable_orNot)
			{
				if ((CurrentSolution.ConNum_Unmatched + CurrentSolution.ConNum_Inblocking) <= (BestSolution.ConNum_Unmatched + BestSolution.ConNum_Inblocking))
				{
					//如果CurrentSolution的目标函数比BestSolution好,就更新BestSolution
					Copy_Sol(CurrentSolution, BestSolution);
				}
				//第五，随机生成一个解
				instance.generateSolution_Byrandom();
				//记录该解到CurrentSolution
				get_MatchingSol(CurrentSolution);
				restart_Num++;
			}
			else
			{
				//第六，Select neighbor and evaluate adjacent matching
				double best_Fvalue = MY_MAXNUM;
				Blockscheme bestblocking;
				//遍历每个CurrentSolution.Blocking_Schemes[i]插入当前解CurrentSolution
				for (int i = 0; i < CurrentSolution.blockingNum; i++)
				{
					Insertblock_intoSol(CurrentSolution.Blocking_Schemes[i]);
					//记录该解到TempSolution
					get_MatchingSol(TempSolution);
					if ((TempSolution.ConNum_Unmatched + TempSolution.ConNum_Inblocking)<best_Fvalue)
					{
						//更新该TempSolution到BestSolution
						bestblocking.route_index = CurrentSolution.Blocking_Schemes[i].route_index;
						bestblocking.scheme_order = CurrentSolution.Blocking_Schemes[i].scheme_order;
						best_Fvalue = TempSolution.ConNum_Unmatched + TempSolution.ConNum_Inblocking;
					}
				}
				Insertblock_intoSol(bestblocking);
				//记录该解到CurrentSolution
				get_MatchingSol(CurrentSolution);
			}
		}
		iteration_Num++;
	} while (iteration_Num<=Conf::MaxNum_iter && restart_Num<= Conf::MaxNum_incompleteStable);
}

StableMatching::StableMatching(InstanceLIB & ins) :
	instance(ins)
{
	CurrentSolution.route_matchedScheme = vector<int>(instance.routeNum, -1);
	CurrentSolution.shipment_matchedScheme= vector<int>(instance.shipmentNum, -1);
	CurrentSolution.Blocking_Schemes= vector<Blockscheme>(instance.routeNum);

	BestSolution.route_matchedScheme = vector<int>(instance.routeNum, -1);
	BestSolution.shipment_matchedScheme = vector<int>(instance.shipmentNum, -1);
	BestSolution.Blocking_Schemes = vector<Blockscheme>(instance.routeNum);

	TempSolution.route_matchedScheme = vector<int>(instance.routeNum, -1);
	TempSolution.shipment_matchedScheme = vector<int>(instance.shipmentNum, -1);
	TempSolution.Blocking_Schemes = vector<Blockscheme>(instance.routeNum);
}

void StableMatching::Copy_Sol(MatchingSol & fromSol, MatchingSol & toSol)
{
	toSol.stable_orNot = fromSol.stable_orNot;
	toSol.ConNum_Unmatched = fromSol.ConNum_Unmatched;
	toSol.ConNum_Inblocking = fromSol.ConNum_Inblocking;
	toSol.matchedShipment_Num = fromSol.matchedShipment_Num;
	toSol.matchedRoute_Num = fromSol.matchedRoute_Num;

	for (int i = 0; i < instance.routeNum; i++)
	{
		toSol.route_matchedScheme[i] = fromSol.route_matchedScheme[i];
		toSol.Blocking_Schemes[i].route_index = fromSol.Blocking_Schemes[i].route_index;
		toSol.Blocking_Schemes[i].scheme_order = fromSol.Blocking_Schemes[i].scheme_order;

	}

	for (int i = 0; i < instance.shipmentNum; i++)
	{
		toSol.shipment_matchedScheme[i] = fromSol.shipment_matchedScheme[i];
	}
}

void StableMatching::exportBestSolution(string fileName, double cpuTime)
{
	ofstream myfile(fileName);
	if (myfile.is_open())
	{
		myfile << "Shipment" << ',';
		for (int i = 0; i < instance.shipmentNum; i++)
		{
			myfile << i+1 << ',';
		}
		myfile << endl;
		myfile << "Route" << ',';
		//首先，检验BestSolution是否为稳定解
		//如果是则输出BestSolution
		if (true== BestSolution.stable_orNot)
		{
			for (int i = 0; i < instance.shipmentNum; i++)
			{
				if (BestSolution.shipment_matchedScheme[i]<instance.Conshipments[i].scheme_Num - 1)
				{
					myfile << instance.Conshipments[i].preferList[BestSolution.shipment_matchedScheme[i]].matched_route + 1 << ',';
				}
				else
				{
					//与自己匹配
					myfile <<"Self" << ',';
				}
			}
			myfile << endl;
			myfile << "Total capacity in route" << ',' << instance.Total_capacity << endl;
			myfile << "Total container number in shipment" << ',' << instance.Total_containerNum<< endl;
			myfile << "Total matched container number" << ',' << instance.Total_containerNum- BestSolution .ConNum_Unmatched<< endl;
			myfile << "Stable or not" << ',' << "True" << endl;
			myfile << "Iteration num" << ',' << iteration_Num << endl;
			myfile << "Restart num" << ',' << restart_Num << endl;
			myfile << "CPU time" << ',' << cpuTime << ',' << "s";
		}
		else
		{
			//否则比较BestSolution和CurrentSolution哪个好，再输出
			if ((BestSolution.ConNum_Unmatched)<=(CurrentSolution.ConNum_Unmatched))
			{
				for (int i = 0; i < instance.shipmentNum; i++)
				{
					if (BestSolution.shipment_matchedScheme[i]<instance.Conshipments[i].scheme_Num - 1)
					{
						myfile << instance.Conshipments[i].preferList[BestSolution.shipment_matchedScheme[i]].matched_route + 1 << ',';
					}
					else
					{
						//与自己匹配
						myfile << "Self" << ',';
					}
				}
				myfile << endl;
				myfile << "Total capacity in route" << ',' << instance.Total_capacity << endl;
				myfile << "Total container number in shipment" << ',' << instance.Total_containerNum << endl;
				myfile << "Total matched container number" << ',' << instance.Total_containerNum - BestSolution.ConNum_Unmatched << endl;;
				myfile << "Stable or not" << ',' << "False" << endl;
				myfile << "Iteration num" << ',' << iteration_Num << endl;
				myfile << "Restart num" << ',' << restart_Num << endl;
				myfile << "CPU time" << ',' << cpuTime << ',' << "s";
			}
			else
			{
				for (int i = 0; i < instance.shipmentNum; i++)
				{
					if (CurrentSolution.shipment_matchedScheme[i]<instance.Conshipments[i].scheme_Num - 1)
					{
						myfile << instance.Conshipments[i].preferList[CurrentSolution.shipment_matchedScheme[i]].matched_route + 1 << ',';
					}
					else
					{
						//与自己匹配
						myfile << "Self" << ',';
					}
				}
				myfile << endl;
				myfile << "Total capacity in route" << ',' << instance.Total_capacity << endl;
				myfile << "Total container number in shipment" << ',' << instance.Total_containerNum << endl;
				myfile << "Total matched container number" << ',' << instance.Total_containerNum - CurrentSolution.ConNum_Unmatched << endl;;
				myfile << "Stable or not" << ',' << "False" << endl;
				myfile << "Iteration num" << ',' << iteration_Num << endl;
				myfile << "Restart num" << ',' << restart_Num << endl;
				myfile << "CPU time" << ',' << cpuTime << ',' << "s";
			}
		}
	}
	else
	{
		cout << "----- IMPOSSIBLE TO OPEN: " << fileName << endl;
	}
}
