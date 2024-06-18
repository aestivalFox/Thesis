#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <E:/m11107617/lcgrand.h>
using namespace std;
double input;
#define link_size 5
#define sys_1_size 50
#define sys_2_size 50
double arr_h = 1.5;
double arr_l0 = 3.5;
double arr_l1 = 1;
double serv_rates_h[link_size] = {4, 3, 3, 2, 2};
double sw01 = 0.5;
double sw10 = 0.5;
// #define dep_at_2 0.25

#define end_target pow(10, 7)
int q_size_array[link_size] = {sys_1_size - 1, sys_2_size - 1, sys_2_size - 1, sys_2_size - 1, sys_2_size - 1 /*, sys_3_size-1*/};
double dep_prob[link_size] = {0.1, 0.1, 0.2, 0.3, 0.3};
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
struct customData
{
	double arr_time = 0;
	int prior_lvl = 100;
	int dep_node = -1;
	unsigned int arr_type;
};
unsigned int mmoo_stat, arr_type, last_stat;
double current_event_time, now, next_event_time, mmoo_arr_rate, next_sw, total_sw1_time, total_sw2_time, last_sw_time;
string current_event, next_event;
struct node
{
	int index = -1;
	unsigned int q_size = 0;
	unsigned int num_arr_l0 = 0;
	unsigned int num_arr_l1 = 0;
	unsigned int num_arr_h = 0;
	unsigned int num_serv_l0 = 0;
	unsigned int num_serv_l1 = 0;
	unsigned int num_serv_h = 0;
	unsigned int l0_block = 0;
	unsigned int l1_block = 0;
	unsigned int rh_block = 0;
	double serv_h = 0;
	double serv_l = 0;
	double previous_event_time = 0;
	d_high_low arr_num, num_in_q, sum_in_q, num_in_sys, sum_in_sys, total_block, num_dep, num_served, num_relayed;
	f_high_low sum_q_time, overall_sum_sys_time, sum_sys_time, time_ave_sum_in_sys, time_ave_sum_in_q;
	customData custom[100];
	bool serverIdle = true;
};
node q[link_size];

node *current_node;
// node relay_node;
// node next_event_node;

