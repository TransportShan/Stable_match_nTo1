// Stable_match_nTo1.cpp : 定义控制台应用程序的入口点。
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
	//设置时间种子，无论是主函数还是其他函数体要使用随机数，只需要在main中设置一次种子
	srand((unsigned)time(NULL));
	//输入/输出数据时，文件名参数
	string data_network_fileName;			//输入参数文件路径
	string data_shipment_fileName;			//输入参数文件路径
	string data_route_fileName;				//输入参数文件路径
	string data_cooperation_fileName;		//输入参数文件路径
	string result_fileName;					//求解结果文件路径
											//监控算法的时间 
	clock_t start;							//记录算法的开始时间
	clock_t end;							//记录算法的结束时间

	//输入部分/////////////////////////////////////////////////////////////////////////
	data_network_fileName = "./data./network.csv";
	data_shipment_fileName = "./data./shipment.csv";
	data_route_fileName = "./data./route.csv";
	data_cooperation_fileName = "./data./cooperation parameter.csv";
	result_fileName = "./result./stable matching.csv";
	//////////////////////////////////////////////////////////////////////////////////
	start = clock();
	try
	{
		//第一，读入算法参数+输入输出文件路径
		CommandLine commandline(data_network_fileName,data_shipment_fileName, data_route_fileName, data_cooperation_fileName, result_fileName);

		//展示算法参数
		commandline.print_algorithm_parameters();

		//第二，载入算例
		cout << "----- READING INSTANCE -----" << endl;
		cout <<  endl;
		InstanceLIB CRSM(commandline.pathInstance_network,commandline.pathInstance_shipment, commandline.pathInstance_route, commandline.pathInstance_cooperation);

		//第三，稳定匹配过程
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

