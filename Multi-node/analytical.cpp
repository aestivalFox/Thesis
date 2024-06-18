#include <iostream>
#include <math.h>
#include "first_q_5Q.cpp"
#include <fstream>
using namespace std;

#define link_size 5
#define sys_size_limit 50
int sys_size[link_size] = {50, 50, 50, 50, 50};
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

unsigned int iter_cnt;
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

        first_q q[5];
        q[0].sys_size = sys_size[0];
        q[0].arr_p = arr_1_h;
        q[0].arr_0 = arr_1_l0;
        q[0].arr_1 = arr_1_l1;
        q[0].serv_p = serv_rates_h[0];
        q[0].serv_s = serv_rates_h[0];
        q[0].sw01 = sw01_1;
        q[0].sw10 = sw10_1;
        q[0].dept = dept[0];
        q[0].iteration();

        printf("end\niter_count:%d\n", q[0].iter_cnt);
        q[0].calc_metrics();
        printf("q[0]_th:%f\nq[0]_th_h:%f\nq[0]_th_l:%f\n\n"
               "q[0]_pb:%f\nq[0]_pb_h:%f\nq[0]_pb_l:%f\n\n"
               "q[0]_L:%f\nq[0]_L_h:%f\nq[0]_L_l:%f\n\n"
               "q[0]_Lq:%f\nq[0]_Lq_h:%f\nq[0]_Lq_l:%f\n\n"
               "q[0]_W:%f\nq[0]_W_h:%f\nq[0]_W_l:%f\n\n"
               "q[0]_Wq:%f\nq[0]_Wq_h:%f\nq[0]_Wq_l:%f\n++++++\n",
               q[0].th_servd.total, q[0].th_servd.high, q[0].th_servd.low,
               q[0].pb.total, q[0].pb.high, q[0].pb.low,
               q[0].L.total, q[0].L.high, q[0].L.low,
               q[0].Lq.total, q[0].Lq.high, q[0].Lq.low,
               q[0].W.total, q[0].W.high, q[0].W.low,
               q[0].Wq.total, q[0].Wq.high, q[0].Wq.low);
        for (int i = 1; i < link_size; i++)
        {
            q[i].sys_size = sys_size[i];
            q[i].arr_p = q[i - 1].th_relay.high;
            q[i].arr_0 = q[i - 1].th_l0_relay / (q[i - 1].sw10 / (q[i - 1].sw01 + q[i - 1].sw10));
            q[i].arr_1 = q[i - 1].th_l1_relay / (q[i - 1].sw01 / (q[i - 1].sw01 + q[i - 1].sw10));
            q[i].serv_p = serv_rates_h[i];
            q[i].serv_s = serv_rates_h[i];
            q[i].sw01 = sw01_1;
            q[i].sw10 = sw10_1;
            q[i].dept = dept[i];

            q[i].iteration();
            printf("end\niter_count:%d\n", q[i].iter_cnt);
            q[i].calc_metrics();

            printf("q[%d]_th:%f\nq[%d]_th_h:%f\nq[%d]_th_l:%f\n\n"
                   "q[%d]_pb:%f\nq[%d]_pb_h:%f\nq[%d]_pb_l:%f\n\n"
                   "q[%d]_L:%f\nq[%d]_L_h:%f\nq[%d]_L_l:%f\n\n"
                   "q[%d]_Lq:%f\nq[%d]_Lq_h:%f\nq[%d]_Lq_l:%f\n\n"
                   "q[%d]_W:%f\nq[%d]_W_h:%f\nq[%d]_W_l:%f\n\n"
                   "q[%d]_Wq:%f\nq[%d]_Wq_h:%f\nq[%d]_Wq_l:%f\n++++++\n",
                   i, q[i].th_servd.total, i, q[i].th_servd.high, i, q[i].th_servd.low,
                   i, q[i].pb.total, i, q[i].pb.high, i, q[i].pb.low,
                   i, q[i].L.total, i, q[i].L.high, i, q[i].L.low,
                   i, q[i].Lq.total, i, q[i].Lq.high, i, q[i].Lq.low,
                   i, q[i].W.total, i, q[i].W.high, i, q[i].W.low,
                   i, q[i].Wq.total, i, q[i].Wq.high, i, q[i].Wq.low);
        }

        f_high_low th, pb, L, Lq, W, Wq;

        pb.total = q[0].pb.total;
        pb.high = q[0].pb.high;
        pb.low = q[0].pb.low;

        for (int i = 0; i < link_size; i++)
        {
            th.total += q[i].th_servd.total;
            th.high += q[i].th_servd.high;
            th.low += q[i].th_servd.low;

            if (i > 0)
            {
                pb.total += (1 - q[i - 1].dept) * (1 - q[i - 1].pb.total) * q[i].pb.total;
                pb.high += (1 - q[i - 1].dept) * (1 - q[i - 1].pb.high) * q[i].pb.high;
                pb.low += (1 - q[i - 1].dept) * (1 - q[i - 1].pb.low) * q[i].pb.low;
            }

            L.total += q[i].L.total;
            L.high += q[i].L.high;
            L.low += q[i].L.low;

            Lq.total += q[i].Lq.total;
            Lq.high += q[i].Lq.high;
            Lq.low += q[i].Lq.low;

            W.total += q[i].W.total;
            W.high += q[i].W.high;
            W.low += q[i].W.low;

            Wq.total += q[i].Wq.total;
            Wq.high += q[i].Wq.high;
            Wq.low += q[i].Wq.low;
        }

        printf("th:%f\nth_h:%f\nth_l:%f\n\n"
               "pb:%f\npb_h:%f\npb_l:%f\n\n"
               "L:%f\nL_h:%f\nL_l:%f\n\n"
               "Lq:%f\nLq_h:%f\nLq_l:%f\n\n"
               "W:%f\nW_h:%f\nW_l:%f\n\n"
               "Wq:%f\nWq_h:%f\nWq_l:%f\n",
               th.total, th.high, th.low,
               pb.total, pb.high, pb.low,
               L.total, L.high, L.low,
               Lq.total, Lq.high, Lq.low,
               W.total, W.high, W.low,
               Wq.total, Wq.high, Wq.low);
        ofstream ofs;

        ofs.open("C:\\Users\\T2-401\\Desktop\\iter_4-4_GEO2LEOLink.csv", ios::app); // T2-401//lksh4//ethan chen
        ofs << th.total << "," << th.high << "," << th.low << "," << pb.total << "," << pb.high << "," << pb.low << "," << L.total << "," << L.high << "," << L.low << "," << Lq.total << "," << Lq.high << "," << Lq.low << "," << W.total << "," << W.high << "," << W.low << "," << Wq.total << "," << Wq.high << "," << Wq.low << "\n";
        ofs.close();

        count++;
        begin += interval;
    }
    system("pause");
    return 0;
}
