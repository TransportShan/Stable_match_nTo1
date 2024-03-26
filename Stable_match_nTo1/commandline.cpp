#include "stdafx.h"
#include "commandline.h"



CommandLine::CommandLine(const string & instance_nework, const string & instance_shipment, const string & instance_route, const string & instance_cooperation, const string & result)
{
	pathInstance_network = instance_nework;
	pathInstance_shipment = instance_shipment;
	pathInstance_route = instance_route;
	pathInstance_cooperation = instance_cooperation;
	pathSolution = result;

	cout << endl;
	cout << "------------------------ Local search algorithm for container-route stable matching(2023) -----------------------------" << endl;
	cout << "Read instance from " << pathInstance_network << "," << pathInstance_shipment << "," << pathInstance_route << "," << pathInstance_cooperation << endl;
	cout << "Write result in " << pathSolution << endl;
	cout << "-----------------------------------------------------------------------------------------------------------------------" << endl;
	cout << endl;
}

void CommandLine::print_algorithm_parameters()
{
	cout << "============================= Algorithm Parameters =================================" << endl;
	cout << "---- Maximum number of iterations                    is set to " << Conf::MaxNum_iter << endl;
	cout << "---- Maximum number of incomplete stable matching    is set to " << Conf::MaxNum_incompleteStable << endl;
	cout << "---- probability of random walk                      is set to " << Conf::probability_randomWalk << endl;
	cout << "====================================================================================" << endl;
	cout << endl;
}

