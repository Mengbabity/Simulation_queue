#include<iostream>
#include<queue>
#include<vector>
#include<math.h>
#include<Windows.h>
#include<time.h>
#include<stdlib.h>
#include "customer.h"
#include "event.h"
#include "service.h"

using namespace std;

queue<customer> customers;

int wholetime = 540;
service s1,s2,s3;
vector<event> events;
vector<int> customers_count(10,0);   //每次仿真的总人数
vector<double> wholetime_of_wait(10,0);   //每次仿真的总的等待时间
vector<double> whole_length(10, 0);   //每次仿真总的队长
int i = 0;   //仿真序号
vector<double> average_wait_time;   //每次仿真的平均等待时间
vector<double> average_wait_length;   //每次仿真的平均队长


//顾客到达时间/接受服务时间随机变量
int Possion(double parameter)
{
	int result = 0;
	double n;

	while (result == 0)
	{
		srand((unsigned int)time(NULL) + rand());
		n = rand()*(1.0 / RAND_MAX);
		result = (int)(-log(n) * parameter);
	}

	return result;
}

//取号机取号，生成顾客序列
void queue_generator(int wholetime, double parameter1, double parameter2)
{
	customer c;
	int num = 1;
	int current_time = 0;
	while (current_time < wholetime)
	{
		c.arrive_time = current_time+Possion(parameter1);
		if (c.arrive_time >= wholetime)
			break;
		c.service_time = Possion(parameter2);
		c.number = num;
		num++;
		current_time = c.arrive_time;
		customers.push(c);
	}

/*	for (int i = 0; i < customers.size(); i++)
	{
		cout << customers.front().arrive_time<<" "<<customers.front().number << " " << endl;
		customers.pop();
	}*/
}

//事件推进,表明在什么时间，第几个顾客到达/离开，并输出事件
void event_generator(bool type,int time,int count,customer c)
{
	event e;
	e.type = type;
	e.time = time;
	e.count = count;
	e.c = c;
	events.push_back(e);

}

//仿真过程
void process()
{
	int current_time = 0;
	while (current_time < wholetime && customers.size() != 0)
	{
		customer c = customers.front();   //当前被叫到号的顾客

		if (s1.service_customers.size() == 0)   //服务台没人
		{
			c.leave_time = c.arrive_time + c.service_time;
			c.service_number = 1;
			s1.busy_or_not.push_back(1);
			s1.service_customers.push(c);
		
		}
		else     //服务台有人
		{
			customer c1 = s1.service_customers.back();

			if (c1.leave_time > c.arrive_time)   //需要在服务台1排队
			{
				if (s2.service_customers.size() == 0)   //服务台2没人
				{
					c.leave_time = c.arrive_time + c.service_time;
					c.service_number = 2;
					s2.busy_or_not.push_back(1);
					s2.service_customers.push(c);
				}
				else
				{
					customer c2 = s2.service_customers.back();

					if (c2.leave_time > c.arrive_time)   //需要在服务台2排队
					{
						if ((s1.service_customers.size()+s2.service_customers.size()) > 3)   //如果当前排队顾客人数超过3人，开启服务台3
						{
							if (s3.service_customers.size() == 0)   //服务台没人
							{
								c.leave_time = c.arrive_time + c.service_time;
								c.service_number = 3;
								s3.busy_or_not.push_back(1);
								s3.service_customers.push(c);
							}
							else
							{
								customer c3 = s3.service_customers.back();

								if (c1.leave_time < c2.leave_time)
								{
									if (c1.leave_time < c3.leave_time)
									{
										c.leave_time = c1.leave_time + c.service_time;
										c.service_number = 1;
										s1.busy_or_not.push_back(1);
										s1.service_customers.push(c);
									}
									else
									{
										if (c3.leave_time < c2.leave_time)
										{
											c.leave_time = c3.leave_time + c.service_time;
											c.service_number = 3;
											s3.busy_or_not.push_back(1);
											s3.service_customers.push(c);
										}
									}
								}
								else
								{
									if (c2.leave_time < c3.leave_time)
									{
										c.leave_time = c2.leave_time + c.service_time;
										c.service_number = 2;
										s2.busy_or_not.push_back(1);
										s2.service_customers.push(c);
									}
									else
									{
										c.leave_time = c3.leave_time + c.service_time;
										c.service_number = 3;
										s3.busy_or_not.push_back(1);
										s3.service_customers.push(c);
									}
								}
							}
						}
						else                                     //不需要开启服务台3
						{
							if (c1.leave_time < c2.leave_time)   //服务台1的排队时间比较短
							{
								c.leave_time = c1.leave_time + c.service_time;
								c.service_number = 1;
								s1.busy_or_not.push_back(1);
								s1.service_customers.push(c);
							}
							else                                //服务台2的排队时间比较短
							{
								c.leave_time = c2.leave_time + c.service_time;
								c.service_number = 2;
								s2.busy_or_not.push_back(1);
								s2.service_customers.push(c);
							}
						}
					}
					else              //不需要在服务台2排队
					{
						c.leave_time = c.arrive_time + c.service_time;
						c.service_number = 2;
						s2.busy_or_not.push_back(1);
						s2.service_customers.push(c);
					}
				}

			}
			else         //不需要在服务台1排队
			{
				c.leave_time = c.arrive_time + c.service_time;
				c.service_number = 1;
				s1.busy_or_not.push_back(1);
				s1.service_customers.push(c);
			}
		}

		current_time = c.arrive_time;   //当前时间取该顾客到达时间，只要该顾客到达时银行没有下班，就可继续接受顾客
		int length = s1.service_customers.size() + s2.service_customers.size()+s3.service_customers.size();
		event_generator(0, c.arrive_time, length-1,c);
		event_generator(1, c.leave_time, 0,c);

//		event_generator(0, c.arrive_time, c);
//		event_generator(1, c.leave_time, c);
		customers.pop();

		while (s1.service_customers.size() != 0)
		{
			if (s1.service_customers.front().leave_time < current_time)
				s1.service_customers.pop();
			else
				break;
		}

		while (s2.service_customers.size() != 0)
		{
			if (s2.service_customers.front().leave_time < current_time)
				s2.service_customers.pop();
			else
				break;
		}

		while (s3.service_customers.size() != 0)
		{
			if (s3.service_customers.front().leave_time < current_time)
				s3.service_customers.pop();
			else
				break;
		}
	}
}

