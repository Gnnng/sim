//
//  multiins.h
//  mips
//
//  Created by zyhc on 14-6-4.
//  Copyright (c) 2014年 zyhc. All rights reserved.
//


//基本用法如原来，需要初始化的内存从multiins类的public属性mem获得，是一个union属性的数组，然后
#ifndef __mips__multiins__
#define __mips__multiins__

#include <iostream>
#include <string>
#include <vector>
struct tip{
    std::string name;
    int num;
};
union mips_mem_type{
    char byte[4];
    int word;
};
class multiins{
private:
    std::vector <std::string> instructions;
    std::vector <tip> tipset;
    std::vector <tip> varset;
    std::vector<std::string> _reterror;
public:
    mips_mem_type mem[512*1024];
    void add(std::string);
    std::vector<std::string> handle();
    std::vector<std::string> translate(std::vector<std::string> &reterror);
};
#endif /* defined(__mips__multiins__) */
