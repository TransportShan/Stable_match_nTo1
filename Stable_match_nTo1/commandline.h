#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <iostream>
#include <string>

#include "Conf.h"

using namespace std;

class CommandLine
{
public:
	string pathInstance_network;				//Instance path
	string pathInstance_shipment;				//Instance path
	string pathInstance_route;					//Instance path
	string pathInstance_cooperation;			//Instance path
	string pathSolution;						//Solution path

	//从输入读取算法参数以及输入输出文件
	CommandLine(const string & instance_nework, const string & instance_shipment, const string & instance_route, const string & instance_cooperation, const string & result);

	//展示该算法中设置的参数
	void print_algorithm_parameters();

};
#endif