void print()
{
	double average_wait = 0;
	int average_length = 0;
	sort(events.begin(), events.end(), [](event a, event b) {return a.time < b.time; });
	for (auto e:events)
	{
		int minute = e.time % 60;
		int hour = e.time / 60 + 9;
		if (e.type == 0)
		{
			Sleep(50);
			whole_length[i] += e.count;
			if (minute >= 10)
				cout << "The #" << e.c.number << " customer arrived at " << hour << ":" << minute 
				<< ",the length of queue was "<< e.count << endl;
			else
				cout << "The #" << e.c.number << " customer arrived at " << hour << ":0" << minute 
				<< ",the length of queue was " << e.count << endl;
		}
		else
		{
			int time_of_wait = e.c.leave_time - e.c.arrive_time - e.c.service_time;
			if (time_of_wait < 0)
				time_of_wait = 0;
			wholetime_of_wait[i] += time_of_wait;
			customers_count[i] ++ ;
			Sleep(50);
			if (minute >= 10)
			    cout << "The #" << e.c.number << " customer was serviced at #" << e.c.service_number << " Service,and left at "
				<< hour << ":" << minute << ",waited " << time_of_wait << endl;
			else
				cout << "The #" << e.c.number << " customer was serviced at #" << e.c.service_number << " Service,and left at "
				<< hour << ":0" << minute << ",waited " << time_of_wait << endl;
		}
	}

	cout << "********************" << endl;
	//顾客总人数
	cout << "The #" << i+1 << " simulation had " << customers_count[i] << " customers" << endl;
	//服务台忙/闲状态
	int busy1 = 0, busy2 = 0, busy3 = 0;
	for (int i = 0; i < s1.busy_or_not.size(); i++)
	{
		if (s1.busy_or_not[i] == 1)
			busy1++;
	}
	for (int i = 0; i < s2.busy_or_not.size(); i++)
	{
		if (s2.busy_or_not[i] == 1)
			busy2++;
	}
	for (int i = 0; i < s3.busy_or_not.size(); i++)
	{
		if (s3.busy_or_not[i] == 1)
			busy3++;
	}
	cout << "#1 Service has " << busy1 << " customers." << endl;
	cout << "#2 Service has " << busy2 << " customers." << endl;
	cout << "#3 Service has " << busy3 << " customers." << endl;
	//平均等待时间
	average_wait = (double)wholetime_of_wait[i] / customers_count[i];
	cout << "The #" << i+1 << " simulation's average time is " << average_wait << endl;
	average_wait_time.push_back(average_wait);
	//平均队长
	average_length = (double)whole_length[i] / customers_count[i];
	cout<< "The #" << i + 1 << " simulation's average length of queue is " << average_length << endl;
	average_wait_length.push_back(average_length);
	cout << "********************" << endl;
}

void initial()
{
	queue<customer> initial;
	customers = initial;
	s1.service_customers = initial;
	s2.service_customers = initial;
	s3.service_customers = initial;
	events.clear();
}

int main()
{
	int n = 10;   //仿真次数
	double sum_wait = 0.0, avg_wait=0.0, var_wait=0.0;
	double sum_length = 0.0, avg_length=0.0, var_length=0.0;
	for (int j = 0; j < n; j++)
	{
		initial();
		queue_generator(wholetime, 5.0, 10);
		process();
		print();
		i++;   //仿真序号
	}
	for (int x = 0; x < average_wait_time.size(); x++)
	{
		sum_wait += average_wait_time[x];
		cout << "The #" << x << " simulation's average time is " << average_wait_time[x] << endl;
	}
	avg_wait = sum_wait / i;   //i次仿真等待时间均值

	for (int x = 0; x < average_wait_time.size(); x++)
	{
		var_wait += pow(avg_wait - average_wait_time[x], 2);   
	}
	var_wait /= i;//方差

	for (int y = 0; y < average_wait_length.size(); y++)
	{
		sum_length+=average_wait_length[y];   
		cout << "The #" << y << " simulation's average length of queue is " << average_wait_length[y] << endl;
	}
	avg_length = sum_length / i;   //i次仿真平均队长
	for (int y = 0; y < average_wait_length.size(); y++)
	{
		var_length += pow(average_wait_length[y] - avg_length, 2);
	}
	var_length /= i;   //方差

	cout << "********************" << endl;
	cout << "The average of waiting time is " << avg_wait << endl;
	cout << "The variance of waiting time is " << var_wait << endl;
	cout << "The average of length of waiting queue is " << avg_length << endl;
	cout << "The variance of length of waiting queue is " << var_length << endl;

	system("pause");
	return 0;
}
