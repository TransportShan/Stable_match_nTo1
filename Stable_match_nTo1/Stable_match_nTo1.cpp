// Stable_match_nTo1.cpp : �������̨Ӧ�ó������ڵ㡣
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				         	local search for container-route stable matching							  //
//											WX SHAN														  //
//											2023-08-06													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <ctime>
#include <string>

#include "Conf.h"
#include "commandline.h"
#include "InstanceLIB.h"
#include "StableMatching.h"

using namespace std;

int main()
{
	//����ʱ�����ӣ���������������������������Ҫʹ���������ֻ��Ҫ��main������һ������
	srand((unsigned)time(NULL));
	//����/�������ʱ���ļ�������
	string data_network_fileName;			//��������ļ�·��
	string data_shipment_fileName;			//��������ļ�·��
	string data_route_fileName;				//��������ļ�·��
	string data_cooperation_fileName;		//��������ļ�·��
	string result_fileName;					//������ļ�·��
											//����㷨��ʱ�� 
	clock_t start;							//��¼�㷨�Ŀ�ʼʱ��
	clock_t end;							//��¼�㷨�Ľ���ʱ��

	//���벿��/////////////////////////////////////////////////////////////////////////
	data_network_fileName = "./data./network.csv";
	data_shipment_fileName = "./data./shipment.csv";
	data_route_fileName = "./data./route.csv";
	data_cooperation_fileName = "./data./cooperation parameter.csv";
	result_fileName = "./result./stable matching.csv";
	//////////////////////////////////////////////////////////////////////////////////
	start = clock();
	try
	{
		//��һ�������㷨����+��������ļ�·��
		CommandLine commandline(data_network_fileName,data_shipment_fileName, data_route_fileName, data_cooperation_fileName, result_fileName);

		//չʾ�㷨����
		commandline.print_algorithm_parameters();

		//�ڶ�����������
		cout << "----- READING INSTANCE -----" << endl;
		cout <<  endl;
		InstanceLIB CRSM(commandline.pathInstance_network,commandline.pathInstance_shipment, commandline.pathInstance_route, commandline.pathInstance_cooperation);

		//�������ȶ�ƥ�����
		cout << "----- STABLE MATCHING -----" << endl;
		cout << endl;

		// Running Local Search for container-route stable matching
		StableMatching LS(CRSM);
		LS.run();

		// Exporting the best solution
		if (true== LS.BestSolution.stable_orNot)
		{
			cout << "----- Stable Matching Found -----" << endl;
			cout << endl;
		}
		else
		{
			cout << "----- No Stable Matching Found -----" <<endl;
			cout << endl;
		}
		cout << "----- WRITING BEST SOLUTION -----" << endl;
		cout << endl;
		end = clock();
		LS.exportBestSolution(commandline.pathSolution, (float)(end - start) / CLOCKS_PER_SEC);
	}
	catch (const string& e) { cout << "EXCEPTION | " << e << endl; }
	catch (const exception& e) { cout << "EXCEPTION | " << e.what() << endl; }
	cout << "CPU time is: " << (float)(end - start) / CLOCKS_PER_SEC << "S" << endl;

	return 0;
}

