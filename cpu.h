/*
    Single Clock CPU
    Big-Endian
    32bits
*/
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;

#define $zero reg[0]
#define $at reg[1]
#define $v0 reg[2]
#define $v1 reg[3]
#define $a0 reg[4]
#define $a1 reg[5]
#define $a2 reg[6]
#define $a3 reg[7]
#define $t0 reg[8]
#define $t1 reg[9]
#define $t2 reg[10]
#define $t3 reg[11]
#define $t4 reg[12]
#define $t5 reg[13]
#define $t6 reg[14]
#define $t7 reg[15]
#define $s0 reg[16]
#define $s1 reg[17]
#define $s2 reg[18]
#define $s3 reg[19]
#define $s4 reg[20]
#define $s5 reg[21]
#define $s6 reg[22]
#define $s7 reg[23]
#define $t8 reg[24]
#define $t9 reg[25]
#define $k0 reg[26]
#define $k1 reg[27]
#define $gp reg[28]
#define $sp reg[29]
#define $fp reg[30]
#define $ra reg[31]

class CPU{
public:
    //MMU
    static const int MEMSIZE = (1 << 21) - 1; //memory 2MB Byte
//    static const int MEMSIZE = 65536; //memory 2MB Byte
    string video;
    int PC;
    int reg[32]; 							//register
    int memory[MEMSIZE];					//memory 65536 Byte = 64 KB
    //mmu_class memory;						///
    vector<int> IR;							//32bits InstructionMemory

public:
    CPU(){
        PC = 0;
        $zero = 0; 						//$zero
        $sp = MEMSIZE; 					//$sp
        video = "";
    }
    //mmu(word vir_addr,int write,word wdata)
    int LW(int addr);
    void SW(int addr, int data);
    int LB(int addr);
    void SB(int addr, int data);
    //Syscall
    void print_char(int addr);
    void print_int(int addr);
    void print_string(int addr);
    void init(vector<int> _IR);
    //void init(string FileName);
    void run(int step = 0, int line = 0);
    ~CPU(){}
};
