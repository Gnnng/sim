//
//  newmips.cpp
//  mips
//
//  Created by zyhc on 14-6-3.
//  Copyright (c) 2014年 zyhc. All rights reserved.
//

#include "newmips.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "exp.h"
singleins::singleins(std::string filename)
{
    init(filename);
}
singleins::singleins()
{
    init();
}
std::string singleins::address(int ii,int x)
{
    std::string ret="";
    if (ii<0)
    {
        ii = ii & 0x7FFFFFFF;
    }
    while(ii>0)
    {
        std::string tmp;
        tmp.insert(tmp.begin(),(char)(ii%2)+'0');
        ret=tmp+ret;
        ii=ii/2;
    }
    //cout<<ret<<endl;
    if (ret.length()<=x)
    {
        ret.insert(ret.begin(),x-ret.length(),'0');
    }
    else
    {
        ret.erase(0,ret.length()-x);
    }
    //std::cout<<ret<<std::endl;
    return ret;
}
std::string singleins::address(std::string reg)
{
    std::string ret="";
    reg.erase(reg.begin(),reg.begin()+1);
    // cout<<reg<<endl;
    for (int i=0;i<32;i++)
    {
        if (reg==regname[i])
        {
            ret=address(i,5);
            //cout<<ret<<endl;
            return ret;
        }
    }
    return "XXX";
}
void singleins::init(std::string filename)
{
    std::string ope;
    std::ifstream ifile(filename);
    while (ifile>>ope)
    {
        ins newins;
        newins.ope=ope;
        getline(ifile,newins.format);
        insset.push_back(newins);
    }
}
void singleins::init()
{
    std::string ope;
    std::stringstream istring;
    for (int i=0;i<31;i++)
    {
        std::string temp;
        temp=datain[i]+"\n";
        istring.clear();
        istring<<datain[i];
        istring>>ope;
        ins newins;
        newins.ope=ope;
        getline(istring,newins.format);
        insset.push_back(newins);
    }
}
int singleins::single(std::string sins,std::string &reterror,std::string &result,int &ins)
{
    std::string ope;
    int insnum;
    std::string typec;
    std::stringstream res;
    res.clear();
    std::stringstream ss;
    std::string s2;
    s2=sins;
    for (int i=0;i<s2.length();i++)
    {
        if (s2[i]==',' || s2[i]=='(' || s2[i]==')')
            s2[i]=' ';
    }
    ss.clear();
    ss<<s2;
    ss>>ope;
    if (ope=="syscall")
    {
        result="00000000000000000000000000001100";
        ins=0x0000000C;
        return 0;
    }
    if (ope=="")
    {
        return -1;
    }
    insnum=-1;
    int i;
    for (i=0;i<insset.size();i++)
    {
        //cout<<insset[i].ope<<endl;
        if (ope==insset[i].ope)
        {
            insnum=i;
            break;
        }
    }
    if (i==insset.size())
    {
        reterror="unkown operator:'"+ope+"'";
        return 1;
    }
    std::stringstream st;
    st<<insset[insnum].format;
    st>>typec;
    std::string todo;
    char exp[100];
    int py;
    if (typec=="R")
    {
        std::string func;
        std::string reg[3];
        int regnum;
        std::string shamt;
        st>>func;
        for (int i=0;i<3;i++)
        {
            st>>todo;
            if (todo=="r")
            {
                st>>regnum;
                ss>>reg[regnum-1];
            }
            if (todo=="w")
            {
                st>>regnum;
                reg[regnum-1]="$zero";
            }
        }
        st>>todo;
        if (todo=="r")
        {
            ss>>exp;
            py=(int)expression(exp);
            shamt=address(py,5);
        }
        if (todo=="w")
        {
            shamt="00000";
        }
        std::string add[3];
        for (int i=0;i<3;i++)
        {
            add[i]=address(reg[i]);
            if (add[i]=="XXX")
            {
                reterror="unknwon reg:'"+reg[i]+"'";
                return 1;
            }
        }
        res<<"000000"<<add[0]<<add[1]<<add[2]<<shamt<<func;
    }
    if (typec=="I")
    {
        std::string reg[2];
        std::string add[2];
        std::string begin;
        int regnum;
        st>>begin;
        for (int i=0;i<3;i++)
        {
            st>>todo;
            if (todo=="r")
            {
                st>>regnum;
                ss>>reg[regnum-1];
            }
            if (todo=="w")
            {
                st>>regnum;
                reg[regnum-1]="$zero";
            }
            if (todo=="i")
            {
                ss>>exp;
            }
        }
        py=(int)expression(exp);
        for (int i=0;i<2;i++)
        {
            add[i]=address(reg[i]);
            if (add[i]=="XXX")
            {
                reterror="unknwon reg:'"+reg[i]+"'";
                return 1;
            }
        }
        res<<begin<<add[0]<<add[1]<<address(py,16);
    }
    if (typec=="J")
    {
        std::string begin;
        st>>begin;
        ss>>exp;
        py=(int)expression(exp);
        res<<begin<<address(py,26);
    }
    result=res.str();
    ins=0;
    for (int i=0;i<32;i++)
    {
        ins=ins*2+(result[i]-'0');
    }
    return 0;
}

