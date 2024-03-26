#include "stdafx.h"
#include "StableMatching.h"

#include <fstream>
#include <string>
#include <iostream>

bool StableMatching::search_Blockscheme(MatchingSol & targetSol)
{
	targetSol.blockingNum = 0;
	targetSol.ConNum_Inblocking = 0;
	//���ô�route������ÿ�����ܷ���
	bool block_ornot;
	for (int i = 0; i < instance.routeNum; i++)
	{
		//����instance.Conroutes[i].preferList�б�instance.Conroutes[i].preferList[instance.Conroutes[i].matched_scheme]���õķ���
		for (int j = 0; j < instance.Conroutes[i].matched_scheme; j++)
		{
			block_ornot = true;
			//����÷������ڣ���ô���ȸ÷�����route����ͬ���
			//��У��÷�����instance.Conroutes[i].preferList[j].matched_shipment�е�����shipment�Ƿ�ͬ��
			for (int k = 0; k < instance.Conroutes[i].preferList[j].scheme_shipmentNum; k++)
			{
				if (false== instance.Conshipments[instance.Conroutes[i].preferList[j].matched_shipment[k]].check_schemeOrder(i))
				{
					//�÷�����ֻҪ��һ��shipment��ͬ�⣬��ô�Ͳ���Blockscheme
					block_ornot = false;
					break;
				}
			}
			if (true == block_ornot)
			{
				//����instance.Conroutes[i].preferList[j]ΪBlockscheme
				targetSol.Blocking_Schemes[targetSol.blockingNum].route_index = instance.Conroutes[i].Rindex;
				targetSol.Blocking_Schemes[targetSol.blockingNum].scheme_order = j;
				targetSol.ConNum_Inblocking = targetSol.ConNum_Inblocking + instance.Conroutes[i].preferList[j].scheme_totaldemand;
				targetSol.blockingNum = targetSol.blockingNum + 1;
				//ÿ��route��ֻ��¼һ��Blockscheme
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
	//���ȼ�¼��ǰ��
	for (int i = 0; i < instance.shipmentNum; i++)
	{
		targetSol.shipment_matchedScheme[i] = instance.Conshipments[i].matched_scheme;
		if (targetSol.shipment_matchedScheme[i]<instance.Conshipments[i].scheme_Num-1)
		{
			//��ĳ��routeƥ��
			targetSol.matchedShipment_Num = targetSol.matchedShipment_Num + 1;
		}
		else
		{
			//û�в���ƥ��
			targetSol.ConNum_Unmatched = targetSol.ConNum_Unmatched + instance.Conshipments[i].demand;
		}
	}
	for (int i = 0; i < instance.routeNum; i++)
	{
		targetSol.route_matchedScheme[i] = instance.Conroutes[i].matched_scheme;
		if (targetSol.route_matchedScheme[i] < instance.Conroutes[i].scheme_Num-1)
		{
			//��ĳЩshipmentƥ��
			targetSol.matchedRoute_Num = targetSol.matchedRoute_Num + 1;
		}
	}
	search_Blockscheme(targetSol);
}

void StableMatching::Insertblock_intoSol(Blockscheme bs)
{
	//˼·
	//1����instance.Conroutes[bs.route_index]ѡ���bs.scheme_order��������ͬʱinstance.Conroutes[bs.route_index].preferList[bs.scheme_order].matched_shipment��shipments
	//ѡ��bs.route_index���ڷ���
	//2����instance.Conroutes[bs.route_index].preferList[bs.scheme_order].matched_shipment��shipment��preferList����bs.route_index���ڷ���֮ǰ�ķ��������Ϊ�ܾ�
	//3��������shipment��preferList��bs.route_index���ڷ������Ϊ�ܾ�
	//4������instance.generateSolution()����

	//��������route��shipment�ľܾ��б�
	instance.reset_selection();

	//Ȼ��ָ��blocking bs�е�shipment��route����ѡ��
	instance.assign_scheme(bs.route_index,bs.scheme_order);
	
	//���Ÿ���blocking bs������shipment�ľܾ��б�
	for (int i = 0; i < instance.shipmentNum; i++)
	{
		if (1== instance.Conroutes[bs.route_index].preferList[bs.scheme_order].matched_Sindicator[i])
		{
			//���instance.Conshipments[i]��blocking bs�У�����instance.Conshipments[i].matched_scheme�������ߵķ������Ϊ�ܾ�
			for (int j = 0; j < instance.Conshipments[i].matched_scheme; j++)
			{
				instance.Conshipments[i].preferList_state[j] = 1;
			}
		}
		else
		{
			//������shipment
			int temp = instance.Conshipments[i].get_schemeOrder(bs.route_index);
			if (temp>=0)
			{
				instance.Conshipments[i].preferList_state[temp] = 1;
			}
		}
	}

	//���ʼshipment����+route�ܾ�����
	instance.generateSolution();
}

void StableMatching::run()
{
	//��һ�����ɳ�ʼƥ���
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
	//��ѭ��
	do
	{
		//�ڶ�������Ѿ��ҵ�ȫƥ����ȶ�ƥ�����ֱ�����
		if (0==(CurrentSolution.ConNum_Unmatched + CurrentSolution.ConNum_Inblocking))
		{
			Copy_Sol(CurrentSolution, BestSolution);
			break;
		}
		RandomWalk_temp = double(rand() % 100) / 100;
		if (RandomWalk_temp <=Conf::probability_randomWalk && CurrentSolution.blockingNum>=1)
		{
			Random_block = rand() % CurrentSolution.blockingNum;
			//��������CurrentSolution.Blocking_Schemes[Random_block]����CurrentSolution�У�����µĽ�
			Insertblock_intoSol(CurrentSolution.Blocking_Schemes[Random_block]);
			//��¼�ý⵽CurrentSolution
			get_MatchingSol(CurrentSolution);
		}
		else
		{
			//���ģ���CurrentSolutionΪһ���ȶ�ƥ���	
			if (true== CurrentSolution.stable_orNot)
			{
				if ((CurrentSolution.ConNum_Unmatched + CurrentSolution.ConNum_Inblocking) <= (BestSolution.ConNum_Unmatched + BestSolution.ConNum_Inblocking))
				{
					//���CurrentSolution��Ŀ�꺯����BestSolution��,�͸���BestSolution
					Copy_Sol(CurrentSolution, BestSolution);
				}
				//���壬�������һ����
				instance.generateSolution_Byrandom();
				//��¼�ý⵽CurrentSolution
				get_MatchingSol(CurrentSolution);
				restart_Num++;
			}
			else
			{
				//������Select neighbor and evaluate adjacent matching
				double best_Fvalue = MY_MAXNUM;
				Blockscheme bestblocking;
				//����ÿ��CurrentSolution.Blocking_Schemes[i]���뵱ǰ��CurrentSolution
				for (int i = 0; i < CurrentSolution.blockingNum; i++)
				{
					Insertblock_intoSol(CurrentSolution.Blocking_Schemes[i]);
					//��¼�ý⵽TempSolution
					get_MatchingSol(TempSolution);
					if ((TempSolution.ConNum_Unmatched + TempSolution.ConNum_Inblocking)<best_Fvalue)
					{
						//���¸�TempSolution��BestSolution
						bestblocking.route_index = CurrentSolution.Blocking_Schemes[i].route_index;
						bestblocking.scheme_order = CurrentSolution.Blocking_Schemes[i].scheme_order;
						best_Fvalue = TempSolution.ConNum_Unmatched + TempSolution.ConNum_Inblocking;
					}
				}
				Insertblock_intoSol(bestblocking);
				//��¼�ý⵽CurrentSolution
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
		//���ȣ�����BestSolution�Ƿ�Ϊ�ȶ���
		//����������BestSolution
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
					//���Լ�ƥ��
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
			//����Ƚ�BestSolution��CurrentSolution�ĸ��ã������
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
						//���Լ�ƥ��
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
						//���Լ�ƥ��
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
