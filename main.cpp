//
//  main.cpp
//  mips
//
//  Created by zyhc on 14-3-30.
//  Copyright (c) 2014年 zyhc. All rights reserved.
//

#include <iostream>
#include "multiins.h"
#include <string>
#include <sstream>
#include "newmips.h"
int main(int argc, const char * argv[])
{
    using namespace std;
    string ins;
    string sin,s2;
    string error;
    string result;
    stringstream ss;
    stringstream ss1;
    stringstream st;
    stringstream ss2;
    singleins oneins;
    multiins all;
    for (int i=0;i<3;i++)
    {
        getline(cin,sin);
        all.add(sin);
//        ss2.clear();
//        ss2<<sin;
//        ss.clear();
//        ss<<sin;
//        er=oneins.single(insset,ss.str(),error,result,insnum);
//        if (er)
//            cout<<error<<endl;
//        else
//            cout<<result<<endl;
    }
    all.handle();
    vector<string> res;
    std::vector<std::string> errorset;
    res=all.translate(errorset);
    for (int i=0;i<res.size();i++)
    {
        cout<<res[i]<<endl;
    }
    for (int i=0;i<errorset.size();i++)
    cout<<errorset[i]<<endl;
//    if (ins=="ble")
//    {
//        for (int i=0;i<s2.length();i++)
//        {
//            if (s2[i]==',' || s2[i]=='(' || s2[i]==')')
//                s2[i]=' ';
//        }
//        st<<s2;
//        string reg1,reg2,exp;
//        st>>reg1>>reg2>>exp;
//        ss1<<"slt $at,"<<reg1<<","<<reg2<<endl;
//        cout<<ss1.str()<<endl;
//        single(ss1.str());
//        ss1.clear();
//        ss1<<"beq $at,$zero,"<<exp<<endl;
//        cout<<ss1.str()<<endl;
//
//        single(ss1.str());
//    }
//    else
//    {
//        single(ss2.str());
//    }
//    }
    return 0;
}

