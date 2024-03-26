//hgs算法的参数都放在这里，作为全局变量


#ifndef CONF_H_
#define CONF_H_

#include <string>
#include <climits>


#define MAXCELLLEN  102400			//读取CSV所需要的常数，当CSV文件的数据没有特别多时，该常数不用改
#define MY_EPSILON 0.00001			//定义一个等价于0的常量，用于数值对比时的精度设置
#define MY_MAXNUM 999999999			//用于表示一个足够大的正数



class Conf
{
public:
	//数组相关的
	static int MaxNum_preferList;				//class Route中数组preferList中的最大匹配方案数量
	//算法相关的
	static int MaxNum_iter;						//整体的最大迭代次数
	static int MaxNum_incompleteStable;			//出现非全匹配的最大迭代次数
	static double probability_randomWalk;		//random walk的概率

};


#endif /* CONF_H_ */