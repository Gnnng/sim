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
void multiins::handle()
{
    int bh;
    for (int i=0;i<instructions.size();i++)
    {
        for (int j=0;j<instructions[i].length();j++)
        {
            if (instructions[i][j]==':')
            {
                std::string newname=instructions[i].substr(0,j);
                tip newtip;
                newtip.name=newname;
                newtip.num=i;
                tipset.push_back(newtip);
                instructions[i].erase(0,j+1);
            }
        }
    }
    for (int i=0;i<instructions.size();i++)
    {
        for (int j=0;j<tipset.size();j++)
        {
            int k;
           // std::cout<<tipset[j].name<<std::endl;
            //`std::cout<<instructions[i]<<std::endl;
            if ((k=instructions[i].find(tipset[j].name,0))!=std::string::npos)
            {
                std::stringstream ss;
                std::string s;
                
                ss.str("");
                ss<<instructions[i];
                std::string ope;
                ss>>ope;
                ss.str("");
                if (ope=="beq"||ope=="bne")
                {
                    ss<<tipset[j].num-i;
                }
                else
                {
                    ss << tipset[j].num;
                }
                s=ss.str();
                std::cout<<s<<std::endl;
                instructions[i].replace(k,tipset[j].name.size(),s);
            }
           // std::cout<<instructions[i]<<std::endl;
        }
    }
}
std::vector<std::string> multiins::translate(std::vector<std::string> &reterror){
    std::vector<std::string> result;
    reterror.clear();
    singleins oneins;
    for (int i=0;i<instructions.size();i++)
    {
        
        std::string error,oneresult;
        int insnum;
        //std::cout<<instructions[i]<<std::endl;
        int ret=oneins.single(instructions[i], error, oneresult, insnum);
        std::stringstream lineerror;
        lineerror<<i<<" "<<error;
        if (ret==1)
        {
            reterror.push_back(lineerror.str());
        }else
        //if (ret==0)
        {
            //std::cout<<oneresult<<std::endl;
            result.push_back(oneresult);
        }
    }
    return result;
}