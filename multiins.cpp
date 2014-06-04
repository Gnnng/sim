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
                ss << tipset[j].num;
                ss >> s;
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
        if (ret==1)
        {
            reterror.push_back(error);
        }else
        //if (ret==0)
        {
            //std::cout<<oneresult<<std::endl;
            result.push_back(oneresult);
        }
    }
    return result;
}
