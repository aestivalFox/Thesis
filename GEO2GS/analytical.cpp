
#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

#define sys_size 50
double arr_p = 1.5;
double arr_0 = 3.5;
double arr_1 = 1;
double serv_p = 4;
double serv_s = 4;
double sw01 = 0.5;
#define sw10 0.5

struct d_high_low
{
    unsigned int total = 0;
    unsigned int high = 0;
    unsigned int low = 0;
};
struct f_high_low
{
    double total = 0;
    double p = 0;
    double s = 0;
};

unsigned int iter_cnt;
double old_pi[2][3][sys_size + 1][sys_size + 1] = {0};
double current_pi[2][3][sys_size + 1][sys_size + 1] = {0};
double deviation;
double L, Lq, W, Wq, block_prob;
double eff_arr_rate;
void initialization(), iteration(), print_metrics();

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
            printf("\nserv_rates_h[1] = %f\n", serv_p);
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

        initialization();

        while (deviation > 1E-8)
        {
            iter_cnt++;
            if (iter_cnt >= 10000)
            {
                cout << "diverge" << endl;
                system("pause");
                break;
            }

            iteration();
            deviation = 0;
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k <= sys_size; k++)
                    {
                        for (int l = 0; l <= sys_size; l++)
                        {
                            if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                            {
                                deviation += pow(old_pi[i][j][k][l] - current_pi[i][j][k][l], 2);
                                old_pi[i][j][k][l] = current_pi[i][j][k][l];
                            }
                        }
                    }
                }
            }
            deviation = sqrt(deviation);
            // cout << deviation << endl;
        }

        printf("end\niter_count : %d\n", iter_cnt);
        /*
        int count = 0;
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k <= sys_size; k++)
                {
                    for (int l = 0; l <= sys_size; l++)
                    {
                        if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                        {
                            count++;
                            printf("Pi[%d][%d][%d][%d] : %6f \n", i, j, k, l, current_pi[i][j][k][l]);
                        }
                    }
                }
            }
        }
        cout << count << endl;
        */
        print_metrics();

        count++;
        begin += interval;
    }
    return 0;
}

void initialization()
{
    iter_cnt = 0;
    deviation = 1;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k <= sys_size; k++)
            {
                for (int l = 0; l <= sys_size; l++)
                {
                    if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                    {
                        current_pi[i][j][k][l] = 1.0 / (2 * 3 * (sys_size + 1) * (sys_size + 1));
                        old_pi[i][j][k][l] = 0;
                    }
                }
            }
        }
    }
    L = 0;
    Lq = 0;
    W = 0;
    Wq = 0;
    block_prob = 0;
    eff_arr_rate = 0;
}

