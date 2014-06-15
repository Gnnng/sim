//
//  single_remips.h
//  remips
//
//  Created by zyhc on 14-6-6.
//  Copyright (c) 2014年 zyhc. All rights reserved.
//

#ifndef __remips__single_remips__
#define __remips__single_remips__

#include <iostream>
#define R 0
#define I 1
#define J 2
typedef unsigned int remips_dword;//my OS is 64-bit

const std::string remips_regname[32]={"zero","at","v0","v1","a0","a1","a2","a3",
    "t0","t1","t2","t3","t4","t5","t6","t7",
    "s0","s1","s2","s3","s4","s5","s6","s7",
    "t8","t9","k0","k1","gp","sp","fp","ra"};
const std::string opetoI[][3]={
    "addi"	,"001000","n",
    "addiu"	,"001001","n",
    "andi"	,"001100","n",
    "ori"	,"001101","n",
    "xori"	,"001110","n",
    "lui"	,"001111","n",
    "lw"	,"100011","e",
    "sw"	,"101011","e",
    "beq"	,"000100","n",
    "bne"	,"000101","n",
    "slti"	,"001010","n",
    "sltiu"	,"001011","n",
    "x","x","x"
};
const std::string opetoJ[][2]={
    "000010","j",
    "000011","jal"
    "x"
};
const std::string fuctoR[][6]={
    "100000","add","2","3","1","n",
    "100001","addu","2","3","1","n",
    "100010","sub","2","3","1","n",
    "100011","subu","2","3","1","n",
    "100100","and","2","3","1","n",
    "100101","or","2","3","1","n",
    "100110","xor","2","3","1","n",
    "100111","nor","2","3","1","n",
    "101010","slt","2","3","1","n",
    "101011","sltu","2","3","1","n",
    "000000","sll","n","2","1","e",
    "000010","srl","n","2","1","e",
    "000011","sra","n","2","1","e",
    "000100","sllv","3","3","1","n",
    "000110","srlv","3","2","1","n",
    "000111","srav","3","2","1","n",
    "001000","jr","1","n","n","n",
    "x","x","x","x"
};


class singleRemips
{
private:
public:
    remips_dword num(std::string s);
    std::string check(remips_dword begin,remips_dword tail,int &type,int &openum);
    std::string name(remips_dword s);
    std::string translate(remips_dword sin);
};
#endif /* defined(__remips__single_remips__) */
