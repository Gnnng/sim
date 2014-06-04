//
//  multiins.h
//  mips
//
//  Created by zyhc on 14-6-4.
//  Copyright (c) 2014年 zyhc. All rights reserved.
//

#ifndef __mips__multiins__
#define __mips__multiins__

#include <iostream>
#include <string>
#include <vector>
struct tip{
    std::string name;
    int num;
};
class multiins{
private:
    std::vector <std::string> instructions;
    std::vector <tip> tipset;
public:
    void add(std::string);
    void handle();
    std::vector<std::string> translate(std::vector<std::string> &reterror);
};
#endif /* defined(__mips__multiins__) */
