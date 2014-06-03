//
//  mips.cpp
//  mips
//
//  Created by zyhc on 14-6-3.
//  Copyright (c) 2014年 zyhc. All rights reserved.
//

#include "mips.h"
#include <iostream>
#include <string>
#include <sstream>
#include "exp.h"
#define R 1
#define I 2
#define J 3
using namespace std;
const string regname[32]={"zero","at","v0","v1","a0","a1","a2","a3",
    "t0","t1","t2","t3","t4","t5","t6","t7",
    "s0","s1","s2","s3","s4","s5","s6","s7",
    "t8","t9","k0","k1","gp","sp","fp","ra"};
int checktype(string ins,string &begin,string &tail,string &move,string &error)
{
    if (ins=="add")
    {
        begin="000000";
        tail="100000";
        move="00000";
        return R;
    }
    if (ins=="addu")
    {
        begin="000000";
        tail="100001";
        move="00000";
        return R;
    }
    if (ins=="sub")
    {
        begin="000000";
        tail="100010";
        move="00000";
        return R;
    }
    if (ins=="subu")
    {
        begin="000000";
        tail="100011";
        move="00000";
        return R;
    }
    if (ins=="and")
    {
        begin="000000";
        tail="100100";
        move="00000";
        return R;
    }
    if (ins=="or")
    {
        begin="000000";
        tail="100101";
        move="00000";
        return R;
    }
    if (ins=="xor")
    {
        begin="000000";
        tail="100110";
        move="00000";
        return R;
    }
    if (ins=="nor")
    {
        begin="000000";
        tail="100111";
        move="00000";
        return R;
    }
    if (ins=="slt")
    {
        begin="000000";
        tail="101010";
        move="00000";
        return R;
    }
    if (ins=="sltu")
    {
        begin="000000";
        tail="101011";
        move="00000";
        return R;
    }
    if (ins=="sll")
    {
        begin="000000";
        tail="000000";
        move="00000";
        return R;
    }
    if (ins=="srl")
    {
        begin="000000";
        tail="000010";
        move="00000";
        return R;
    }
    if (ins=="sra")
    {
        begin="000000";
        tail="000011";
        move="00000";
        return R;
    }
    if (ins=="sllv")
    {
        begin="000000";
        tail="000100";
        move="00000";
        return R;
    }
    if (ins=="srlv")
    {
        begin="000000";
        tail="000110";
        move="00000";
        return R;
    }
    if (ins=="srav")
    {
        begin="000000";
        tail="000111";
        move="00000";
        return R;
    }
    if (ins=="jr")
    {
        begin="000000";
        tail="001000";
        move="00000";
        return R;
    }
    if (ins=="addi")
    {
        begin="001000";
        tail="";
        move="";
        return I;
    }
    if (ins=="andi")
    {
        begin="001100";
        tail="";
        move="";
        return I;
    }
    if (ins=="ori")
    {
        begin="001101";
        tail="";
        move="";
        return I;
    }
    if (ins=="xori")
    {
        begin="001110";
        tail="";
        move="";
        return I;
    }
    if (ins=="lui")
    {
        begin="001111";
        tail="";
        move="";
        return I;
    }
    if (ins=="lw")
    {
        begin="100011";
        tail="";
        move="";
        return I;
    }
    if (ins=="sw")
    {
        begin="101011";
        tail="";
        move="";
        return I;
    }
    
    if (ins=="beq")
    {
        begin="000100";
        tail="";
        move="";
        return I;
    }
    if (ins=="bne")
    {
        begin="000101";
        tail="";
        move="";
        return I;
    }
    if (ins=="slti")
    {
        begin="001010";
        tail="";
        move="";
        return I;
    }
    if (ins=="sltiu")
    {
        begin="001011";
        tail="";
        move="";
        return I;
    }
    if (ins=="j")
    {
        begin="000010";
        tail="";
        move="";
        return J;
    }
    if (ins=="jal")
    {
        begin="000011";
        tail="";
        move="";
        return J;
    }
    error="unknown:'"+ins+"'";
    return 0;
}
string address(int ii,int x)
{
    string ret="";
    while(ii>0)
    {
        string tmp;
        tmp.insert(tmp.begin(),(char)(ii%2)+'0');
        ret=tmp+ret;
        ii=ii/2;
    }
    //cout<<ret<<endl;
    ret.insert(ret.begin(),x-ret.length(),'0');
    //cout<<ret<<endl;
    return ret;
}
string address(string reg)
{
    string ret="";
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
int single(string sins,string &reterror,string &result)
{
    int type;
    stringstream ss;
    string ins;
    string begin,tail,move;
    string s2;
    stringstream sin;
    stringstream res;
    string error;
    sin<<sins;
    sin>>ins;
    type=checktype(ins,begin,tail,move,error);
    if (type==0)
    {
        reterror=error;
        return 1;
    }
    sin>>s2;
    for (int i=0;i<s2.length();i++)
    {
        if (s2[i]==',' || s2[i]=='(' || s2[i]==')')
            s2[i]=' ';
    }
    ss<<s2;
    if (type==R)
    {
        string reg1,reg2,reg3;
        string add1,add2,add3;
        if (ins=="jr")
        {
            ss>>reg2;
            reg1="$zero";
            reg3="$zero";
        }
        else
        {
            ss>>reg1>>reg2>>reg3;
        }
        add1=address(reg2);
        add2=address(reg3);
        add3=address(reg1);
        if (add1=="XXX")
        {
            reterror="unknwon reg:'"+reg2+"'";
            return 1;
        }
        if (add2=="XXX")
        {
            reterror="unknwon reg:'"+reg3+"'";
            return 1;
        }
        if (add3=="XXX")
        {
            reterror="unknwon reg:'"+reg1+"'";
            return 1;
        }
        res<<begin<<add1<<add2<<add3<<move<<tail<<endl;
    }
    if (type==I)
    {
        string reg1,reg2;
        string add1,add2;
        char exp[100];
        int py;
        if (ins!="sw" && ins!="lw")
        {
            ss>>reg1>>reg2>>exp;
        }
        else
        {
            if (ins=="lui")
            {
                ss>>reg1>>exp;
                reg2="$zero";
            }
            ss>>reg1>>exp>>reg2;
        }
        add1=address(reg2);
        add2=address(reg1);
        if (add1=="XXX")
        {
            reterror="unknwon reg:'"+reg2+"'";
            return 1;
        }
        if (add2=="XXX")
        {
            reterror="unknwon reg:'"+reg1+"'";
            return 1;
        }
        py=(int)expression(exp);
        res<<begin<<add1<<add2<<address(py,16)<<endl;
    }
    if (type==J)
    {
        int add;
        ss>>add;
        res<<begin<<address(add,26)<<endl;
    }
    result=res.str();
    return 0;
}
