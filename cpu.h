#ifndef CPU_H
#define CPU_H

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
using namespace std;

class CPU{
public:
    static const int MEMSIZE = 65536;
    int PC;
    int reg[32]; 							//register
    int memory[MEMSIZE];					//memory 65536 Byte = 64 KB
    vector<int> IR;							//32bits InstructionMemory

public:
    CPU(){
        PC = 0;
        reg[0] = 0; 						//$zero
    }
    void init(vector<int> codes);
    void run(int step = 0, int line = 0);
    ~CPU(){}
};

#endif // CPU_H
