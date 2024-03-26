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

	//�������ȡ�㷨�����Լ���������ļ�
	CommandLine(const string & instance_nework, const string & instance_shipment, const string & instance_route, const string & instance_cooperation, const string & result);

	//չʾ���㷨�����õĲ���
	void print_algorithm_parameters();

};
#endif

