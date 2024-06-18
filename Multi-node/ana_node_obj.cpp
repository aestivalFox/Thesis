#define sys_size_limit 50

class first_q
{
    public:
    struct f_high_low
    {
        double total=0;
        double high=0;
        double low=0;
    };
    
    int sys_size;
    double arr_p;
    double arr_0;
    double arr_1;
    double serv_p;
    double serv_s;
    double sw01;
    double sw10;
    double dept;
    double current_pi[2][3][sys_size_limit+1][sys_size_limit+1]={0};
    double old_pi[2][3][sys_size_limit+1][sys_size_limit+1] = {0};
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
                        if((j==0&&k==0&&l==0)||(j==1&&k>0&&k+l<=sys_size)||(j==2&&l>0&&k+l<=sys_size))
                        {
                            current_pi[i][j][k][l] = 1.0 / (2*3*(sys_size + 1)*(sys_size + 1));
                            old_pi[i][j][k][l] = 0;
                        }    
    }
    
    void equation()
    {
        double w8_1=0,w8_1_bar=0;
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k <= sys_size; k++)
                {
                    for (int l = 0; l <= sys_size; l++)//current_pi[i][j][k][l]
                    {
                        if((j==0&&k==0&&l==0)||(j==1&&k>0&&k+l<=sys_size)||(j==2&&l>0&&k+l<=sys_size))
                        {
                            /**/
                            if(j==1)
                            {
                                w8_1=(double)(k-1)/(double)(k-1+l);
                                w8_1_bar=(double)l/(double)(k-1+l);
                            }
                            if(j==2)
                            {
                                w8_1=(double)k/(double)(k+l-1);
                                w8_1_bar=(double)(l-1)/(double)(k+l-1);
                            }  
                            
                        /*
                        
                            w8_1=arr_p/(arr_p+((arr_0*sw10+arr_1*sw01)/(sw10+sw01)));
                            w8_mmoo_0=((arr_0*sw10)/(sw10+sw01))/(arr_p+((arr_0*sw10+arr_1*sw01)/(sw10+sw01)));
                            w8_mmoo_1=((arr_1*sw01)/(sw10+sw01))/(arr_p+((arr_0*sw10+arr_1*sw01)/(sw10+sw01)));
    */
                            if(i==0){
                                if(j==0&&k==0&&l==0)
                                {
                                    current_pi[i][j][k][l]=
                                        (current_pi[i+1][j][k][l]*sw10+
                                        current_pi[i][j+1][k+1][l]*serv_p+
                                        current_pi[i][j+2][k][l+1]*serv_s
                                        )/(sw01 + arr_p + arr_0);
                                }
                                if(j==1)
                                {
                                    if(k==1&&l==0)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j-1][k-1][l]*arr_p+
                                            current_pi[i][j][k+1][l]*serv_p+
                                            current_pi[i][j+1][k][l+1]*serv_s
                                            )/(sw01 + arr_p + arr_0 + serv_p);
                                    }
                                    else if(k==1&&l>0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k][l-1]*arr_0+
                                            current_pi[i][j][k+1][l]*serv_p*(k/(double)(k+l))+
                                            current_pi[i][j+1][k][l+1]*serv_s*(k/(double)(k+l))
                                            )/(sw01 + arr_p + arr_0 + serv_p);
                                    }
                                    else if(k==1&&k+l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k][l-1]*arr_0
                                            )/(sw01 + serv_p);
                                    }
                                    else if(k>1&&l==0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k+1][l]*serv_p+
                                            current_pi[i][j+1][k][l+1]*serv_s
                                            )/(sw01 + arr_p + arr_0 + serv_p);
                                    }
                                    else if(k>1&&l>0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k][l-1]*arr_0+
                                            current_pi[i][j][k+1][l]*serv_p*(k/(double)(k+l))+
                                            current_pi[i][j+1][k][l+1]*serv_s*(k/(double)(k+l))
                                            )/(sw01 + arr_p + arr_0 + serv_p*w8_1 + serv_p*w8_1_bar);
                                    }
                                    else if(k>1&&l>0&&k+l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k][l-1]*arr_0
                                            )/(sw01 + serv_p*w8_1 + serv_p*w8_1_bar);
                                    }
                                    else if(k==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k-1][l]*arr_p
                                            )/(sw01 + serv_p);
                                    }
                                }
                                if(j==2)
                                {
                                    if(l==1&&k==0)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j-2][k][l-1]*arr_0+
                                            current_pi[i][j-1][k+1][l]*serv_p+
                                            current_pi[i][j][k][l+1]*serv_s
                                            )/(sw01 + arr_p + arr_0 + serv_s);
                                    }
                                    else if(l==1&&k>0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j-1][k+1][l]*serv_p*(l/(double)(k+l))+
                                            current_pi[i][j][k][l+1]*serv_s*(l/(double)(k+l))
                                            )/(sw01 + arr_p + arr_0 + serv_s);
                                    }
                                    else if(l==1&&k+l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k-1][l]*arr_p
                                            )/(sw01 + serv_s);
                                    }
                                    else if(l>1&&k==0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k][l-1]*arr_0+
                                            current_pi[i][j-1][k+1][l]*serv_p+
                                            current_pi[i][j][k][l+1]*serv_s
                                            )/(sw01 + arr_p + arr_0 + serv_s);
                                    }
                                    else if(l>1&&k>0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k][l-1]*arr_0+
                                            current_pi[i][j-1][k+1][l]*serv_p*(l/(double)(k+l))+
                                            current_pi[i][j][k][l+1]*serv_s*(l/(double)(k+l))
                                            )/(sw01 + arr_p + arr_0 + serv_s*w8_1_bar + serv_s*w8_1);
                                    }
                                    else if(l>1&&k>0&&k+l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k][l-1]*arr_0
                                            )/(sw01 + serv_s*w8_1_bar + serv_s*w8_1);
                                    }
                                    else if(l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i+1][j][k][l]*sw10+
                                            current_pi[i][j][k][l-1]*arr_0
                                            )/(sw01 + serv_s);
                                    }
                                    
                                    
                                }
                            }
                            //---------------------------------------------------------------------------------------
                            if(i==1)
                            {
                                if(j==0&&k==0&&l==0)
                                {
                                    current_pi[i][j][k][l]=
                                        (current_pi[i-1][j][k][l]*sw01+
                                        current_pi[i][j+1][k+1][l]*serv_p+
                                        current_pi[i][j+2][k][l+1]*serv_s
                                        )/(sw10 + arr_p + arr_1);
                                }
                                if(j==1)
                                {
                                    if(k==1&&l==0)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j-1][k-1][l]*arr_p+
                                            current_pi[i][j][k+1][l]*serv_p+
                                            current_pi[i][j+1][k][l+1]*serv_s
                                            )/(sw10 + arr_p + arr_1 + serv_p);
                                    }
                                    else if(k==1&&l>0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k][l-1]*arr_1+
                                            current_pi[i][j][k+1][l]*serv_p*(k/(double)(k+l))+
                                            current_pi[i][j+1][k][l+1]*serv_s*(k/(double)(k+l))
                                            )/(sw10 + arr_p + arr_1 + serv_p);
                                    }
                                    else if(k==1&&k+l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k][l-1]*arr_1
                                            )/(sw10 + serv_p);
                                    }
                                    else if(k>1&&l==0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k+1][l]*serv_p+
                                            current_pi[i][j+1][k][l+1]*serv_s
                                            )/(sw10 + arr_p + arr_1 + serv_p);
                                    }
                                    else if(k>1&&l>0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k][l-1]*arr_1+
                                            current_pi[i][j][k+1][l]*serv_p*(k/(double)(k+l))+
                                            current_pi[i][j+1][k][l+1]*serv_s*(k/(double)(k+l))
                                            )/(sw10 + arr_p + arr_1 + serv_p*w8_1 + serv_p*w8_1_bar);
                                    }
                                    else if(k>1&&l>0&&k+l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k][l-1]*arr_1
                                            )/(sw10 + serv_p*w8_1 + serv_p*w8_1_bar);
                                    }
                                    else if(k==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k-1][l]*arr_p
                                            )/(sw10 + serv_p);
                                    }
                                }
                                if(j==2)
                                {
                                    if(l==1&&k==0)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j-2][k][l-1]*arr_1+
                                            current_pi[i][j-1][k+1][l]*serv_p+
                                            current_pi[i][j][k][l+1]*serv_s
                                            )/(sw10 + arr_p + arr_1 + serv_s);
                                    }
                                    else if(l==1&&k>0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j-1][k+1][l]*serv_p*(l/(double)(k+l))+
                                            current_pi[i][j][k][l+1]*serv_s*(l/(double)(k+l))
                                            )/(sw10 + arr_p + arr_1 + serv_s);
                                    }
                                    else if(l==1&&k+l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k-1][l]*arr_p
                                            )/(sw10 + serv_s);
                                    }
                                    else if(l>1&&k==0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k][l-1]*arr_1+
                                            current_pi[i][j-1][k+1][l]*serv_p+
                                            current_pi[i][j][k][l+1]*serv_s
                                            )/(sw10 + arr_p + arr_1 + serv_s);
                                    }
                                    else if(l>1&&k>0&&k+l<sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k][l-1]*arr_1+
                                            current_pi[i][j-1][k+1][l]*serv_p*(l/(double)(k+l))+
                                            current_pi[i][j][k][l+1]*serv_s*(l/(double)(k+l))
                                            )/(sw10 + arr_p + arr_1 + serv_s*w8_1_bar + serv_s*w8_1);
                                    }
                                    else if(l>1&&k>0&&k+l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k-1][l]*arr_p+
                                            current_pi[i][j][k][l-1]*arr_1
                                            )/(sw10 + serv_s*w8_1_bar + serv_s*w8_1);
                                    }
                                    else if(l==sys_size)
                                    {
                                        current_pi[i][j][k][l]=
                                            (current_pi[i-1][j][k][l]*sw01+
                                            current_pi[i][j][k][l-1]*arr_1
                                            )/(sw10 + serv_s);
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
                        if((j==0&&k==0&&l==0)||(j==1&&k>0&&k+l<=sys_size)||(j==2&&l>0&&k+l<=sys_size))
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
                        if((j==0&&k==0&&l==0)||(j==1&&k>0&&k+l<=sys_size)||(j==2&&l>0&&k+l<=sys_size))
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
                            if((j==0&&k==0&&l==0)||(j==1&&k>0&&k+l<=sys_size)||(j==2&&l>0&&k+l<=sys_size))
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
    f_high_low th,th_relay,th_servd,pb,L,Lq,W,Wq;
    double th_l0=0,th_l1=0,th_l0_relay,th_l1_relay,th_servd_l0=0,th_servd_l1=0;
    double pi_0=0,pi_1=0,pk_0=0,pk_1=0,L_0_h=0,L_0_l=0,L_1_h=0,L_1_l=0,Lq_0_h=0,Lq_0_l=0,Lq_1_h=0,Lq_1_l=0;
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
                        if((j==0&&k==0&&l==0)||(j==1&&k>0&&k+l<=sys_size)||(j==2&&l>0&&k+l<=sys_size))
                        {
                            //printf("pi[%d][%d]{%d}{%d} = %f\n",i,j,k,l,current_pi[i][j][k][l]);
                        }
                    }
                }
            }
        }
        
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                for (int k = 0; k <= sys_size; k++)
                    for (int l = 0; l <= sys_size; l++)
                    {
                        if((j==0&&k==0&&l==0)||(j==1&&k>0&&k+l<=sys_size)||(j==2&&l>0&&k+l<=sys_size))
                        {
                            //th
                            if(j==1&&k>0)
                                th.high+=serv_p*current_pi[i][j][k][l];
                            if(i==0&&j==2)
                                th_l0+=serv_s*current_pi[i][j][k][l];
                            if(i==1&&j==2)
                                th_l1+=serv_s*current_pi[i][j][k][l];
                            
                            //pb
                            if(i==0)
                            {
                                pi_0+=current_pi[i][j][k][l];
                                if(k+l==sys_size)
                                    pk_0+=current_pi[i][j][k][l];
                            }
                            if(i==1)
                            {
                                pi_1+=current_pi[i][j][k][l];
                                if(k+l==sys_size)
                                    pk_1+=current_pi[i][j][k][l];
                            }
                            //L,Lq
                            L.high+=k*current_pi[i][j][k][l];
                            L.low+=l*current_pi[i][j][k][l];
                            if(j==1){
                                Lq.high+=(k-1)*current_pi[i][j][k][l];
                                Lq.low+=l*current_pi[i][j][k][l];
                            }
                            if(j==2){
                                Lq.high+=k*current_pi[i][j][k][l];
                                Lq.low+=(l-1)*current_pi[i][j][k][l];
                            }
                        }        
                    }
        
        th_relay.high=th.high*(1-dept);
        th_l0_relay=th_l0*(1-dept);
        th_l1_relay=th_l1*(1-dept);
        th_relay.low=th_l0_relay+th_l1_relay;
        th_relay.total=th_relay.high+th_l0_relay+th_l1_relay;

        th_servd.high=th.high*dept;
        th_servd_l0=th_l0*dept;
        th_servd_l1=th_l1*dept;
        th_servd.low=th_servd_l0+th_servd_l1;
        th_servd.total=th_servd.high+th_servd.low;

        pb.total=((arr_p+arr_0)*pk_0+(arr_p+arr_1)*pk_1)/((arr_p+arr_0)*pi_0+(arr_p+arr_1)*pi_1);
        pb.high=(pk_0+pk_1)/(pi_0+pi_1);
        pb.low=(arr_0*pk_0+arr_1*pk_1)/(arr_0*pi_0+arr_1*pi_1);

        L.total=L.high+L.low;
        Lq.total=Lq.high+Lq.low;

        W.total=L.total/((arr_p+arr_0)*(pi_0-pk_0)+(arr_p+arr_1)*(pi_1-pk_1));
        W.high=L.high/(arr_p*(1-pb.high));
        W.low=L.low/(arr_0*(pi_0-pk_0)+arr_1*(pi_1-pk_1));
        
        Wq.total=Lq.total/((arr_p+arr_0)*(pi_0-pk_0)+(arr_p+arr_1)*(pi_1-pk_1));
        Wq.high=Lq.high/(arr_p*(1-pb.high));
        Wq.low=Lq.low/(arr_0*(pi_0-pk_0)+arr_1*(pi_1-pk_1));
    }
};
