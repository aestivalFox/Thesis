#include <iostream>
#include <math.h>
#include "first_q.cpp"
#include <fstream>
using namespace std;

#define sys_size_limit 50
#define sys_1_size 50
double arr_1_h = 1.5;
double arr_1_l0 = 3.5;
double arr_1_l1 = 1;
double serv_rates_h = 4;
double serv_rates_l = 4;
double sw01_1 = 0.5;
#define sw10_1 0.5
#define dept_1 1

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
            serv_rates_h = begin;
            serv_rates_l = begin;
            printf("\nserv_rates_h[0] = %f\n", serv_rates_h);
            break;
        case 4:
            serv_rates_h = begin;
            serv_rates_l = begin;
            printf("\nserv_rates_h[1] = %f\n", serv_rates_l);
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

        first_q q_G;
        q_G.sys_size = sys_1_size;
        q_G.arr_h = arr_1_h;
        q_G.arr_l0 = arr_1_l0;
        q_G.arr_l1 = arr_1_l1;
        q_G.serv_h = serv_rates_h;
        q_G.serv_l = serv_rates_l;
        q_G.sw01 = sw01_1;
        q_G.sw10 = sw10_1;
        q_G.dept = dept_1;
        q_G.iteration();

        printf("end\niter_count:%d\n", q_G.iter_cnt);
        q_G.calc_metrics();
        printf("q0_th:%f\nq0_th_h:%f\nq0_th_l:%f\n"
               "q0_pb:%f\nq0_pb_h:%f\nq0_pb_l:%f\n"
               "q0_L:%f\nq0_L_h:%f\nq0_L_l:%f\n"
               "q0_Lq:%f\nq0_Lq_h:%f\nq0_Lq_l:%f\n"
               "q0_W:%f\nq0_W_h:%f\nq0_W_l:%f\n"
               "q0_Wq:%f\nq0_Wq_h:%f\nq0_Wq_l:%f\n",
               q_G.th_servd.total, q_G.th_servd.high, q_G.th_servd.low,
               q_G.pb.total, q_G.pb.high, q_G.pb.low,
               q_G.L.total, q_G.L.high, q_G.L.low,
               q_G.Lq.total, q_G.Lq.high, q_G.Lq.low,
               q_G.W.total, q_G.W.high, q_G.W.low,
               q_G.Wq.total, q_G.Wq.high, q_G.Wq.low);

        /**/
        ofstream ofs;
        ofs.open("C:\\Users\\T2-401\\Desktop\\iter_4-4_GEO2GSwithP.csv", ios::app); // T2-401//lksh4//ethan chen

        ofs << q_G.th_servd.total << "," << q_G.th_servd.high << "," << q_G.th_servd.low << ","
            << q_G.pb.total << "," << q_G.pb.high << "," << q_G.pb.low << ","
            << q_G.L.total << "," << q_G.L.high << "," << q_G.L.low << ","
            << q_G.Lq.total << "," << q_G.Lq.high << "," << q_G.Lq.low << ","
            << q_G.W.total << "," << q_G.W.high << "," << q_G.W.low << ","
            << q_G.Wq.total << "," << q_G.Wq.high << "," << q_G.Wq.low << "\n";
        /*
            ofs << q_G.th_servd.total << "\n" << q_G.th_servd.high << "\n" << q_G.th_servd.low << "\n"
                << q_G.pb.total << "\n" << q_G.pb.high  << "\n" << q_G.pb.low << "\n"
                << q_G.L.total  << "\n" << q_G.L.high   << "\n" << q_G.L.low << "\n"
                << q_G.Lq.total << "\n" << q_G.Lq.high  << "\n" << q_G.Lq.low << "\n"
                << q_G.W.total  << "\n" << q_G.W.high   << "\n" << q_G.W.low << "\n"
                << q_G.Wq.total << "\n" << q_G.Wq.high  << "\n" << q_G.Wq.low << "\n"
                << q_L1.th.total    << "\n" << q_L1.th.high << "\n" << q_L1.th.low << "\n"
                << q_L1.pb.total    << "\n" << q_L1.pb.high << "\n" << q_L1.pb.low << "\n"
                << q_L1.L.total     << "\n" << q_L1.L.high  << "\n" << q_L1.L.low << "\n"
                << q_L1.Lq.total    << "\n" << q_L1.Lq.high << "\n" << q_L1.Lq.low << "\n"
                << q_L1.W.total     << "\n" << q_L1.W.high  << "\n" << q_L1.W.low << "\n"
                << q_L1.Wq.total    << "\n" << q_L1.Wq.high << "\n" << q_L1.Wq.low << "\n\n\n\n";
        */
        ofs.close();
        count++;
        begin += interval;
    }
    return 0;
}