//---------------------------------------------------------------------------
// stack
struct event_stack
{
	string name;
	double time;
	int cur_node;
};
vector<event_stack> stack_event;
void stack_push(string event, double event_time, int n_cur)
{
	event_stack es = {event, event_time, n_cur};
	stack_event.push_back(es);
}
void stack_pop()
{
	auto min = std::min_element(stack_event.begin(), stack_event.end(), [](const event_stack &a, const event_stack &b)
								{ return a.time < b.time; });
	int resultIndices = std::distance(std::begin(stack_event), min);
	// auto it = std::minmax_element(stack_event.begin(), stack_event.end());
	// int min_idx = std::distance(stack_event.begin(), it.first);
	current_event = min->name;
	current_event_time = min->time;
	current_node = &q[min->cur_node];
	stack_event.erase(stack_event.begin() + resultIndices);
}
//---------------------------------------------------------------------------
// random variable generator
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
//---------------------------------------------------------------------------
// initialization
void initialization()
{
	stack_event.clear();
	current_event = "";
	next_event = "";
	current_node = NULL;
	for (int i = 0; i < link_size; i++)
	{
		q[i] = {};
		q[i].index = i;
		q[i].serv_h = serv_rates_h[i];
		q[i].serv_l = serv_rates_h[i];
		q[i].q_size = q_size_array[i];
	}
	last_stat = 0;
	total_sw1_time = 0;
	total_sw2_time = 0;
	last_sw_time = 0;

	current_event_time = 0;
	now = 0;
	next_event_time = 0;
	mmoo_stat = 0;
	mmoo_arr_rate = 0;
	next_sw = now + Expon(sw10);
	stack_push("poisson_arr", now + Expon(arr_h), 0);
	stack_push("mmoo_sw10", next_sw, 0);
}
//---------------------------------------------------------------------------
// event
void arr(node &q1, int priority)
{
	double dep_prob_gen = uniform(0, 1);
	int dep_node;
	if (dep_prob_gen < dep_prob[0])
		dep_node = 0;
	else if (dep_prob_gen < dep_prob[0] + dep_prob[1])
		dep_node = 1;
	else if (dep_prob_gen < dep_prob[0] + dep_prob[1] + dep_prob[2])
		dep_node = 2;
	else if (dep_prob_gen < dep_prob[0] + dep_prob[1] + dep_prob[2] + dep_prob[3])
		dep_node = 3;
	else
		dep_node = 4;

	if (arr_type == 0)
		q1.num_arr_l0++;
	else if (arr_type == 1)
		q1.num_arr_l1++;
	else if (arr_type == 2)
		q1.num_arr_h++;

	q1.arr_num.total++;
	if (priority == 1)
	{
		q1.arr_num.high++;
	}
	else if (priority == 2)
	{
		q1.arr_num.low++;
	}
	q1.sum_in_sys.total += q1.num_in_sys.total;
	q1.sum_in_sys.high += q1.num_in_sys.high;
	q1.sum_in_sys.low += q1.num_in_sys.low;
	q1.sum_in_q.total += q1.num_in_q.total;
	q1.sum_in_q.high += q1.num_in_q.high;
	q1.sum_in_q.low += q1.num_in_q.low;

	q1.time_ave_sum_in_sys.total += q1.num_in_sys.total * (now - q1.previous_event_time);
	q1.time_ave_sum_in_sys.high += q1.num_in_sys.high * (now - q1.previous_event_time);
	q1.time_ave_sum_in_sys.low += q1.num_in_sys.low * (now - q1.previous_event_time);
	q1.time_ave_sum_in_q.total += q1.num_in_q.total * (now - q1.previous_event_time);
	q1.time_ave_sum_in_q.high += q1.num_in_q.high * (now - q1.previous_event_time);
	q1.time_ave_sum_in_q.low += q1.num_in_q.low * (now - q1.previous_event_time);

	if (q1.arr_num.total % 1000000 == 0)
		cout << q1.arr_num.total << endl;

	if (q1.serverIdle)
	{
		q1.num_in_sys.total++;
		if (priority == 1)
		{
			q1.num_in_sys.high++;
			if (dep_node != 0)
				stack_push("relay_dep", now + Expon(q1.serv_h), 0);
			else
				stack_push("departure", now + Expon(q1.serv_h), 0);
		}
		else if (priority == 2)
		{
			q1.num_in_sys.low++;
			if (dep_node != 0)
				stack_push("relay_dep", now + Expon(q1.serv_l), 0);
			else
				stack_push("departure", now + Expon(q1.serv_l), 0);
		}

		q1.custom[0] = {now, priority, dep_node, arr_type};
		q1.serverIdle = false;
	}
	else if (q1.num_in_q.total < q1.q_size)
	{
		q1.num_in_sys.total++;
		q1.num_in_q.total++;
		if (priority == 1)
		{
			q1.num_in_sys.high++;
			q1.num_in_q.high++;
		}
		else if (priority == 2)
		{
			q1.num_in_sys.low++;
			q1.num_in_q.low++;
		}
		q1.custom[q1.num_in_q.total] = {now, priority, dep_node, arr_type};
	}
	else // drop
	{
		q1.total_block.total++;
		if (priority == 1)
			q1.total_block.high++;
		else if (priority == 2)
			q1.total_block.low++;

		if (arr_type == 0)
			q1.l0_block++;
		if (arr_type == 1)
			q1.l1_block++;
		if (arr_type == 2)
			q1.rh_block++;
	}
}

