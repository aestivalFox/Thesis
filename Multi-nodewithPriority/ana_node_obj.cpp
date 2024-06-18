#define sys_size_limit 50

class first_q
{
public:
    struct f_high_low
    {
        double total = 0;
        double high = 0;
        double low = 0;
    };

    int sys_size;
    double arr_h;
    double arr_l0;
    double arr_l1;
    double serv_h;
    double serv_l;
    double sw01;
    double sw10;
    double dept;
    double current_pi[2][3][sys_size_limit + 1][sys_size_limit + 1] = {0};
    double old_pi[2][3][sys_size_limit + 1][sys_size_limit + 1] = {0};
    int iter_cnt;
    double deviation;
    void initialization()
    {
        iter_cnt = 0;
        deviation = 1;
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                for (int k = 0; k <= sys_size; k++)
                    for (int l = 0; l <= sys_size; l++)
                        if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                        {
                            current_pi[i][j][k][l] = 1.0 / (2 * 3 * (sys_size + 1) * (sys_size + 1));
                            old_pi[i][j][k][l] = 0;
                        }
    }

    void equation()
    {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k <= sys_size; k++)
                {
                    for (int l = 0; l <= sys_size; l++) // current_pi[i][j][k][l]
                    {
                        if (i == 0)
                        {
                            if (j == 0 && k == 0 && l == 0)
                            {
                                current_pi[i][j][k][l] =
                                    (current_pi[i + 1][j][k][l] * sw10 +
                                     current_pi[i][j + 1][k + 1][l] * serv_h +
                                     current_pi[i][j + 2][k][l + 1] * serv_l) /
                                    (sw01 + arr_h + arr_l0);
                            }
                            if (j == 1)
                            {
                                if (k == 1 && l == 0)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j - 1][k - 1][l] * arr_h +
                                         current_pi[i][j][k + 1][l] * serv_h +
                                         current_pi[i][j + 1][k][l + 1] * serv_l) /
                                        (sw01 + serv_h + arr_h + arr_l0);
                                }
                                else if (k == 1 && l > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k][l - 1] * arr_l0 +
                                         current_pi[i][j][k + 1][l] * serv_h +
                                         current_pi[i][j + 1][k][l + 1] * serv_l) /
                                        (sw01 + arr_h + arr_l0 + serv_h);
                                }
                                else if (k == 1 && l > 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k][l - 1] * arr_l0) /
                                        (sw01 + serv_h);
                                }
                                else if (k > 1 && l == 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k + 1][l] * serv_h +
                                         current_pi[i][j + 1][k][l + 1] * serv_l) /
                                        (sw01 + arr_h + serv_h + arr_l0);
                                }
                                else if (k > 1 && l == 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_h) /
                                        (sw01 + arr_h);
                                }
                                else if (k > 1 && l > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k][l - 1] * arr_l0 +
                                         current_pi[i][j][k + 1][l] * serv_h +
                                         current_pi[i][j + 1][k][l + 1] * serv_l) /
                                        (sw01 + serv_h + arr_h + arr_l0);
                                }
                                else if (k > 1 && l > 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k][l - 1] * arr_l0) /
                                        (sw01 + serv_h);
                                }
                            }
                            if (j == 2)
                            {
                                if (k == 0 && l == 1)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j - 2][k][l - 1] * arr_l0 +
                                         current_pi[i][j][k][l + 1] * serv_l +
                                         current_pi[i][j - 1][k + 1][l] * serv_h) /
                                        (sw01 + serv_l + arr_h + arr_l0);
                                }
                                else if (k >= 1 && l == 1 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_h) /
                                        (sw01 + arr_h + arr_l0 + serv_l);
                                }
                                else if (k >= 1 && l == 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_h) /
                                        (sw01 + serv_l);
                                }
                                else if (k == 0 && l > 1 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k][l - 1] * arr_l0 +
                                         current_pi[i][j][k][l + 1] * serv_l +
                                         current_pi[i][j - 1][k + 1][l] * serv_h) /
                                        (sw01 + serv_l + arr_h + arr_l0);
                                }
                                else if (k == 0 && l > 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k][l - 1] * arr_l0) /
                                        (sw01 + serv_l);
                                }
                                else if (k >= 1 && l > 1 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k][l - 1] * arr_l0) /
                                        (sw01 + serv_l + arr_h + arr_l0);
                                }
                                else if (k >= 1 && l > 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i + 1][j][k][l] * sw10 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k][l - 1] * arr_l0) /
                                        (sw01 + serv_l);
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
                                     current_pi[i][j + 1][k + 1][l] * serv_h +
                                     current_pi[i][j + 2][k][l + 1] * serv_l) /
                                    (sw10 + arr_h + arr_l1);
                            }
                            if (j == 1)
                            {
                                if (k == 1 && l == 0)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j - 1][k - 1][l] * arr_h +
                                         current_pi[i][j][k + 1][l] * serv_h +
                                         current_pi[i][j + 1][k][l + 1] * serv_l) /
                                        (sw10 + serv_h + arr_h + arr_l1);
                                }
                                else if (k == 1 && l > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k][l - 1] * arr_l1 +
                                         current_pi[i][j][k + 1][l] * serv_h +
                                         current_pi[i][j + 1][k][l + 1] * serv_l) /
                                        (sw10 + arr_h + arr_l1 + serv_h);
                                }
                                else if (k == 1 && l > 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k][l - 1] * arr_l1) /
                                        (sw10 + serv_h);
                                }
                                else if (k > 1 && l == 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k + 1][l] * serv_h +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j + 1][k][l + 1] * serv_l) /
                                        (sw10 + arr_h + serv_h + arr_l1);
                                }
                                else if (k > 1 && l == 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_h) /
                                        (sw10 + arr_h);
                                }
                                else if (k > 1 && l > 0 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k][l - 1] * arr_l1 +
                                         current_pi[i][j][k + 1][l] * serv_h +
                                         current_pi[i][j + 1][k][l + 1] * serv_l) /
                                        (sw10 + serv_h + arr_h + arr_l1);
                                }
                                else if (k > 1 && l > 0 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k][l - 1] * arr_l1) /
                                        (sw10 + serv_h);
                                }
                            }
                            if (j == 2)
                            {
                                if (k == 0 && l == 1)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j - 2][k][l - 1] * arr_l1 +
                                         current_pi[i][j][k][l + 1] * serv_l +
                                         current_pi[i][j - 1][k + 1][l] * serv_h) /
                                        (sw10 + serv_l + arr_h + arr_l1);
                                }
                                else if (k >= 1 && l == 1 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_h) /
                                        (sw10 + arr_h + arr_l1 + serv_l);
                                }
                                else if (k >= 1 && l == 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_h) /
                                        (sw10 + serv_l);
                                }
                                else if (k == 0 && l > 1 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k][l - 1] * arr_l1 +
                                         current_pi[i][j][k][l + 1] * serv_l +
                                         current_pi[i][j - 1][k + 1][l] * serv_h) /
                                        (sw10 + serv_l + arr_h + arr_l1);
                                }
                                else if (k == 0 && l > 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k][l - 1] * arr_l1) /
                                        (sw10 + serv_l);
                                }
                                else if (k >= 1 && l > 1 && k + l < sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k][l - 1] * arr_l1) /
                                        (sw10 + serv_l + arr_h + arr_l1);
                                }
                                else if (k >= 1 && l > 1 && k + l == sys_size)
                                {
                                    current_pi[i][j][k][l] =
                                        (current_pi[i - 1][j][k][l] * sw01 +
                                         current_pi[i][j][k - 1][l] * arr_h +
                                         current_pi[i][j][k][l - 1] * arr_l1) /
                                        (sw10 + serv_l);
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
    }

    void iteration()
    {
        initialization();
        while (deviation > 1E-9)
        {
            iter_cnt++;
            if (iter_cnt >= 10000)
            {
                system("pause");
                break;
            }
            equation();
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
        }
    }
    f_high_low th, th_relay, th_servd, pb, L, Lq, W, Wq;
    double th_l0 = 0, th_l1 = 0, th_l0_relay, th_l1_relay, th_servd_l0 = 0, th_servd_l1 = 0;
    void calc_metrics()
    {
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
                            // printf("pi[%d][%d]{%d}{%d} = %f\n",i,j,k,l,current_pi[i][j][k][l]);
                        }
                    }
                }
            }
        }
        double pi_0 = 0, pi_1 = 0, pk_0 = 0, pk_1 = 0;
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                for (int k = 0; k <= sys_size; k++)
                    for (int l = 0; l <= sys_size; l++)
                    {
                        if ((j == 0 && k == 0 && l == 0) || (j == 1 && k > 0 && k + l <= sys_size) || (j == 2 && l > 0 && k + l <= sys_size))
                        {
                            // th
                            if (j == 1 && k > 0)
                                th.high += serv_h * current_pi[i][j][k][l];
                            if (i == 0 && j == 2)
                                th_l0 += serv_l * current_pi[i][j][k][l];
                            if (i == 1 && j == 2)
                                th_l1 += serv_l * current_pi[i][j][k][l];

                            // pb
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
                            L.high += k * current_pi[i][j][k][l];
                            L.low += l * current_pi[i][j][k][l];
                            if (j == 1)
                            {
                                Lq.high += (k - 1) * current_pi[i][j][k][l];
                                Lq.low += l * current_pi[i][j][k][l];
                            }
                            if (j == 2)
                            {
                                Lq.high += k * current_pi[i][j][k][l];
                                Lq.low += (l - 1) * current_pi[i][j][k][l];
                            }
                        }
                    }

        th_relay.high = th.high * (1 - dept);
        th_l0_relay = th_l0 * (1 - dept);
        th_l1_relay = th_l1 * (1 - dept);
        th_relay.total = th_relay.high + th_l0_relay + th_l1_relay;
        th_servd.high = th.high * dept;
        th_servd_l0 = th_l0 * dept;
        th_servd_l1 = th_l1 * dept;
        th_servd.low = th_servd_l0 + th_servd_l1;
        th_servd.total = th_servd.high + th_servd.low;
        pb.total = ((arr_h + arr_l0) * pk_0 + (arr_h + arr_l1) * pk_1) / ((arr_h + arr_l0) * pi_0 + (arr_h + arr_l1) * pi_1);
        pb.high = (pk_0 + pk_1) /*/(pi_0+pi_1)*/;
        pb.low = (arr_l0 * pk_0 + arr_l1 * pk_1) / (arr_l0 * pi_0 + arr_l1 * pi_1);
        L.total = L.high + L.low;
        Lq.total = Lq.high + Lq.low;
        W.total = L.total / ((arr_h + arr_l0) * (pi_0 - pk_0) + (arr_h + arr_l1) * (pi_1 - pk_1));
        W.high = L.high / (arr_h * (1 - pb.high));
        W.low = L.low / (arr_l0 * (pi_0 - pk_0) + arr_l1 * (pi_1 - pk_1));
        Wq.total = Lq.total / ((arr_h + arr_l0) * (pi_0 - pk_0) + (arr_h + arr_l1) * (pi_1 - pk_1));
        Wq.high = Lq.high / (arr_h * (1 - pb.high));
        Wq.low = Lq.low / (arr_l0 * (pi_0 - pk_0) + arr_l1 * (pi_1 - pk_1));
    }
};