void iteration()
{
    double w8_1 = 0, w8_1_bar = 0, w8_p = 0, w8_mmoo_0, w8_mmoo_1;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k <= sys_size; k++)
            {
                for (int l = 0; l <= sys_size; l++) // current_pi[i][j][k][l]
                {
                    if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                    {
                        /**/
                        if (j == 1)
                        {
                            w8_1 = (double)(k - 1) / (double)(k - 1 + l);
                            w8_1_bar = (double)l / (double)(k - 1 + l);
                        }
                        if (j == 2)
                        {
                            w8_1 = (double)k / (double)(k + l - 1);
                            w8_1_bar = (double)(l - 1) / (double)(k + l - 1);
                        }

                        /*

                         w8_1=arr_p/(arr_p+((arr_0*sw10+arr_1*sw01)/(sw10+sw01)));
                         w8_mmoo_0=((arr_0*sw10)/(sw10+sw01))/(arr_p+((arr_0*sw10+arr_1*sw01)/(sw10+sw01)));
                         w8_mmoo_1=((arr_1*sw01)/(sw10+sw01))/(arr_p+((arr_0*sw10+arr_1*sw01)/(sw10+sw01)));
 */
                        if (i == 0)
                        {
                            if (j == 0 && k == 0 && l == 0)
                            {
                                current_pi[i][j][k][l] =
                                    (current_pi[i + 1][j][k][l] * sw10 +
                                     current_pi[i][j + 1][k + 1][l] * serv_p +
                                     current_pi[i][j + 2][k][l + 1] * serv_s) /
                                    (sw01 + arr_p + arr_0);
                            }
                            if (j == 1)
                            {
                                if (k == 1 && l == 0)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j - 1][k - 1][l] * arr_p +
                                         current_pi[i][j][k + 1][l] * serv_p +
                                         current_pi[i][j + 1][k][l + 1] * serv_s) /
                                        (sw01 + arr_p + arr_0 + serv_p);
                                }
                                else if (k == 1 && l > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k][l - 1] * arr_0 +
                                         current_pi[i][j][k + 1][l] * serv_p * (k / (double)(k + l)) +
                                         current_pi[i][j + 1][k][l + 1] * serv_s * (k / (double)(k + l))) /
                                        (sw01 + arr_p + arr_0 + serv_p);
                                }
                                else if (k == 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k][l - 1] * arr_0) /
                                        (sw01 + serv_p);
                                }
                                else if (k > 1 && l == 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k + 1][l] * serv_p +
                                         current_pi[i][j + 1][k][l + 1] * serv_s) /
                                        (sw01 + arr_p + arr_0 + serv_p);
                                }
                                else if (k > 1 && l > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k][l - 1] * arr_0 +
                                         current_pi[i][j][k + 1][l] * serv_p * (k / (double)(k + l)) +
                                         current_pi[i][j + 1][k][l + 1] * serv_s * (k / (double)(k + l))) /
                                        (sw01 + arr_p + arr_0 + serv_p * w8_1 + serv_p * w8_1_bar);
                                }
                                else if (k > 1 && l > 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k][l - 1] * arr_0) /
                                        (sw01 + serv_p * w8_1 + serv_p * w8_1_bar);
                                }
                                else if (k == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_p) /
                                        (sw01 + serv_p);
                                }
                            }
                            if (j == 2)
                            {
                                if (l == 1 && k == 0)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j - 2][k][l - 1] * arr_0 +
                                         current_pi[i][j - 1][k + 1][l] * serv_p +
                                         current_pi[i][j][k][l + 1] * serv_s) /
                                        (sw01 + arr_p + arr_0 + serv_s);
                                }
                                else if (l == 1 && k > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j - 1][k + 1][l] * serv_p * (l / (double)(k + l)) +
                                         current_pi[i][j][k][l + 1] * serv_s * (l / (double)(k + l))) /
                                        (sw01 + arr_p + arr_0 + serv_s);
                                }
                                else if (l == 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_p) /
                                        (sw01 + serv_s);
                                }
                                else if (l > 1 && k == 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k][l - 1] * arr_0 +
                                         current_pi[i][j - 1][k + 1][l] * serv_p +
                                         current_pi[i][j][k][l + 1] * serv_s) /
                                        (sw01 + arr_p + arr_0 + serv_s);
                                }
                                else if (l > 1 && k > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k][l - 1] * arr_0 +
                                         current_pi[i][j - 1][k + 1][l] * serv_p * (l / (double)(k + l)) +
                                         current_pi[i][j][k][l + 1] * serv_s * (l / (double)(k + l))) /
                                        (sw01 + arr_p + arr_0 + serv_s * w8_1_bar + serv_s * w8_1);
                                }
                                else if (l > 1 && k > 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k][l - 1] * arr_0) /
                                        (sw01 + serv_s * w8_1_bar + serv_s * w8_1);
                                }
                                else if (l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k][l - 1] * arr_0) /
                                        (sw01 + serv_s);
                                }
                            }
                        }
                        //---------------------------------------------------------------------------------------
                        if (i == 1)
                        {
                            if (j == 0 && k == 0 && l == 0)
                            {
                                current_pi[i][j][k][l] =
                                    (current_pi[i - 1][j][k][l] * sw01 +
                                     current_pi[i][j + 1][k + 1][l] * serv_p +
                                     current_pi[i][j + 2][k][l + 1] * serv_s) /
                                    (sw10 + arr_p + arr_1);
                            }
                            if (j == 1)
                            {
                                if (k == 1 && l == 0)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j - 1][k - 1][l] * arr_p +
                                         current_pi[i][j][k + 1][l] * serv_p +
                                         current_pi[i][j + 1][k][l + 1] * serv_s) /
                                        (sw10 + arr_p + arr_1 + serv_p);
                                }
                                else if (k == 1 && l > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k][l - 1] * arr_1 +
                                         current_pi[i][j][k + 1][l] * serv_p * (k / (double)(k + l)) +
                                         current_pi[i][j + 1][k][l + 1] * serv_s * (k / (double)(k + l))) /
                                        (sw10 + arr_p + arr_1 + serv_p);
                                }
                                else if (k == 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k][l - 1] * arr_1) /
                                        (sw10 + serv_p);
                                }
                                else if (k > 1 && l == 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k + 1][l] * serv_p +
                                         current_pi[i][j + 1][k][l + 1] * serv_s) /
                                        (sw10 + arr_p + arr_1 + serv_p);
                                }
                                else if (k > 1 && l > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k][l - 1] * arr_1 +
                                         current_pi[i][j][k + 1][l] * serv_p * (k / (double)(k + l)) +
                                         current_pi[i][j + 1][k][l + 1] * serv_s * (k / (double)(k + l))) /
                                        (sw10 + arr_p + arr_1 + serv_p * w8_1 + serv_p * w8_1_bar);
                                }
                                else if (k > 1 && l > 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k][l - 1] * arr_1) /
                                        (sw10 + serv_p * w8_1 + serv_p * w8_1_bar);
                                }
                                else if (k == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_p) /
                                        (sw10 + serv_p);
                                }
                            }
                            if (j == 2)
                            {
                                if (l == 1 && k == 0)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j - 2][k][l - 1] * arr_1 +
                                         current_pi[i][j - 1][k + 1][l] * serv_p +
                                         current_pi[i][j][k][l + 1] * serv_s) /
                                        (sw10 + arr_p + arr_1 + serv_s);
                                }
                                else if (l == 1 && k > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j - 1][k + 1][l] * serv_p * (l / (double)(k + l)) +
                                         current_pi[i][j][k][l + 1] * serv_s * (l / (double)(k + l))) /
                                        (sw10 + arr_p + arr_1 + serv_s);
                                }
                                else if (l == 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_p) /
                                        (sw10 + serv_s);
                                }
                                else if (l > 1 && k == 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k][l - 1] * arr_1 +
                                         current_pi[i][j - 1][k + 1][l] * serv_p +
                                         current_pi[i][j][k][l + 1] * serv_s) /
                                        (sw10 + arr_p + arr_1 + serv_s);
                                }
                                else if (l > 1 && k > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k][l - 1] * arr_1 +
                                         current_pi[i][j - 1][k + 1][l] * serv_p * (l / (double)(k + l)) +
                                         current_pi[i][j][k][l + 1] * serv_s * (l / (double)(k + l))) /
                                        (sw10 + arr_p + arr_1 + serv_s * w8_1_bar + serv_s * w8_1);
                                }
                                else if (l > 1 && k > 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_p +
                                         current_pi[i][j][k][l - 1] * arr_1) /
                                        (sw10 + serv_s * w8_1_bar + serv_s * w8_1);
                                }
                                else if (l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k][l - 1] * arr_1) /
                                        (sw10 + serv_s);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /**/
    double temp = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k <= sys_size; k++)
            {
                for (int l = 0; l <= sys_size; l++)
                {
                    if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                    {
                        temp += current_pi[i][j][k][l];
                    }
                }
            }
        }
    }
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k <= sys_size; k++)
            {
                for (int l = 0; l <= sys_size; l++)
                {
                    if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                    {
                        current_pi[i][j][k][l] /= temp;
                    }
                }
            }
        }
    }

    // printf("sum : %6f \n", temp);
}

