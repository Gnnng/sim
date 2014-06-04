//
//  main.c
//  expression
//
//  Created by zyhc on 13-6-5.
//  Copyright (c) 2013年 zyhc. All rights reserved.
//
#ifndef __mips__exp__
#define __mips__exp__

#include <stdio.h>
#include <stdlib.h>
double calc(double x,double y,char p)
{
    switch (p)
    {
        case '+': return x+y;break;
        case '-': return x-y;break;
        case '/': return x/y;break;
        case '*': return x*y;break;
        //default:printf("wrong!");break;
    }
    return 0;
}
double expression(char* exp)
{
    int s1[100];
    double pm,pj;
    double num[100];
    int m,sk,nk,fir,la;
    int numflag,braflag,begflag,negflag,pointflag;
    char *p;
    begflag=1;
    la=0;
    pointflag=0;
    fir=0;
    negflag=0;
    p=exp;
    pj=0.1;
    m=0;
    pm=0;
    numflag=0;
    sk=0;
    nk=0;
    while (*p!=0)
    {
        if ( ( *p<='9' && *p>='0' ) || *p=='.')
        {
            if (*p=='.')
            {
                pointflag=1;
            }
            else
            {
                if (pointflag==0)
                {
                    m=m*10+*p-'0';
                }
                else
                {
                    pm=pm+(*p-'0')*pj;
                    pj=pj/10;
                }
            }
            numflag=1;
        }
        else
        {
            if (numflag==1)
            {
                nk++;
                num[nk]=m+pm;
                numflag=0;
                m=0;
                pm=0;
                pj=0.1;
                pointflag=0;
                if (negflag==1)
                {
                    num[nk]=-num[nk];
                    negflag=0;
                }
                
            }
        }
        if (*p==')')
        {
            braflag=0;
            while (la>0)
            {
                if (s1[la]!='(')
                {
                    num[nk-1]=calc(num[nk-1],num[nk],s1[la]);
                    nk--;
                    la--;
                }
                else
                {
                    la--;
                    braflag=1;
                    break;
                }
            }
            if (braflag==0)
            {
                //printf("can't find '('\n");
                //system("pause");
            }
        }
        if (*p=='-' && begflag==1)
        {
            negflag=1;
        }
        else
            if (*p=='+' || *p=='-')
            {
                if (la>0 && s1[la]!='(')
                {
                    num[nk-1]=calc(num[nk-1],num[nk],s1[la]);
                    nk--;
                    la--;
                }
                la++;
                s1[la]=*p;
            }
        if (*p=='*' || *p=='/')
        {
            if (la>0 && s1[la]!='(' && s1[la]!='+' && s1[la]!='-')
            {
                num[nk-1]=calc(num[nk-1],num[nk],s1[la]);
                nk--;
                la--;
            }
            la++;
            s1[la]=*p;
        }
        if (*p=='(')
        {
            la++;
            s1[la]='(';
            begflag=1;
        }
        else
        {
            begflag=0;
        }
        p++;
    }
    if (numflag==1)
    {
        nk++;
        num[nk]=m;
        if (negflag==1)
        {
            num[nk]=-num[nk];
            negflag=0;
        }
        numflag=0;
    }
    while (la>0)
    {
        num[nk-1]=calc(num[nk-1],num[nk],s1[la]);
        la--;
        nk--;
    }
    return num[1];
}
#endif