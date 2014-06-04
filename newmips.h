#ifndef __mips__newmips__
#define __mips__newmips__

#include <iostream>
#include <string>
#include <vector>
struct ins{
    std::string ope;
    std::string format;
};
const std::string regname[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};
const std::string datain[31]={
    "add	 R 100000 r 3 r 1 r 2 w",
    "addu R 100001 r 3 r 1 r 2 w",
    "sub	 R 100010 r 3 r 1 r 2 w",
    "subu R 100011 r 3 r 1 r 2 w",
    "and	 R 100100 r 3 r 1 r 2 w",
    "or	 R 100101 r 3 r 1 r 2 w",
    "xor	 R 100110 r 3 r 1 r 2 w",
    "nor	 R 100111 r 3 r 1 r 2 w",
    "slt	 R 101010 r 3 r 1 r 2 w",
    "sltu R 101011 r 3 r 1 r 2 w",
    "sll	 R 000000 r 3 r 2 w 1 r",
    "srl	 R 000010 r 3 r 2 w 1 r",
    "sra	 R 000011 r 3 r 2 w 1 r",
    "sllv R 000100 r 3 r 2 r 1 w",
    "srlv R 000110 r 3 r 2 r 1 w",
    "srav R 000111 r 3 r 2 r 1 w",
    "jr	 R 001000 r 1 w 2 w 3 w",
    "addi  	I	001000 r 2 r 1 i",
    "addiu  	I	001001 r 2 r 1 i",
    "andi  	I	001100 r 2 r 1 i",
    "ori	  	I	001101 r 2 r 1 i ",
    "xori  	I	001110 r 2 r 1 i",
    "lui	  	I	001111 r 2 w 1 i",
    "lw	  	I	100011 r 2 i r 1",
    "sw	  	I	101011 r 2 i r 1",
    "beq	  	I	000100 r 2 r 1 i",
    "bne	  	I	000101 r 2 r 1 i",
    "slti  	I	001010 r 2 r 1 i",
    "sltiu  	I	001011 r 2 r 1 i",
    "j J 000010",
    "jal J 000011"
};
typedef std::vector<ins> INSSET;
class singleins{
private:
    INSSET insset;
public:
    singleins(std::string filename);
    singleins();
    std::string address(int ii,int x);
    std::string address(std::string reg);
    void init(std::string filename);
    void init();
    int single(std::string sins,std::string &reterror,std::string &result,int &ins);
};
#endif /* defined(__mips__newmips__) */
