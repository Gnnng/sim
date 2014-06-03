#include "newmips.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "exp.h"
#define R 1
#define I 2
#define J 3
using namespace std;
const string regname[32]={"zero","at","v0","v1","a0","a1","a2","a3",
    "t0","t1","t2","t3","t4","t5","t6","t7",
    "s0","s1","s2","s3","s4","s5","s6","s7",
    "t8","t9","k0","k1","gp","sp","fp","ra"};
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
void init(string filename,vector <ins> &insset)
{
    string ope;
    ifstream ifile(filename);
    while (ifile>>ope)
    {
        ins newins;
        newins.ope=ope;
        getline(ifile,newins.format);
        insset.push_back(newins);
    }
}
int single(vector <ins> insset,string sins,string &reterror,string &result,int &ins)
{
    string ope;
    int insnum;
    string typec;
    stringstream res;
    res.clear();
    stringstream ss;
    string s2;
    s2=sins;
    for (int i=0;i<s2.length();i++)
    {
        if (s2[i]==',' || s2[i]=='(' || s2[i]==')')
            s2[i]=' ';
    }
    ss.clear();
    ss<<s2;
    ss>>ope;
    insnum=-1;
    for (int i=0;i<insset.size();i++)
    {
        //cout<<insset[i].ope<<endl;
        if (ope==insset[i].ope)
        {
            insnum=i;
            break;
        }
    }
    int l = insset.size();
    stringstream st;
    st<<insset[insnum].format;
    st>>typec;
    string todo;
    char exp[100];
    int py;
    if (typec=="R")
    {
        string func;
        string reg[3];
        int regnum;
        string shamt;
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
            shamt=address(py,6);
        }
        if (todo=="w")
        {
            shamt="000000";
        }
        string add[3];
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
        string reg[2];
        string add[2];
        string begin;
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
        string begin;
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
