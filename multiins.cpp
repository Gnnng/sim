//
//  multiins.cpp
//  mips
//
//  Created by zyhc on 14-6-4.
//  Copyright (c) 2014年 zyhc. All rights reserved.
//

#include "multiins.h"
#include "newmips.h"
#include <string>
#include <sstream>
#include <vector>
void multiins::add(std::string newins)
{
    instructions.push_back(newins);
}
std::vector<std::string> multiins::handle()
{
    //int bh;
    //int linenum;
    int havetip;
    int dataflag,textflag;
    varset.clear();
    dataflag=0;textflag=1;
    std::vector <std::string> newinstructions;
    newinstructions.clear();
    int tipaddr= 0x00000800;
    for (int i=0;i<instructions.size();i++)
    {
        if (instructions[i]==".data")
        {
            dataflag=1;
            textflag=0;
            continue;
        }
        else
        {
            if (instructions[i]==".text")
            {
                dataflag=0;
                textflag=1;
                continue;
            }
        }
        if (dataflag==1)
        {
            std::stringstream ss;
            ss.clear();
            ss<<instructions[i];
            std::string tipname="";
            std::string tiptype="";
            ss>>tipname;
            ss>>tiptype;
            if (tiptype=="")
            {
                std::stringstream linerror;
                linerror<<i<<" miss type";
                _reterror.push_back(linerror.str());
            }
            else
            {
                tip newvar;
                newvar.name=tipname;
                newvar.num=tipaddr;
                varset.push_back(newvar);
                if (tiptype==".byte")
                {
                    std::string data=ss.str();
                    for (int i=0;i<data.size();i++)
                    {
                        if (data[i]==',') data[i]=' ';
                    }
                    std::stringstream datas;
                    datas.str("");
                    datas<<data;
                    std::string temp;
                    datas>>temp>>temp;
                    int x;
                    while (datas>>x)
                    {
                        mem[tipaddr/4].byte[tipaddr%4]=x;
                        tipaddr++;
                    }
                }
                if (tiptype==".word")
                {
                    std::string data=ss.str();
                    for (int i=0;i<data.size();i++)
                    {
                        if (data[i]==',') data[i]=' ';
                    }
                    std::stringstream datas;
                    datas.str("");
                    datas<<data;
                     std::string temp;
                    datas>>temp>>temp;
                    union{
                        int i;
                        unsigned int u;
                    }xx;
                    unsigned int x;
                    while (datas>>xx.i)
                    {
                        x=xx.u;
                        char y;
                        for (int i=0;i<4;i++)
                        {
                            y=(char)(x<<(i*8)>>24);
                            mem[tipaddr/4].byte[tipaddr%4]=y;
                            tipaddr++;
                        }
                    }
                }
                if (tiptype==".double")
                {
                    std::string data=ss.str();
                    for (int i=0;i<data.size();i++)
                    {
                        if (data[i]==',') data[i]=' ';
                    }
                    std::stringstream datas;
                    datas.str("");
                    datas<<data;
                    std::string temp;
                    datas>>temp>>temp;
                    union{
                        double d;
                        unsigned long u;
                    }xx;
                    unsigned long x;
                    while (datas>>xx.d)
                    {
                        x=xx.u;
                        char y;
                        for (int i=0;i<8;i++)
                        {
                            y=(char)(x<<(i*8)>>56);
                            mem[tipaddr/4].byte[tipaddr%4]=y;
                            tipaddr++;
                        }
                    }
                }
                if (tiptype==".asciiz" || tiptype==".ascii")
                {
                    std::string data=ss.str();
                    std::stringstream datas;
                    datas.str("");
                    char *s;
                    datas<<data;
                    std::string temp;
                    datas>>temp>>temp;
                    char c;
                    while (datas.get(c))
                    {
                        if (c=='"') break;
                    }
                    while (datas.get(c))
                    {
                        if (c=='"') break;
                        mem[tipaddr/4].byte[tipaddr%4]=c;
                        tipaddr++;
                    }
                    if (tiptype==".asciiz")
                    {
                        mem[tipaddr/4].byte[tipaddr%4]=0;
                        tipaddr++;
                    }
                }

            }
        }
        else
        {
            newinstructions.push_back(instructions[i]);
        }
        
    }
    instructions=newinstructions;
    newinstructions.clear();
    
    for (int i=0;i<instructions.size();i++)
    {
        havetip=0;
        for (int j=0;j<instructions[i].length();j++)
        {
            if (instructions[i][j]==':')
            {
                std::string newname=instructions[i].substr(0,j);
                std::string judge=instructions[i].substr(j+1,instructions[i].size()-j-1);
                std::stringstream ss;
                ss.str("");
                ss<<judge;
                std::string judgeope;
                judgeope="";
                ss>>judgeope;
                instructions[i].erase(0,j+1);
                if (judgeope!="")
                {
                    newinstructions.push_back(instructions[i]);
                }
                tip newtip;
                newtip.name=newname;
                newtip.num=(int)newinstructions.size()-1;
                tipset.push_back(newtip);
                havetip=1;
            }
        }
        if (havetip==0)
        {
            std::stringstream ss;
            ss.str("");
            ss<<instructions[i];
            std::string judgeope;
            judgeope="";
            ss>>judgeope;
            if (judgeope!="")
            {
                newinstructions.push_back(instructions[i]);
            }
        }
    }
    instructions=newinstructions;
    newinstructions.clear();
    int tipi=0;
    int tipover=0;
    if (tipset.size()==0) tipover=1;
    for (int i=0;i<instructions.size();i++)
    {
        if (tipover==0 && i==tipset[tipi].num)
        {
            tipset[tipi].num=(int)newinstructions.size();
            if (tipi<newinstructions.size()-1)
                tipi++;
            else
                tipover=1;
        }
        std::stringstream ss;
        std::string s;
        ss.str("");
        std::string ins=instructions[i];
        for (int i=0;i<ins.size();i++)
        {
            if (ins[i]==',')
                ins[i]=' ';
        }
        ss<<ins;
        std::string ope;
        ss>>ope;
        if (ope=="move")
        {
            std::string r1,r2,r3;
            ss>>r1>>r2;
            std::string ins;
            ins="add "+r1+","+r2+",$zero";
            newinstructions.push_back(ins);
        }
        else
        if (ope=="blt")
        {
            std::string r1,r2,r3;
            ss>>r1>>r2>>r3;
            std::string ins;
            ins="slt $at,"+r1+","+r2;
            newinstructions.push_back(ins);
            ins="bne $at,$zero,"+r3;
            newinstructions.push_back(ins);
        }
        else
        if (ope=="bgt")
        {
            std::string r1,r2,r3;
            ss>>r1>>r2>>r3;
            std::string ins;
            ins="slt $at,"+r2+","+r1;
            newinstructions.push_back(ins);
            ins="bne $at,$zero,"+r3;
            newinstructions.push_back(ins);
        }
        else
        if (ope=="ble")
        {
            std::string r1,r2,r3;
            ss>>r1>>r2>>r3;
            std::string ins;
            ins="slt $at,"+r2+","+r1;
            newinstructions.push_back(ins);
            ins="beq $at,$zero,"+r3;
            newinstructions.push_back(ins);
        }
        else
        if (ope=="bge")
        {
            std::string r1,r2,r3;
            ss>>r1>>r2>>r3;
            std::string ins;
            ins="slt $at,"+r1+","+r2;
            newinstructions.push_back(ins);
            ins="beq $at,$zero,"+r3;
            newinstructions.push_back(ins);
        }
        if (ope=="la")
        {
            std::string r1,ime;
            std::string addr;
            std::string ins;
            addr="";
            ss>>r1>>ime;
            if (ime[0]!='(' && (ime[0]<'0' || ime[0]>'9') &&ime[0]!='+' && ime[0]!='-')
            {
                int flag;
                flag=0;
                for (int j=0;j<varset.size();j++)
                {
                    if (varset[j].name==ime)
                    {
                        std::stringstream st;
                        st<<varset[j].num;
                        addr=st.str();
                        flag=1;
                        ins="addi "+r1+",$zero,"+addr;
                        newinstructions.push_back(ins);
                    }
                }
                if (flag!=1)
                {
                    std::stringstream lineerror;
                    lineerror<<i<<" "<<"tip not exist";
                    _reterror.push_back(lineerror.str());
                }
            }
            else
            {
                ins="addi "+r1+",$zero,"+ime;

            }

            
        }
        else
        {
            newinstructions.push_back(instructions[i]);
        }
    }
    instructions=newinstructions;
    for (int i=0;i<instructions.size();i++)
    {
        //int k;
       // std::cout<<tipset[j].name<<std::endl;
        //`std::cout<<instructions[i]<<std::endl;
        //if ((k=instructions[i].find(tipset[j].name,0))!=std::string::npos)
        //{
        std::stringstream ss;
        std::string s;
        ss.str("");
        std::string ins=instructions[i];
        for (int i=0;i<ins.size();i++)
        {
            if (ins[i]==',')
                ins[i]=' ';
        }
        ss<<ins;
        std::string ope;
        ss>>ope;
        std::string reg,ime,change;
        int changeflag;
        changeflag=0;
        //int tipj;
        if (ope=="beq"||ope=="bne")
        {
            ss>>reg>>reg>>ime;
            changeflag=1;
        }
        else
        if (ope=="j"||ope=="jal")
        {
            ss>>ime;
            changeflag=1;
        }
        if (changeflag)
        {
            
            change=ime;
            if (ime[0]!='(' && (ime[0]<'0' || ime[0]>'9') &&ime[0]!='+' && ime[0]!='-')
            {
                int flag;
                flag=0;
                for (int j=0;j<tipset.size();j++)
                {
                    if (tipset[j].name==ime)
                    {
                        std::stringstream st;
                        st.str("");
                        if (ope=="j" || ope=="jal")
                        {
                            st<<tipset[j].num;
                        }
                        else
                        {
                            st<<-(i+1-tipset[j].num);
                        }
                        st>>change;
                        int k;
                        k=(int)instructions[i].find(tipset[j].name,0);
                        instructions[i].replace(k,tipset[j].name.size(),change);

                        flag=1;
                    }
                }
                if (flag!=1)
                {
                    std::stringstream lineerror;
                    lineerror<<i<<" "<<"tip not exist";
                    _reterror.push_back(lineerror.str());
                }
            }
        }
          //  }
           // std::cout<<instructions[i]<<std::endl;
    }
    return instructions;
}
std::vector<std::string> multiins::translate(std::vector<std::string> &reterror){
    std::vector<std::string> result;
    singleins oneins;
    for (int i=0;i<instructions.size();i++)
    {
        
        std::string error,oneresult;
        int insnum;
        //std::cout<<instructions[i]<<std::endl;
        int ret=oneins.single(instructions[i], error, oneresult, insnum);
        mem[i].word=insnum;
        std::stringstream lineerror;
        lineerror<<i<<" "<<error;
        if (ret==1)
        {
            _reterror.push_back(lineerror.str());
        }else
        //if (ret==0)
        {
            //std::cout<<oneresult<<std::endl;
            result.push_back(oneresult);
        }
    }
    reterror=_reterror;
    return result;
}