//hgs�㷨�Ĳ��������������Ϊȫ�ֱ���


#ifndef CONF_H_
#define CONF_H_

#include <string>
#include <climits>


#define MAXCELLLEN  102400			//��ȡCSV����Ҫ�ĳ�������CSV�ļ�������û���ر��ʱ���ó������ø�
#define MY_EPSILON 0.00001			//����һ���ȼ���0�ĳ�����������ֵ�Ա�ʱ�ľ�������
#define MY_MAXNUM 999999999			//���ڱ�ʾһ���㹻�������



class Conf
{
public:
	//������ص�
	static int MaxNum_preferList;				//class Route������preferList�е����ƥ�䷽������
	//�㷨��ص�
	static int MaxNum_iter;						//���������������
	static int MaxNum_incompleteStable;			//���ַ�ȫƥ�������������
	static double probability_randomWalk;		//random walk�ĸ���

};


#endif /* CONF_H_ */