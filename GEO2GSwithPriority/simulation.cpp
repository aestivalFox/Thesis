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
double arr_h = 1.5;
double arr_l0 = 3.5;
double arr_l1 = 1;
double serv_h = 4;
double serv_l = 4;
double sw01 = 0.5;
#define sw10 0.5

#define q_size sys_size - 1
#define end_target pow(10, 7)
//---------------------------------------------------------------------------
// 變數宣告
struct d_high_low
{
	unsigned int total = 0;
	unsigned int high = 0;
	unsigned int low = 0;
};
struct f_high_low
{
	double total = 0;
	double high = 0;
	double low = 0;
};
d_high_low arr_num, dep_num, block_num, num_in_sys, num_in_q, total_left_q;
f_high_low sum_sys_time, sum_q_time, time_ave_sum_in_sys, time_ave_sum_in_q;
unsigned int mmoo_stat;
double current_event_time, now, next_event_time, mmoo_arr_rate, next_sw, previous_event_time;
string current_event, next_event;
bool serverIdle;
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
// custom data
struct customData
{
	double arr_time = 0;
	int prior_lvl = 100;
};
customData custom[q_size + 1];

//---------------------------------------------------------------------------
// initialization
void initialization()
{
	arr_num.total = 0;
	dep_num.total = 0;
	block_num.total = 0;
	num_in_sys.total = 0;
	num_in_q.total = 0;
	total_left_q.total = 0;
	sum_sys_time.total = 0;
	sum_q_time.total = 0;
	time_ave_sum_in_sys.total = 0;
	time_ave_sum_in_q.total = 0;

	arr_num.high = 0;
	dep_num.high = 0;
	block_num.high = 0;
	num_in_sys.high = 0;
	num_in_q.high = 0;
	total_left_q.high = 0;
	sum_sys_time.high = 0;
	sum_q_time.high = 0;
	time_ave_sum_in_sys.high = 0;
	time_ave_sum_in_q.high = 0;

	arr_num.low = 0;
	dep_num.low = 0;
	block_num.low = 0;
	num_in_sys.low = 0;
	num_in_q.low = 0;
	total_left_q.low = 0;
	sum_sys_time.low = 0;
	sum_q_time.low = 0;
	time_ave_sum_in_sys.low = 0;
	time_ave_sum_in_q.low = 0;

	for (int i = 0; i < q_size + 1; i++)
		custom[i] = {0, 100};

	stack_event.clear();
	current_event = "";
	next_event = "";
	stack_event.shrink_to_fit();
	stack_event.reserve(100);
	current_event_time = 0;
	previous_event_time = 0;
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
	if (arr_h > 0)
		stack_push("poisson_arr", now + rv.exponential(arr_h));
}
//---------------------------------------------------------------------------
// event
void arr(int priority)
{
	arr_num.total++;

	if (arr_num.total % 1000000 == 0)
		cout << arr_num.total << endl;

	if (priority == 1)
	{
		arr_num.high++;
	}
	else if (priority == 2)
	{
		arr_num.low++;
	}
	time_ave_sum_in_sys.total += num_in_sys.total * (now - previous_event_time);
	time_ave_sum_in_sys.high += num_in_sys.high * (now - previous_event_time);
	time_ave_sum_in_sys.low += num_in_sys.low * (now - previous_event_time);
	time_ave_sum_in_q.total += num_in_q.total * (now - previous_event_time);
	time_ave_sum_in_q.high += num_in_q.high * (now - previous_event_time);
	time_ave_sum_in_q.low += num_in_q.low * (now - previous_event_time);

	if (serverIdle)
	{
		num_in_sys.total++;
		if (priority == 1)
		{
			num_in_sys.high++;
			stack_push("departure", now + rv.exponential(serv_h));
		}
		else if (priority == 2)
		{
			num_in_sys.low++;
			stack_push("departure", now + rv.exponential(serv_l));
		}
		else
		{
			cout << "arr_hriority_error";
			system("pause");
		}
		// total_left_q.total++;
		custom[0] = {now, priority};

		serverIdle = false;
	}
	else if (num_in_q.total < q_size)
	{
		num_in_sys.total++;
		num_in_q.total++;
		if (priority == 1)
		{
			num_in_sys.high++;
			num_in_q.high++;
		}
		else if (priority == 2)
		{
			num_in_sys.low++;
			num_in_q.low++;
		}
		custom[num_in_q.total] = {now, priority};
		for (int i = num_in_q.total; i > 1; i--)
		{
			if (custom[i].prior_lvl < custom[i - 1].prior_lvl)
			{
				customData tmp;
				tmp = custom[i - 1];
				custom[i - 1] = custom[i];
				custom[i] = tmp;
			}
			else
				break;
		}
	}
	else // block
	{
		block_num.total++;
		if (priority == 1)
			block_num.high++;
		else if (priority == 2)
			block_num.low++;
	}
}
void poisson_arr()
{
	now = current_event_time;
	if (arr_h > 0)
		stack_push("poisson_arr", now + rv.exponential(arr_h));
	arr(1);
	previous_event_time = now;
}
void mmoo_arr()
{
	now = current_event_time;
	double next_arr = now + rv.exponential(mmoo_arr_rate);
	if (next_arr < next_sw && mmoo_arr_rate > 0)
		stack_push("mmoo_arr", next_arr);
	arr(2);
	previous_event_time = now;
}
void departure()
{
	now = current_event_time;

	time_ave_sum_in_sys.total += num_in_sys.total * (now - previous_event_time);
	time_ave_sum_in_sys.high += num_in_sys.high * (now - previous_event_time);
	time_ave_sum_in_sys.low += num_in_sys.low * (now - previous_event_time);

	dep_num.total++;
	num_in_sys.total--;
	sum_sys_time.total += (now - custom[0].arr_time);
	if (custom[0].prior_lvl == 1)
	{
		dep_num.high++;
		num_in_sys.high--;
		sum_sys_time.high += (now - custom[0].arr_time);
	}
	else if (custom[0].prior_lvl == 2)
	{
		dep_num.low++;
		num_in_sys.low--;
		sum_sys_time.low += (now - custom[0].arr_time);
	}
	else
	{
		cout << "depart prior error";
		system("pause");
	}
	serverIdle = true;

	if (num_in_q.total > 0)
	{
		for (int i = 0; i < num_in_q.total; i++)
		{
			custom[i] = custom[i + 1];
		}
		time_ave_sum_in_q.total += num_in_q.total * (now - previous_event_time);
		time_ave_sum_in_q.high += num_in_q.high * (now - previous_event_time);
		time_ave_sum_in_q.low += num_in_q.low * (now - previous_event_time);

		if (custom[0].prior_lvl == 1)
			stack_push("departure", now + rv.exponential(serv_h));
		else if (custom[0].prior_lvl == 2)
			stack_push("departure", now + rv.exponential(serv_l));

		num_in_q.total--;
		total_left_q.total++;
		sum_q_time.total += (now - custom[0].arr_time);
		if (custom[0].prior_lvl == 1)
		{
			num_in_q.high--;
			total_left_q.high++;
			sum_q_time.high += (now - custom[0].arr_time);
		}
		else if (custom[0].prior_lvl == 2)
		{
			num_in_q.low--;
			total_left_q.low++;
			sum_q_time.low += (now - custom[0].arr_time);
		}

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
	mmoo_arr_rate = arr_l0;
	double next_arr = now + rv.exponential(mmoo_arr_rate);
	if (mmoo_arr_rate > 0 && next_arr < next_sw)
		stack_push("mmoo_arr", next_arr);
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
	mmoo_arr_rate = arr_l1;
	double next_arr = now + rv.exponential(mmoo_arr_rate);
	if (mmoo_arr_rate > 0 && next_arr < next_sw)
		stack_push("mmoo_arr", next_arr);
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
	f_high_low th, pb, L, Lq, W, Wq;
	th.total = (double)dep_num.total / now;
	th.high = (double)dep_num.high / now;
	th.low = (double)dep_num.low / now;

	pb.total = block_num.total / (double)arr_num.total;
	pb.high = block_num.high / (double)arr_num.high;
	pb.low = block_num.low / (double)arr_num.low;

	L.total = time_ave_sum_in_sys.total / now;
	L.high = time_ave_sum_in_sys.high / now;
	L.low = time_ave_sum_in_sys.low / now;

	Lq.total = time_ave_sum_in_q.total / now;
	Lq.high = time_ave_sum_in_q.high / now;
	Lq.low = time_ave_sum_in_q.low / now;

	W.total = sum_sys_time.total / (double)dep_num.total;
	W.high = sum_sys_time.high / (double)dep_num.high;
	W.low = sum_sys_time.low / (double)dep_num.low;

	Wq.total = sum_q_time.total / (double)dep_num.total;
	Wq.high = sum_q_time.high / (double)dep_num.high;
	Wq.low = sum_q_time.low / (double)dep_num.low;

	printf("th_total = %f\nth_high = %f\nth_low = %f\n"
		   "pb_total = %f\npb_high = %f\npb_low = %f\n"
		   "L_total = %f\nL_high = %f\nL_low = %f\n"
		   "Lq_total = %f\nLq_high = %f\nLq_low = %f\n"
		   "W_total = %f\nW_high = %f\nW_low = %f\n"
		   "Wq_total = %f\nWq_high = %f\nWq_low = %f\n",
		   th.total, th.high, th.low,
		   pb.total, pb.high, pb.low,
		   L.total, L.high, L.low,
		   Lq.total, Lq.high, Lq.low,
		   W.total, W.high, W.low,
		   Wq.total, Wq.high, Wq.low);
	ofstream ofs;
	ofs.open("C:\\Users\\T2-401\\Desktop\\sim_GEO2GSwithPriority.csv", ios::app); // T2-401//lksh4//ethan chen
	ofs << th.total << ","
		<< th.high << ","
		<< th.low << ","
		<< pb.total << ","
		<< pb.high << ","
		<< pb.low << ","
		<< L.total << ","
		<< L.high << ","
		<< L.low << ","
		<< Lq.total << ","
		<< Lq.high << ","
		<< Lq.low << ","
		<< W.total << ","
		<< W.high << ","
		<< W.low << ","
		<< Wq.total << ","
		<< Wq.high << ","
		<< Wq.low << "\n";

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
			arr_h = begin;
			printf("\narr_h = %f\n", arr_h);
			break;
		case 2:
			arr_l0 = begin;
			printf("\narr_l0 = %f\n", arr_l0);
			break;
		case 3:
			serv_h = begin;
			serv_l = begin;
			printf("\nserv_rates_h[0] = %f\n", serv_h);
			break;
		case 4:
			serv_h = begin;
			serv_l = begin;
			printf("\nserv_rates_h[1] = %f\n", serv_l);
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
		while (arr_num.total < end_target)
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
