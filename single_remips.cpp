//
//  single_remips.cpp
//  remips
//
//  Created by zyhc on 14-6-6.
//  Copyright (c) 2014年 zyhc. All rights reserved.
//

#include "single_remips.h"
#include <string>
#include <sstream>
remips_dwrd singleRemips::num(std::string s)
{
    remips_dwrd ret=0;
    for (int i=0;i<s.length();i++)
    {
        if (s[i]=='1')
        {
            ret=ret*2+1;
        }
        else ret=ret*2;
    }
    return ret;
}

std::string singleRemips::check(remips_dwrd begin,remips_dwrd tail,int &type,int &openum)
{
    std::string ret;
    if (begin==num("000000"))
    {
        type=0;
        for (int i=0;fuctoR[i][0]!="x";i++)
        {
            if (num(fuctoR[i][0])==tail)
            {
                openum=i;
                return fuctoR[i][1];
            }
        }
    }
    for (int i=0;opetoI[i][0]!="x";i++)
    {
        if (num(opetoI[i][1])==begin)
        {
            type=1;
            openum=i;
            return opetoI[i][0];
        }
    }
    for (int i=0;opetoJ[i][0]!="x";i++)
    {
        if (num(opetoJ[i][0])==begin)
        {
            type=2;
            openum=i;
            return opetoJ[i][1];
        }
    }

    return "xxx";
}
std::string singleRemips::name(remips_dwrd s)
{
    return "$"+remips_regname[s];
}

std::string singleRemips::translate(remips_dwrd sin)
{
    using namespace std;
    string ins;
    stringstream res;
    string sins;
    remips_dwrd sig,tail;
    string anssig,ans;
    int type;
    int openum;
    //cin>>sins;
    //sin=num(sins);
    sig=(sin & 0xFC000000)>>26;
    tail=(sin &0x0000003F);
    //res<<tail<<endl;
    anssig=check(sig,tail,type,openum);
    if (type==0)
    {
        unsigned int reg[4];
        std::string regname[4],exp;
        reg[1]=(sin & 0x03E00000)>>21;
        reg[2]=(sin & 0x001F0000)>>16;
        reg[3]=(sin & 0x0000FA00)>>11;
        exp= (sin & 0x000007C0)>>6;
        regname[1]="";regname[2]="",regname[3]="";
        for (int i=1;i<=3;i++)
        {
            if (fuctoR[openum][0]!="n")
            {
                regname[fuctoR[openum][i+1][0]-'0']=name(reg[i]);
            }
        }
        //res<<"1"<<regname[1]<<"2"<<regname[2]<<"3"<<regname[3]<<endl;
        if (fuctoR[openum][5]=="e")
        {
            res<<anssig+" "<<regname[1]<<",";
            if (regname[2]!="")
                res<<regname[2]<<",";
            res<<exp;
        }
        else
        {
            res<<anssig<<" ";
            for (int i=1;i<=3;i++)
            {
                if (regname[i]!="")
                {
                    if (i!=1)
                        res<<",";
                    res<<regname[i];
                }
            }
        }
    }
    if (type==1)
    {
        unsigned int reg1,reg2,exp;
        reg1=(sin & 0x03E00000)>>21;
        reg2=(sin & 0x001F0000)>>16;
        exp=sin & 0x0000FFFF;
        if (opetoI[openum][2]=="e")
        {
            res<<anssig+" "+name(reg2)+","<<exp<<"("<<name(reg1)<<")"<<endl;
        }
        else
        {
            res<<anssig+" "+name(reg2)+","+name(reg1)+","<<exp<<endl;
        }
    }
    if (type==2)
    {
        unsigned long exp;
        exp=sin & 0x03FFFFFF;
        res<<anssig+" "<<exp<<endl;
    }

    return res.str();
}