void relay_dep(node &q1)
{
	node *q2;
	q2 = &q[q1.index + 1];
	now = current_event_time;

	q1.serverIdle = true;

	q1.time_ave_sum_in_sys.total += q1.num_in_sys.total * (now - q1.previous_event_time);
	q1.time_ave_sum_in_sys.high += q1.num_in_sys.high * (now - q1.previous_event_time);
	q1.time_ave_sum_in_sys.low += q1.num_in_sys.low * (now - q1.previous_event_time);

	q1.num_served.total++;
	q1.num_in_sys.total--;
	q1.sum_sys_time.total += (now - q1.custom[0].arr_time);
	if (q1.custom[0].prior_lvl == 1)
	{
		q1.num_served.high++;
		q1.num_in_sys.high--;
		q1.sum_sys_time.high += (now - q1.custom[0].arr_time);
	}
	else if (q1.custom[0].prior_lvl == 2)
	{
		q1.num_served.low++;
		q1.num_in_sys.low--;
		q1.sum_sys_time.low += (now - q1.custom[0].arr_time);
	}

	if (mmoo_stat == 1 && (q1.custom[0].arr_type == 0 || q1.custom[0].arr_type == 1))
	{
		q2->num_arr_l0++;
		if (last_stat == 2)
		{
			total_sw1_time += now - last_sw_time;
			last_sw_time = now;
		}
		last_stat = 1;
	}
	else if (mmoo_stat == 2 && (q1.custom[0].arr_type == 0 || q1.custom[0].arr_type == 1))
	{
		q2->num_arr_l1++;
		if (last_stat == 1)
		{
			total_sw2_time += now - last_sw_time;
			last_sw_time = now;
		}
		last_stat = 2;
	}
	else if (q1.custom[0].arr_type == 2)
		q2->num_arr_h++;

	q2->arr_num.total++;
	if (q1.custom[0].prior_lvl == 1)
		q2->arr_num.high++;
	else if (q1.custom[0].prior_lvl == 2)
		q2->arr_num.low++;

	q2->sum_in_sys.total += q2->num_in_sys.total;
	q2->sum_in_sys.high += q2->num_in_sys.high;
	q2->sum_in_sys.low += q2->num_in_sys.low;
	q2->sum_in_q.total += q2->num_in_q.total;
	q2->sum_in_q.high += q2->num_in_q.high;
	q2->sum_in_q.low += q2->num_in_q.low;

	q2->time_ave_sum_in_sys.total += q2->num_in_sys.total * (now - q2->previous_event_time);
	q2->time_ave_sum_in_sys.high += q2->num_in_sys.high * (now - q2->previous_event_time);
	q2->time_ave_sum_in_sys.low += q2->num_in_sys.low * (now - q2->previous_event_time);
	q2->time_ave_sum_in_q.total += q2->num_in_q.total * (now - q2->previous_event_time);
	q2->time_ave_sum_in_q.high += q2->num_in_q.high * (now - q2->previous_event_time);
	q2->time_ave_sum_in_q.low += q2->num_in_q.low * (now - q2->previous_event_time);

	/*
		if(q2->num_in_q.total<q2->q_size)
		{
			q1.num_relayed.total++;
			q1.sum_sys_time.total+=(now - q1.custom[0].arr_time);
			if (q1.custom[0].prior_lvl==1)
			{
				q1.num_relayed.high++;
				q1.sum_sys_time.high+=(now - q1.custom[0].arr_time);
			}
			else if (q1.custom[0].prior_lvl==2)
			{
				q1.num_relayed.low++;
				q1.sum_sys_time.low+=(now - q1.custom[0].arr_time);
			}
		}
	*/
	if (q2->serverIdle)
	{
		q2->num_in_sys.total++;
		if (q1.custom[0].prior_lvl == 1)
		{
			q2->num_in_sys.high++;
			if (q1.custom[0].dep_node != q2->index)
				stack_push("relay_dep", now + Expon(q2->serv_h), q2->index);
			else
				stack_push("departure", now + Expon(q2->serv_h), q2->index);
		}
		else if (q1.custom[0].prior_lvl == 2)
		{
			q2->num_in_sys.low++;
			if (q1.custom[0].dep_node != q2->index)
				stack_push("relay_dep", now + Expon(q2->serv_l), q2->index);
			else
				stack_push("departure", now + Expon(q2->serv_l), q2->index);
		}

		// q2->custom[0]=q1.custom[0];
		q2->custom[0] = {now, q1.custom[0].prior_lvl, q1.custom[0].dep_node, q1.custom[0].arr_type};
		q2->serverIdle = false;
	}
	else if (q2->num_in_q.total < q2->q_size)
	{
		q2->num_in_sys.total++;
		q2->num_in_q.total++;
		if (q1.custom[0].prior_lvl == 1)
		{
			q2->num_in_sys.high++;
			q2->num_in_q.high++;
		}
		else if (q1.custom[0].prior_lvl == 2)
		{
			q2->num_in_sys.low++;
			q2->num_in_q.low++;
		}
		// q2->custom[q2->num_in_q.total]=q1.custom[0];
		q2->custom[q2->num_in_q.total] = {now, q1.custom[0].prior_lvl, q1.custom[0].dep_node, q1.custom[0].arr_type};
	}
	else // drop
	{
		q2->total_block.total++;
		if (q1.custom[0].prior_lvl == 1)
			q2->total_block.high++;
		else if (q1.custom[0].prior_lvl == 2)
			q2->total_block.low++;

		if (q1.custom[0].arr_type == 0)
			q2->l0_block++;
		if (q1.custom[0].arr_type == 1)
			q2->l1_block++;
		if (q1.custom[0].arr_type == 2)
			q2->rh_block++;
	}

	if (q1.num_in_q.total > 0)
	{
		for (int i = 0; i < q1.num_in_q.total; i++)
			q1.custom[i] = q1.custom[i + 1];

		q1.time_ave_sum_in_q.total += q1.num_in_q.total * (now - q1.previous_event_time);
		q1.time_ave_sum_in_q.high += q1.num_in_q.high * (now - q1.previous_event_time);
		q1.time_ave_sum_in_q.low += q1.num_in_q.low * (now - q1.previous_event_time);

		q1.num_in_q.total--;
		q1.sum_q_time.total += (now - q1.custom[0].arr_time);
		if (q1.custom[0].prior_lvl == 1)
		{
			q1.num_in_q.high--;
			q1.sum_q_time.high += (now - q1.custom[0].arr_time);
			if (q1.custom[0].dep_node != q1.index)
				stack_push("relay_dep", now + Expon(q1.serv_h), q1.index);
			else
				stack_push("departure", now + Expon(q1.serv_h), q1.index);
		}
		else if (q1.custom[0].prior_lvl == 2)
		{
			q1.num_in_q.low--;
			q1.sum_q_time.low += (now - q1.custom[0].arr_time);
			if (q1.custom[0].dep_node != q1.index)
				stack_push("relay_dep", now + Expon(q1.serv_l), q1.index);
			else
				stack_push("departure", now + Expon(q1.serv_l), q1.index);
		}
		q1.serverIdle = false;
	}

	q1.previous_event_time = now;
	q2->previous_event_time = now;
}
void poisson_arr(node &q1)
{
	now = current_event_time;
	stack_push("poisson_arr", now + Expon(arr_h), 0);
	arr_type = 2;
	arr(q1, 1);
	q1.previous_event_time = now;
}
void mmoo_arr(node &q1)
{
	now = current_event_time;
	if (mmoo_stat == 1)
		arr_type = 0;
	else if (mmoo_stat == 2)
		arr_type = 1;
	arr(q1, 2);
	double next_arr = now + Expon(mmoo_arr_rate);
	if (next_arr < next_sw)
		stack_push("mmoo_arr", next_arr, 0);
	q1.previous_event_time = now;
}
void departure(node &q1)
{
	now = current_event_time;
	q1.num_served.total++;
	q1.num_dep.total++;

	q1.time_ave_sum_in_sys.total += q1.num_in_sys.total * (now - q1.previous_event_time);
	q1.time_ave_sum_in_sys.high += q1.num_in_sys.high * (now - q1.previous_event_time);
	q1.time_ave_sum_in_sys.low += q1.num_in_sys.low * (now - q1.previous_event_time);

	q1.num_in_sys.total--;
	q1.sum_sys_time.total += (now - q1.custom[0].arr_time);
	if (q1.custom[0].prior_lvl == 1)
	{
		q1.num_served.high++;
		q1.num_dep.high++;
		q1.num_in_sys.high--;
		q1.sum_sys_time.high += (now - q1.custom[0].arr_time);
	}
	else if (q1.custom[0].prior_lvl == 2)
	{
		q1.num_served.low++;
		q1.num_dep.low++;
		q1.num_in_sys.low--;
		q1.sum_sys_time.low += (now - q1.custom[0].arr_time);
	}

	if (mmoo_stat == 1 && (q1.custom[0].arr_type == 0 || q1.custom[0].arr_type == 1))
	{
		q1.num_serv_l0++;
	}
	else if (mmoo_stat == 2 && (q1.custom[0].arr_type == 0 || q1.custom[0].arr_type == 1))
	{
		q1.num_serv_l1++;
	}
	else if (q1.custom[0].arr_type == 2)
		q1.num_serv_h++;

	q1.serverIdle = true;

	if (q1.num_in_q.total > 0)
	{
		for (int i = 0; i < q1.num_in_q.total; i++)
			q1.custom[i] = q1.custom[i + 1];

		q1.time_ave_sum_in_q.total += q1.num_in_q.total * (now - q1.previous_event_time);
		q1.time_ave_sum_in_q.high += q1.num_in_q.high * (now - q1.previous_event_time);
		q1.time_ave_sum_in_q.low += q1.num_in_q.low * (now - q1.previous_event_time);

		q1.num_in_q.total--;
		q1.sum_q_time.total += (now - q1.custom[0].arr_time);
		if (q1.custom[0].prior_lvl == 1)
		{
			q1.num_in_q.high--;
			q1.sum_q_time.high += (now - q1.custom[0].arr_time);
			if (q1.custom[0].dep_node != q1.index)
				stack_push("relay_dep", now + Expon(q1.serv_h), q1.index);
			else
				stack_push("departure", now + Expon(q1.serv_h), q1.index);
		}
		else if (q1.custom[0].prior_lvl == 2)
		{
			q1.num_in_q.low--;
			q1.sum_q_time.low += (now - q1.custom[0].arr_time);
			if (q1.custom[0].dep_node != q1.index)
				stack_push("relay_dep", now + Expon(q1.serv_l), q1.index);
			else
				stack_push("departure", now + Expon(q1.serv_l), q1.index);
		}
		q1.serverIdle = false;
	}
	q1.previous_event_time = now;
}
void mmoo_sw10(node &q1)
{

	now = current_event_time;
	mmoo_stat = 1;
	mmoo_arr_rate = arr_l0;
	next_sw = now + Expon(sw01);
	double next_arr = now + Expon(mmoo_arr_rate);
	stack_push("mmoo_sw01", next_sw, q1.index);
	if (mmoo_arr_rate > 0 && next_arr < next_sw)
		stack_push("mmoo_arr", next_arr, 0);
}
void mmoo_sw01(node &q1)
{
	now = current_event_time;
	mmoo_stat = 2;
	mmoo_arr_rate = arr_l1;
	next_sw = now + Expon(sw10);
	double next_arr = now + Expon(mmoo_arr_rate);
	stack_push("mmoo_sw10", next_sw, q1.index);
	if (mmoo_arr_rate > 0 && next_arr < next_sw)
		stack_push("mmoo_arr", next_arr, 0);
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
	else if (event == "relay_dep") // q[0],q[1]
		return 6;
	else
		return 0;
}
//---------------------------------------------------------------------------
// print metrics
/**/
void print_metrics()
{
	ofstream ofs;
	ofs.open("C:\\Users\\T2-401\\Desktop\\sim_GEO2LEOLink.csv", ios::app); // T2-401//lksh4//ethan chen
	f_high_low th[link_size], pb[link_size], L[link_size], Lq[link_size], W[link_size], Wq[link_size];
	f_high_low th_total, pb_total, L_total, Lq_total, W_total, Wq_total;
	f_high_low pb_numer, pb_denom, W_numer, W_denom, Wq_numer, Wq_denom;
	for (int i = 0; i < link_size; i++)
	{
		printf("============================\n");
		printf("q[%d]high priority average arr num = %f\n", i, q[i].num_arr_h / now);
		printf("q[%d]low priority rate 0 average arr num = %f\n", i, q[i].num_arr_l0 / now);
		printf("q[%d]low priority rate 1 average arr num = %f\n", i, q[i].num_arr_l1 / now);
		printf("q[%d]low priority rate 0 average serv num = %f\n", i, q[i].num_serv_l0 / now);
		printf("q[%d]low priority rate 1 average serv num = %f\n", i, q[i].num_serv_l1 / now);

		th[i].total = q[i].num_dep.total / now;
		th[i].high = q[i].num_dep.high / now;
		th[i].low = q[i].num_serv_l0 / now + q[i].num_serv_l1 / now;

		pb[i].total = q[i].total_block.total / (double)q[i].arr_num.total;
		pb[i].high = q[i].total_block.high / (double)q[i].arr_num.high;
		pb[i].low = q[i].total_block.low / (double)q[i].arr_num.low;

		L[i].total = q[i].time_ave_sum_in_sys.total / now;
		L[i].high = q[i].time_ave_sum_in_sys.high / now;
		L[i].low = q[i].time_ave_sum_in_sys.low / now;

		Lq[i].total = q[i].time_ave_sum_in_q.total / now;
		Lq[i].high = q[i].time_ave_sum_in_q.high / now;
		Lq[i].low = q[i].time_ave_sum_in_q.low / now;

		W[i].total = q[i].sum_sys_time.total / q[i].num_served.total;
		W[i].high = q[i].sum_sys_time.high / q[i].num_served.high;
		W[i].low = q[i].sum_sys_time.low / q[i].num_served.low;

		Wq[i].total = q[i].sum_q_time.total / q[i].num_served.total;
		Wq[i].high = q[i].sum_q_time.high / q[i].num_served.high;
		Wq[i].low = q[i].sum_q_time.low / q[i].num_served.low;

		pb[1].total = (q[1].rh_block + q[1].l0_block + q[1].l1_block) / (double)q[1].arr_num.total;
		pb[1].high = q[1].rh_block / (double)q[1].num_arr_h;
		pb[1].low = (q[1].l0_block + q[1].l1_block) / (double)(q[1].num_arr_l0 + q[1].num_arr_l1);

		printf("q_%d_th_total 	= %f\nq_%d_th_high 	= %f\nq_%d_th_low 	= %f\n"
			   "q_%d_pb_total 	= %f\nq_%d_pb_high 	= %f\nq_%d_pb_low 	= %f\n"
			   "q_%d_L_total 	= %f\nq_%d_L_high 	= %f\nq_%d_L_low 	= %f\n"
			   "q_%d_Lq_total 	= %f\nq_%d_Lq_high 	= %f\nq_%d_Lq_low 	= %f\n"
			   "q_%d_W_total 	= %f\nq_%d_W_high 	= %f\nq_%d_W_low 	= %f\n"
			   "q_%d_Wq_total 	= %f\nq_%d_Wq_high 	= %f\nq_%d_Wq_low 	= %f\n",
			   i, th[i].total, i, th[i].high, i, th[i].low,
			   i, pb[i].total, i, pb[i].high, i, pb[i].low,
			   i, L[i].total, i, L[i].high, i, L[i].low,
			   i, Lq[i].total, i, Lq[i].high, i, Lq[i].low,
			   i, W[i].total, i, W[i].high, i, W[i].low,
			   i, Wq[i].total, i, Wq[i].high, i, Wq[i].low);

		th_total.total += q[i].num_dep.total / now;
		th_total.high += q[i].num_dep.high / now;
		th_total.low += q[i].num_serv_l0 / now + q[i].num_serv_l1 / now;

		pb_numer.total += q[i].total_block.total;
		pb_numer.high += q[i].total_block.high;
		pb_numer.low += q[i].total_block.low;

		pb_denom.total += q[i].arr_num.total;
		pb_denom.high += q[i].arr_num.high;
		pb_denom.low += q[i].arr_num.low;

		L_total.total += q[i].time_ave_sum_in_sys.total / now;
		L_total.high += q[i].time_ave_sum_in_sys.high / now;
		L_total.low += q[i].time_ave_sum_in_sys.low / now;

		Lq_total.total += q[i].time_ave_sum_in_q.total / now;
		Lq_total.high += q[i].time_ave_sum_in_q.high / now;
		Lq_total.low += q[i].time_ave_sum_in_q.low / now;

		W_numer.total += q[i].sum_sys_time.total;
		W_numer.high += q[i].sum_sys_time.high;
		W_numer.low += q[i].sum_sys_time.low;

		W_denom.total += q[i].num_served.total;
		W_denom.high += q[i].num_served.high;
		W_denom.low += q[i].num_served.low;

		Wq_numer.total += q[i].sum_q_time.total;
		Wq_numer.high += q[i].sum_q_time.high;
		Wq_numer.low += q[i].sum_q_time.low;

		Wq_denom.total += q[i].num_served.total;
		Wq_denom.high += q[i].num_served.high;
		Wq_denom.low += q[i].num_served.low;

		/*	*/
		W_total.total += W[i].total;
		W_total.high += W[i].high;
		W_total.low += W[i].low;

		Wq_total.total += Wq[i].total;
		Wq_total.high += Wq[i].high;
		Wq_total.low += Wq[i].low;

		/*
				ofs << th[i].total << "," << th[i].high << "," << th[i].low << ","
					<< pb[i].total << "," << pb[i].high << "," << pb[i].low << ","
					<< L[i].total << "," << L[i].high << "," << L[i].low << ","
					<< Lq[i].total << "," << Lq[i].high << "," << Lq[i].low << ","
					<< W[i].total << "," << W[i].high << "," << W[i].low << ","
					<< Wq[i].total << "," << Wq[i].high << "," << Wq[i].low << "\n";
		*/
	}

	pb_total.total = pb_numer.total / q[0].arr_num.total;
	pb_total.high = pb_numer.high / q[0].arr_num.high;
	pb_total.low = pb_numer.low / q[0].arr_num.low;

	// pb[1].total = q[1].total_block.total/(double)q[1].arr_num.total;

	// pb[1].low 	= q[1].total_block.low/(double)q[1].arr_num.low;

	/*
		W_total.total=q[1].sum_sys_time.total/q[1].num_served.total;
		W_total.high=q[1].sum_sys_time.high/q[1].num_served.high;
		W_total.low=q[1].sum_sys_time.low/q[1].num_served.low;

		Wq_total.total=q[1].sum_q_time.total/q[1].num_served.total;
		Wq_total.high=q[1].sum_q_time.high/q[1].num_served.high;
		Wq_total.low=q[1].sum_q_time.low/q[1].num_served.low;
	*/
	/**/
	cout << "==============================\n";
	printf("th_total= %f\nth_high\t= %f\nth_low\t= %f\n"
		   "pb_total= %f\npb_high\t= %f\npb_low\t= %f\n"
		   "L_total\t= %f\nL_high\t= %f\nL_low\t= %f\n"
		   "Lq_total= %f\nLq_high\t= %f\nLq_low\t= %f\n"
		   "W_total\t= %f\nW_high\t= %f\nW_low\t= %f\n"
		   "Wq_total= %f\nWq_high\t= %f\nWq_low\t= %f\n",
		   th_total.total, th_total.high, th_total.low,
		   pb_total.total, pb_total.high, pb_total.low,
		   L_total.total, L_total.high, L_total.low,
		   Lq_total.total, Lq_total.high, Lq_total.low,
		   W_total.total, W_total.high, W_total.low,
		   Wq_total.total, Wq_total.high, Wq_total.low);

	/**/
	ofs << th_total.total << "," << th_total.high << "," << th_total.low << ","
		<< pb_total.total << "," << pb_total.high << "," << pb_total.low << ","
		<< L_total.total << "," << L_total.high << "," << L_total.low << ","
		<< Lq_total.total << "," << Lq_total.high << "," << Lq_total.low << ","
		<< W_total.total << "," << W_total.high << "," << W_total.low << ","
		<< Wq_total.total << "," << Wq_total.high << "," << Wq_total.low << "\n";

	ofs.close();

	printf("sw1 time =%f\n", total_sw1_time / now);
	printf("sw1 time =%f\n", total_sw2_time / now);
	/*
	printf("th_total = %f\nth_high = %f\nth_low = %f\n"\
			"pb_total = %f\npb_high = %f\npb_low = %f\n"\
			"L_total = %f\nL_high = %f\nL_low = %f\n"\
			"Lq_total = %f\nLq_high = %f\nLq_low = %f\n"\
			"W_total = %f\nW_high = %f\nW_low = %f\n"\
			"Wq_total = %f\nWq_high = %f\nWq_low = %f\n",
			th_total.total,	th_total.high,	th_total.low,
			pb_total.total,	pb_total.high,	pb_total.low,
			L_total.total,	L_total.high,		L_total.low,
			Lq_total.total,	Lq_total.high,	Lq_total.low,
			W_total.total,	W_total.high,		W_total.low,
			Wq_total.total,	Wq_total.high,	Wq_total.low

	);
	*/
	/**/
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
			serv_rates_h[0] = begin;
			printf("\nserv_rates_h[0] = %f\n", serv_rates_h[0]);
			break;
		case 4:
			serv_rates_h[1] = begin;
			printf("\nserv_rates_h[1] = %f\n", serv_rates_h[1]);
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
		cout << "type\n";
		// cin >> input;

		initialization();

		while (q[0].arr_num.total < end_target)
		{
			stack_pop();
			// cout << current_event << "\n";
			switch (event_map(current_event))
			{
			case 1:
				mmoo_arr(*current_node);
				break;
			case 2:
				mmoo_sw10(*current_node);
				break;
			case 3:
				mmoo_sw01(*current_node);
				break;
			case 4:
				poisson_arr(*current_node);
				break;
			case 5:
				departure(*current_node);
				break;
			case 6:
				relay_dep(*current_node);
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
	system("pause");
	return 0;
}
