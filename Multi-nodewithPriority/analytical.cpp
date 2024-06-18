#include <iostream>
#include <math.h>
#include "first_q.cpp"
#include <fstream>
using namespace std;

#define sys_size_limit 50
#define link_size 5
#define sys_1_size 50
#define sys_2_size 50
double arr_1_h = 1.5;
double arr_1_l0 = 3.5;
double arr_1_l1 = 1;
double serv_rates_h[link_size] = {4, 3, 3, 2, 2};
double sw01_1 = 0.5;
#define sw10_1 0.5
double dept[link_size] = {0.1, 1 / 9.0, 0.25, 0.5, 1};
struct f_high_low
{
    double total = 0;
    double high = 0;
    double low = 0;
};

int iter_cnt;
double deviation;

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
            arr_1_h = begin;
            printf("\narr_h = %f\n", arr_1_h);
            break;
        case 2:
            arr_1_l0 = begin;
            printf("\narr_l0 = %f\n", arr_1_l0);
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
            sw01_1 = begin;
            printf("\nsw01 = %f\n", sw01_1);
            break;
        default:
            printf("\ninput error");
            system("pause");
            return 0;
            break;
        }

        first_q q[link_size];
        q[0].sys_size = sys_1_size;
        q[0].arr_h = arr_1_h;
        q[0].arr_l0 = arr_1_l0;
        q[0].arr_l1 = arr_1_l1;
        q[0].serv_h = serv_rates_h[0];
        q[0].serv_l = serv_rates_h[0];
        q[0].sw01 = sw01_1;
        q[0].sw10 = sw10_1;
        q[0].dept = dept[0];
        q[0].iteration();

        printf("end\niter_count:%d\n", q[0].iter_cnt);
        q[0].calc_metrics();
        printf("q0_th:%f\nq0_th_h:%f\nq0_th_l:%f\n"
               "q0_pb:%f\nq0_pb_h:%f\nq0_pb_l:%f\n"
               "q0_L:%f\nq0_L_h:%f\nq0_L_l:%f\n"
               "q0_Lq:%f\nq0_Lq_h:%f\nq0_Lq_l:%f\n"
               "q0_W:%f\nq0_W_h:%f\nq0_W_l:%f\n"
               "q0_Wq:%f\nq0_Wq_h:%f\nq0_Wq_l:%f\n",
               q[0].th_servd.total, q[0].th_servd.high, q[0].th_servd.low,
               q[0].pb.total, q[0].pb.high, q[0].pb.low,
               q[0].L.total, q[0].L.high, q[0].L.low,
               q[0].Lq.total, q[0].Lq.high, q[0].Lq.low,
               q[0].W.total, q[0].W.high, q[0].W.low,
               q[0].Wq.total, q[0].Wq.high, q[0].Wq.low);

        for (int i = 1; i < link_size; i++)
        {
            q[i].sys_size = sys_2_size;
            q[i].arr_h = q[i - 1].th_relay.high;
            q[i].arr_l0 = q[i - 1].th_l0_relay / (q[i - 1].sw10 / (q[i - 1].sw01 + q[i - 1].sw10));
            q[i].arr_l1 = q[i - 1].th_l1_relay / (q[i - 1].sw01 / (q[i - 1].sw01 + q[i - 1].sw10));

            q[i].serv_h = serv_rates_h[i];
            q[i].serv_l = serv_rates_h[i];
            q[i].sw01 = sw01_1;
            q[i].sw10 = sw10_1;
            q[i].dept = dept[i];

            q[i].iteration();
            printf("end\niter_count:%d\n", q[i].iter_cnt);
            q[i].calc_metrics();

            printf("q1_th:%f\nq1_th_h:%f\nq1_th_l:%f\n"
                   "q1_pb:%f\nq1_pb_h:%f\nq1_pb_l:%f\n"
                   "q1_L:%f\nq1_L_h:%f\nq1_L_l:%f\n"
                   "q1_Lq:%f\nq1_Lq_h:%f\nq1_Lq_l:%f\n"
                   "q1_W:%f\nq1_W_h:%f\nq1_W_l:%f\n"
                   "q1_Wq:%f\nq1_Wq_h:%f\nq1_Wq_l:%f\n++++++\n",
                   q[i].th_servd.total, q[i].th_servd.high, q[i].th_servd.low,
                   q[i].pb.total, q[i].pb.high, q[i].pb.low,
                   q[i].L.total, q[i].L.high, q[i].L.low,
                   q[i].Lq.total, q[i].Lq.high, q[i].Lq.low,
                   q[i].W.total, q[i].W.high, q[i].W.low,
                   q[i].Wq.total, q[i].Wq.high, q[i].Wq.low);
        }

        f_high_low th_total, pb_total, L_total, Lq_total, W_total, Wq_total;

        pb_total.total = q[0].pb.total;
        pb_total.high = q[0].pb.high;
        pb_total.low = q[0].pb.low;

        for (int i = 0; i < link_size; i++)
        {
            th_total.total += q[i].th_servd.total;
            th_total.high += q[i].th_servd.high;
            th_total.low += q[i].th_servd.low;
            if (i > 0)
            {
                pb_total.total += (1 - q[i - 1].dept) * (1 - q[i - 1].pb.total) * q[i].pb.total;
                pb_total.high += (1 - q[i - 1].dept) * (1 - q[i - 1].pb.high) * q[i].pb.high;
                pb_total.low += (1 - q[i - 1].dept) * (1 - q[i - 1].pb.low) * q[i].pb.low;
            }
            L_total.total += q[i].L.total;
            L_total.high += q[i].L.high;
            L_total.low += q[i].L.low;

            Lq_total.total += q[i].Lq.total;
            Lq_total.high += q[i].Lq.high;
            Lq_total.low += q[i].Lq.low;

            W_total.total += q[i].W.total;
            W_total.high += q[i].W.high;
            W_total.low += q[i].W.low;

            Wq_total.total += q[i].Wq.total;
            Wq_total.high += q[i].Wq.high;
            Wq_total.low += q[i].Wq.low;
        }

        printf("th:%f\nth_h:%f\nth_l:%f\n"
               "pb:%f\npb_h:%f\npb_l:%f\n"
               "L:%f\nL_h:%f\nL_l:%f\n"
               "Lq:%f\nLq_h:%f\nLq_l:%f\n"
               "W:%f\nW_h:%f\nW_l:%f\n"
               "Wq:%f\nWq_h:%f\nWq_l:%f\n++++++\n",
               th_total.total, th_total.high, th_total.low,
               pb_total.total, pb_total.high, pb_total.low,
               L_total.total, L_total.high, L_total.low,
               Lq_total.total, Lq_total.high, Lq_total.low,
               W_total.total, W_total.high, W_total.low,
               Wq_total.total, Wq_total.high, Wq_total.low);

        ofstream ofs;
        ofs.open("C:\\Users\\T2-401\\Desktop\\iter_4-4_GEO2LEOLink.csv", ios::app); // T2-401//lksh4//ethan chen

        ofs << th_total.total << "," << th_total.high << "," << th_total.low << ","
            << pb_total.total << "," << pb_total.high << "," << pb_total.low << ","
            << L_total.total << "," << L_total.high << "," << L_total.low << ","
            << Lq_total.total << "," << Lq_total.high << "," << Lq_total.low << ","
            << W_total.total << "," << W_total.high << "," << W_total.low << ","
            << Wq_total.total << "," << Wq_total.high << "," << Wq_total.low << "\n";

        ofs.close();

        count++;
        begin += interval;
    }
    return 0;
}