void print_metrics()
{
    f_high_low th, pb, L, Lq, W, Wq;
    double pi_0 = 0, pi_1 = 0, pk_0 = 0, pk_1 = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k <= sys_size; k++)
            {
                for (int l = 0; l <= sys_size; l++)
                {
                    if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                    {
                        if (j == 1)
                            th.p += serv_p * current_pi[i][j][k][l];
                        if (j == 2)
                            th.s += serv_s * current_pi[i][j][k][l];

                        if (i == 0)
                        {
                            pi_0 += current_pi[i][j][k][l];
                            if (k + l == sys_size)
                                pk_0 += current_pi[i][j][k][l];
                        }
                        if (i == 1)
                        {
                            pi_1 += current_pi[i][j][k][l];
                            if (k + l == sys_size)
                                pk_1 += current_pi[i][j][k][l];
                        }
                        // L,Lq
                        L.p += k * current_pi[i][j][k][l];
                        L.s += l * current_pi[i][j][k][l];
                        if (j == 1)
                        {
                            Lq.p += (k - 1) * current_pi[i][j][k][l];
                            Lq.s += l * current_pi[i][j][k][l];
                        }
                        if (j == 2)
                        {
                            Lq.p += k * current_pi[i][j][k][l];
                            Lq.s += (l - 1) * current_pi[i][j][k][l];
                        }
                    }
                }
            }
        }
    }
    th.total = th.p + th.s;
    pb.total = ((arr_p + arr_0) * pk_0 + (arr_p + arr_1) * pk_1) / ((arr_p + arr_0) * pi_0 + (arr_p + arr_1) * pi_1);
    pb.p = (pk_0 + pk_1) / (pi_0 + pi_1);
    pb.s = (arr_0 * pk_0 + arr_1 * pk_1) / (arr_0 * pi_0 + arr_1 * pi_1);
    L.total = L.p + L.s;
    Lq.total = Lq.p + Lq.s;
    W.total = L.total / ((arr_p + arr_0) * (pi_0 - pk_0) + (arr_p + arr_1) * (pi_1 - pk_1));
    W.p = L.p / (arr_p * (1 - pb.p));
    W.s = L.s / (arr_0 * (pi_0 - pk_0) + arr_1 * (pi_1 - pk_1));
    Wq.total = Lq.total / ((arr_p + arr_0) * (pi_0 - pk_0) + (arr_p + arr_1) * (pi_1 - pk_1));
    Wq.p = Lq.p / (arr_p * (1 - pb.p));
    Wq.s = Lq.s / (arr_0 * (pi_0 - pk_0) + arr_1 * (pi_1 - pk_1));
    /*
    Wq.total=Lq.total/th.total;
    Wq.p=Lq.p/th.p;
    Wq.s=Lq.s/th.s;
    */
    printf("th : %6f \n", th.total);
    printf("th.p : %6f \n", th.p);
    printf("th.s : %6f \n", th.s);
    printf("pb.total : %6f \n", pb.total);
    printf("pb.p : %6f \n", pb.total);
    printf("pb.s : %6f \n", pb.total);
    printf("L.p : %6f \n", L.p);
    printf("L.s : %6f \n", L.s);
    printf("Lq.total : %6f \n", Lq.total);
    printf("Lq.p : %6f \n", Lq.p);
    printf("Lq.s : %6f \n", Lq.s);
    printf("W.total : %6f \n", W.total);
    printf("W.p : %6f \n", W.p);
    printf("W.s : %6f \n", W.s);
    printf("Wq.total : %6f \n", Wq.total);
    printf("Wq.p : %6f \n", Wq.p);
    printf("Wq.s : %6f \n", Wq.s);
    printf("tmp : %6f \n", (arr_p + arr_0) * (pi_0 - pk_0) + (arr_p + arr_1) * (pi_1 - pk_1));

    ofstream ofs;
    ofs.open("C:\\Users\\T2-401\\Desktop\\iter_GEO2GS.csv", ios::app); // T2-401//lksh4//ethan chen
    ofs << th.total << "," << th.p << "," << th.s << ","
        << pb.total << "," << pb.p << "," << pb.s << ","
        << L.total << "," << L.p << "," << L.s << ","
        << Lq.total << "," << Lq.p << "," << Lq.s << ","
        << W.total << "," << W.p << "," << W.s << ","
        << Wq.total << "," << Wq.p << "," << Wq.s << "\n";
    ofs.close();
}
