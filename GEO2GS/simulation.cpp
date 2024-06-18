#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <E:/m11107617/lcgrand.h>
using namespace std;
#define sys_size 50
double arr_p = 1.5;
double arr_0 = 3.5;
#define arr_1 1
double serv_p = 4;
double serv_s = 4;
double sw01 = 0.5;
#define sw10 0.5

#define q_length sys_size - 1
#define end_target pow(10, 7)
//---------------------------------------------------------------------------
// 變數宣告
struct dataType_f
{
	double total = 0;
	double poisson = 0;
	double mmoo = 0;
	double mmoo_0 = 0;
	double mmoo_1 = 0;
};
struct dataType_d
{
	unsigned int total = 0;
	unsigned int poisson = 0;
	unsigned int mmoo_0 = 0;
	unsigned int mmoo_1 = 0;
};
dataType_d num_arr, num_block, num_dep, num_left_q, num_in_sys, num_in_q;
dataType_f sum_sys_time, sum_q_time, time_sum_in_sys, time_sum_in_q;
unsigned int mmoo_stat;
double current_event_time, previous_event_time, now, next_event_time, mmoo_arr_rate, next_sw;
string current_event, next_event;
bool serverIdle;
struct customData
{
	double arr_time = 0;
	double arr_type = -1;
};
customData custom[sys_size];
//---------------------------------------------------------------------------
// stack
struct event_stack
{
	string name;
	double time;
};
vector<event_stack> stack_event;
void stack_push(string event, double event_time)
{
	event_stack es = {event, event_time};
	stack_event.push_back(es);
}
void stack_pop()
{
	auto min = std::min_element(stack_event.begin(), stack_event.end(), [](const event_stack &a, const event_stack &b)
								{ return a.time < b.time; });
	int resultIndices = std::distance(std::begin(stack_event), min);
	current_event = min->name;
	current_event_time = min->time;
	stack_event.erase(stack_event.begin() + resultIndices);
}
//---------------------------------------------------------------------------
// random variable generator
class randomVariable
{
public:
	double uniform(double a, double b)
	{
		int n;
		double x;
		n = rand() % (int)pow(2.0, 15);
		x = a + (b - a) * n / pow(2.0, 15);
		return x;
	}
	double exponential(double c)
	{
		double x, y;
		x = uniform(0, 1);
		if (x <= 1e-10)
		{
			x = 1e-10;
		}
		y = -log(1 - x) / c;
		return (y);
	}
} rv;
//---------------------------------------------------------------------------
// initialization
void initialization()
{
	num_arr.total = 0;
	num_block.total = 0;
	num_dep.total = 0;
	num_left_q.total = 0;
	num_in_sys.total = 0;
	num_in_q.total = 0;
	sum_sys_time.total = 0;
	sum_q_time.total = 0;
	time_sum_in_sys.total = 0;
	time_sum_in_q.total = 0;

	sum_sys_time.mmoo = 0;
	sum_q_time.mmoo = 0;
	time_sum_in_sys.mmoo = 0;
	time_sum_in_q.mmoo = 0;

	num_arr.mmoo_0 = 0;
	num_block.mmoo_0 = 0;
	num_dep.mmoo_0 = 0;
	num_left_q.mmoo_0 = 0;
	num_in_sys.mmoo_0 = 0;
	num_in_q.mmoo_0 = 0;
	sum_sys_time.mmoo_0 = 0;
	sum_q_time.mmoo_0 = 0;
	time_sum_in_sys.mmoo_0 = 0;
	time_sum_in_q.mmoo_0 = 0;

	num_arr.mmoo_1 = 0;
	num_block.mmoo_1 = 0;
	num_dep.mmoo_1 = 0;
	num_left_q.mmoo_1 = 0;
	num_in_sys.mmoo_1 = 0;
	num_in_q.mmoo_1 = 0;
	sum_sys_time.mmoo_1 = 0;
	sum_q_time.mmoo_1 = 0;
	time_sum_in_sys.mmoo_1 = 0;
	time_sum_in_q.mmoo_1 = 0;

	num_arr.poisson = 0;
	num_block.poisson = 0;
	num_dep.poisson = 0;
	num_left_q.poisson = 0;
	num_in_sys.poisson = 0;
	num_in_q.poisson = 0;
	sum_sys_time.poisson = 0;
	sum_q_time.poisson = 0;
	time_sum_in_sys.poisson = 0;
	time_sum_in_q.poisson = 0;

	stack_event.clear();
	stack_event.shrink_to_fit();
	stack_event.reserve(100);
	current_event_time = 0;
	now = 0;
	next_event_time = 0;
	serverIdle = true;
	mmoo_stat = 0;
	mmoo_arr_rate = 0;
	if (sw01 > 0)
	{
		next_sw = now + rv.exponential(sw01);
		stack_push("mmoo_sw01", next_sw);
	}
	else if (sw10 > 0)
	{
		next_sw = now + rv.exponential(sw10);
		stack_push("mmoo_sw10", next_sw);
	}
	if (arr_p > 0)
		stack_push("poisson_arr", now + rv.exponential(arr_p));
}
//---------------------------------------------------------------------------
// event
void arr(int type)
{
	num_arr.total++;
	if (type == 2)
		num_arr.poisson++;
	else if (mmoo_stat == 1)
		num_arr.mmoo_0++;
	else if (mmoo_stat == 2)
		num_arr.mmoo_1++;

	if (num_arr.total % 1000000 == 0)
		cout << num_arr.total << endl;
	time_sum_in_sys.total += num_in_sys.total * (now - previous_event_time);
	time_sum_in_sys.poisson += num_in_sys.poisson * (now - previous_event_time);
	time_sum_in_sys.mmoo_0 += num_in_sys.mmoo_0 * (now - previous_event_time);
	time_sum_in_sys.mmoo_1 += num_in_sys.mmoo_1 * (now - previous_event_time);

	time_sum_in_q.total += num_in_q.total * (now - previous_event_time);
	time_sum_in_q.poisson += num_in_q.poisson * (now - previous_event_time);
	time_sum_in_q.mmoo_0 += num_in_q.mmoo_0 * (now - previous_event_time);
	time_sum_in_q.mmoo_1 += num_in_q.mmoo_1 * (now - previous_event_time);

	if (serverIdle)
	{
		num_in_sys.total++;
		if (type == 2)
			num_in_sys.poisson++;
		else if (mmoo_stat == 1)
			num_in_sys.mmoo_0++;
		else if (mmoo_stat == 2)
			num_in_sys.mmoo_1++;
		if (type == 2)
			stack_push("departure", now + rv.exponential(serv_p));
		else
			stack_push("departure", now + rv.exponential(serv_s));
		num_left_q.total++;
		custom[0].arr_time = now;
		custom[0].arr_type = type;
		serverIdle = false;
	}
	else if (num_in_q.total < q_length)
	{
		num_in_sys.total++;
		num_in_q.total++;
		if (type == 2)
		{
			num_in_sys.poisson++;
			num_in_q.poisson++;
		}

		else if (mmoo_stat == 1)
		{
			num_in_sys.mmoo_0++;
			num_in_q.mmoo_0++;
		}

		else if (mmoo_stat == 2)
		{
			num_in_sys.mmoo_1++;
			num_in_q.mmoo_1++;
		}

		custom[num_in_q.total].arr_time = now;
		custom[num_in_q.total].arr_type = type;
	}
	else // block
	{
		num_block.total++;
		if (type == 2)
			num_block.poisson++;
		else if (mmoo_stat == 1)
			num_block.mmoo_0++;
		else if (mmoo_stat == 2)
			num_block.mmoo_1++;
	}
}
void poisson_arr()
{
	now = current_event_time;
	if (arr_p > 0)
		stack_push("poisson_arr", now + rv.exponential(arr_p));
	arr(2);
	previous_event_time = now;
}
void mmoo_arr()
{
	now = current_event_time;
	double next_arr = now + rv.exponential(mmoo_arr_rate);
	if (next_arr < next_sw && mmoo_arr_rate > 0)
		stack_push("mmoo_arr", next_arr);
	int arr_type;
	if (mmoo_stat == 1)
		arr_type = 0;
	else if (mmoo_stat == 2)
		arr_type = 1;
	arr(arr_type);
	previous_event_time = now;
}
void departure()
{
	now = current_event_time;

	time_sum_in_sys.total += num_in_sys.total * (now - previous_event_time);
	time_sum_in_sys.poisson += num_in_sys.poisson * (now - previous_event_time);
	time_sum_in_sys.mmoo_0 += num_in_sys.mmoo_0 * (now - previous_event_time);
	time_sum_in_sys.mmoo_1 += num_in_sys.mmoo_1 * (now - previous_event_time);

	serverIdle = true;
	num_in_sys.total--;
	if (custom[0].arr_type == 0)
		num_in_sys.mmoo_0--;
	else if (custom[0].arr_type == 1)
		num_in_sys.mmoo_1--;
	else if (custom[0].arr_type == 2)
		num_in_sys.poisson--;

	sum_sys_time.total += (now - custom[0].arr_time);
	if (mmoo_stat == 1 && (custom[0].arr_type == 0 || custom[0].arr_type == 1))
		sum_sys_time.mmoo_0 += (now - custom[0].arr_time);
	else if (mmoo_stat == 2 && (custom[0].arr_type == 0 || custom[0].arr_type == 1))
		sum_sys_time.mmoo_1 += (now - custom[0].arr_time);
	else if (custom[0].arr_type == 2)
		sum_sys_time.poisson += (now - custom[0].arr_time);

	num_dep.total++;
	if (mmoo_stat == 1 && (custom[0].arr_type == 0 || custom[0].arr_type == 1))
		num_dep.mmoo_0++;
	else if (mmoo_stat == 2 && (custom[0].arr_type == 0 || custom[0].arr_type == 1))
		num_dep.mmoo_1++;
	else if (custom[0].arr_type == 2)
		num_dep.poisson++;

	custom[0].arr_time = 0;
	custom[0].arr_type = -1;
	if (num_in_q.total > 0)
	{

		time_sum_in_q.total += num_in_q.total * (now - previous_event_time);
		time_sum_in_q.poisson += num_in_q.poisson * (now - previous_event_time);
		time_sum_in_q.mmoo_0 += num_in_q.mmoo_0 * (now - previous_event_time);
		time_sum_in_q.mmoo_1 += num_in_q.mmoo_1 * (now - previous_event_time);

		for (int i = 0; i < num_in_q.total; i++)
			custom[i] = custom[i + 1];

		num_in_q.total--;
		num_left_q.total++;

		if (custom[0].arr_type == 0)
		{
			num_in_q.mmoo_0--;
			num_left_q.mmoo_0++;
		}
		else if (custom[0].arr_type == 1)
		{
			num_in_q.mmoo_1--;
			num_left_q.mmoo_1++;
		}
		else if (custom[0].arr_type == 2)
		{
			num_in_q.poisson--;
			num_left_q.poisson++;
		}

		sum_q_time.total += (now - custom[0].arr_time);
		if (mmoo_stat == 1 && (custom[0].arr_type == 0 || custom[0].arr_type == 1))
			sum_q_time.mmoo_0 += (now - custom[0].arr_time);
		else if (mmoo_stat == 2 && (custom[0].arr_type == 0 || custom[0].arr_type == 1))
			sum_q_time.mmoo_1 += (now - custom[0].arr_time);
		else if (custom[0].arr_type == 2)
			sum_q_time.poisson += (now - custom[0].arr_time);

		if (custom[0].arr_type == 2)
			stack_push("departure", now + rv.exponential(serv_p));
		else
			stack_push("departure", now + rv.exponential(serv_s));
		serverIdle = false;
	}
	previous_event_time = now;
}
void mmoo_sw10()
{
	now = current_event_time;
	mmoo_stat = 1;
	if (sw01 > 0)
	{
		next_sw = now + rv.exponential(sw01);
		stack_push("mmoo_sw01", next_sw);
	}
	else
		next_sw = INFINITY;
	mmoo_arr_rate = arr_0;
	double next_arr = now + rv.exponential(mmoo_arr_rate);
	if (mmoo_arr_rate > 0 && next_arr < next_sw)
		stack_push("mmoo_arr", next_arr);
	// previous_event_time=now;
}
void mmoo_sw01()
{
	now = current_event_time;
	mmoo_stat = 2;
	if (sw10 > 0)
	{
		next_sw = now + rv.exponential(sw10);
		stack_push("mmoo_sw10", next_sw);
	}
	else
		next_sw = INFINITY;
	mmoo_arr_rate = arr_1;
	double next_arr = now + rv.exponential(mmoo_arr_rate);
	if (mmoo_arr_rate > 0 && next_arr < next_sw)
		stack_push("mmoo_arr", next_arr);
	// previous_event_time=now;
}
//---------------------------------------------------------------------------
// event mapping
int event_map(string event)
{
	if (event == "mmoo_arr")
		return 1;
	else if (event == "mmoo_sw10")
		return 2;
	else if (event == "mmoo_sw01")
		return 3;
	else if (event == "poisson_arr")
		return 4;
	else if (event == "departure")
		return 5;
	else
		return 0;
}
//---------------------------------------------------------------------------
// print metrics
void print_metrics()
{
	dataType_f th, pb, L, Lq, W, Wq, th_h, th_l0, th_l1;
	th.total = (double)num_dep.total / now;
	th.poisson = (double)num_dep.poisson / now;
	th.mmoo = (double)(num_dep.mmoo_0 + num_dep.mmoo_1) / now;

	pb.total = num_block.total / (double)num_arr.total;
	pb.poisson = num_block.poisson / (double)num_arr.poisson;
	pb.mmoo = (num_block.mmoo_0 + num_block.mmoo_1) / (double)(num_arr.mmoo_0 + num_arr.mmoo_1);

	L.total = time_sum_in_sys.total / now;
	L.poisson = time_sum_in_sys.poisson / now;
	L.mmoo = (time_sum_in_sys.mmoo_0 + time_sum_in_sys.mmoo_1) / now;

	Lq.total = time_sum_in_q.total / now;
	Lq.poisson = time_sum_in_q.poisson / now;
	Lq.mmoo = (time_sum_in_q.mmoo_0 + time_sum_in_q.mmoo_1) / now;

	W.total = sum_sys_time.total / (double)num_dep.total;
	W.poisson = sum_sys_time.poisson / (double)num_dep.poisson;
	W.mmoo = (sum_sys_time.mmoo_0 + sum_sys_time.mmoo_1) / (double)(num_dep.mmoo_0 + num_dep.mmoo_1);

	Wq.total = sum_q_time.total / (double)num_dep.total;
	Wq.poisson = sum_q_time.poisson / (double)num_dep.poisson;
	Wq.mmoo = (sum_q_time.mmoo_0 + sum_q_time.mmoo_1) / (double)(num_dep.mmoo_0 + num_dep.mmoo_1);

	printf("th.total = %f\nth.poisson = %f\nth.mmoo = %f\n", th.total, th.poisson, th.mmoo);
	printf("pb.total = %f\npb.poisson = %f\npb.mmoo = %f\n", pb.total, pb.poisson, pb.mmoo);
	printf("L.total = %f\nL.poisson = %f\nL.mmoo = %f\n", L.total, L.poisson, L.mmoo);
	printf("Lq.total = %f\nLq.poisson = %f\nLq.mmoo = %f\n", Lq.total, Lq.poisson, Lq.mmoo);
	printf("W.total = %f\nW.poisson = %f\nW.mmoo = %f\n", W.total, W.poisson, W.mmoo);
	printf("Wq.total = %f\nWq.poisson = %f\nWq.mmoo = %f\n", Wq.total, Wq.poisson, Wq.mmoo);
	ofstream ofs;
	ofs.open("C:\\Users\\T2-401\\Desktop\\sim_GEO2GS.csv", ios::app); // T2-401//lksh4//ethan chen
	ofs << th.total << "," << th.poisson << "," << th.mmoo << ","
		<< pb.total << "," << pb.poisson << "," << pb.mmoo << ","
		<< L.total << "," << L.poisson << "," << L.mmoo << ","
		<< Lq.total << "," << Lq.poisson << "," << Lq.mmoo << ","
		<< W.total << "," << W.poisson << "," << W.mmoo << ","
		<< Wq.total << "," << Wq.poisson << "," << Wq.mmoo << "\n";

	ofs.close();
}
//---------------------------------------------------------------------------
// main
int main()
{
	printf("[1] : arr_h\n[2] : arr_l0\n[3] : serv_1\n[4] : serv_2\n[5] : sw01\n");
	int option;
	double begin;
	double interval;
	int count = 1;
	printf("option");
	cin >> option;
	printf("\nstart from :");
	cin >> begin;
	printf("\ninterval :");
	cin >> interval;

	while (count <= 10)
	{

		switch (option)
		{
		case 1:
			arr_p = begin;
			printf("\narr_h = %f\n", arr_p);
			break;
		case 2:
			arr_0 = begin;
			printf("\narr_l0 = %f\n", arr_0);
			break;
		case 3:
			serv_p = begin;
			serv_s = begin;
			printf("\nserv_rates_h[0] = %f\n", serv_p);
			break;
		case 4:
			serv_p = begin;
			serv_s = begin;
			printf("\nserv_rates_h[1] = %f\n", serv_s);
			break;
		case 5:
			sw01 = begin;
			printf("\nsw01 = %f\n", sw01);
			break;
		default:
			printf("\ninput error");
			system("pause");
			return 0;
			break;
		}
		srand(time(NULL));
		initialization();
		while (num_arr.total < end_target)
		{
			stack_pop();
			// cout << current_event << "\n";
			switch (event_map(current_event))
			{
			case 1:
				mmoo_arr();
				break;
			case 2:
				mmoo_sw10();
				break;
			case 3:
				mmoo_sw01();
				break;
			case 4:
				poisson_arr();
				break;
			case 5:
				departure();
				break;
			default:
				cout << "event mapping error\n";
				system("pause");
				return 0;
			}
		}
		cout << "done\n";
		print_metrics();
		count++;
		begin += interval;
	}
	return 0;
}
