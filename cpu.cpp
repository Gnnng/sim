#include "cpu.h"

void CPU::init(vector<int> _IR){
    PC = 0;
    reg[0] = 0;							//$zero
//    IR.clear(); //##
    IR = _IR;
    memset(reg, 0, sizeof(reg));
}

void CPU::run(int step, int line){
    //step = 0 && line = 0 means RUN ALL
    //step = 1 means RUN ONCE
    //line = X, means Breakpoint at line X;
    int op, rs, rt, rd, sft, func, adr, dat0, dat1;
    while((PC >> 2) < IR.size()){
        int now = IR[PC >> 2];
        op = (now >> 26) & 0x3F;		//6
        rs = (now >> 21) & 0x1F;		//5
        rt = (now >> 16) & 0x1F;		//5
        rd = (now >> 11) & 0x1F;    	//5
        sft = (now >> 6) & 0x1F;        //5
        func = now & 0x3F;				//6
        dat0 = (now & 0xFFFF);			//16
        dat1 = dat0;
        if (dat0 & (1 << (16 - 1))) dat1 |= 0xFFFF0000;
        adr = (now & 0x3FFFFFF); 		//26
        //printf("%d\n", op);
        switch(op){
            //00 0000 R-type
            case 0x00:
                switch(func){
                    //10 0000 add
                    case 0x20:
                        //puts("add");
                        reg[rd] = reg[rs] + reg[rt];
                        ///CF & OF
                        break;

                    //10 0001 addu
                    case 0x21:
                        reg[rd] = reg[rs] + reg[rt];
                        break;

                    //10 0100 and
                    case 0x24:
                        reg[rd] = reg[rs] & reg[rt];
                        break;

                    //00 1000 jr
                    case 0x08:
                        PC = reg[rs];
                        break;

                    //10 0111 nor
                    case 0x27:
                        reg[rd] = ~(reg[rs] | reg[rt]);
                        break;

                    //10 0101 or
                    case 0x25:
                        reg[rd] = reg[rs] | reg[rt];
                        break;

                    //00 0000 sll
                    case 0x00:
                        reg[rd] = reg[rt] << sft;
                        break;

                    //00 0100 sllv
                    case 0x04:
                        reg[rd] = reg[rt] << reg[rs];
                        break;

                    //10 1010 slt
                    case 0x2A:
                        reg[rd] = (reg[rs] < reg[rt]);
                        break;

                    //10 1011 sltu
                    case 0x2B:
                        reg[rd] = (unsigned int)(reg[rs]) < (unsigned int)(reg[rt]);
                        break;

                    //00 0011 sra
                    case 0x03:
                        reg[rd] = reg[rt] >> sft;
                        break;
                    //00 0111 srav
                    case 0x07:
                        reg[rd] = reg[rt] >> reg[rs];
                        break;

                    //00 0010 srl
                    case 0x02:
                        reg[rd] = (unsigned int)(reg[rt]) >> sft;
                        break;
                    //00 0110 srlv
                    case 0x06:
                        reg[rd] = (unsigned int)(reg[rt]) >> reg[rs];
                        break;

                    //10 0010 sub
                    case 0x22:
                        reg[rd] = reg[rs] - reg[rt];
                        //OF & CF
                        break;

                    //10 0011 subu
                    case 0x23:
                        reg[rd] = reg[rs] - reg[rt];
                        break;

                    //10 0110 xor
                    case 0x26:
                        reg[rd] = reg[rs] ^ reg[rt];
                        break;

                    default :
                        assert(0);
                }
                break;
            //End of R-type

            //00 1000 addi
            case 0x08:
                reg[rt]  = reg[rs] + dat1;
                //OF & CF;
                break;

            //00 1001 addiu
            case 0x09:
                reg[rt] = reg[rs] + dat0;
                break;

            //00 1100 andi
            case 0x0C:
                reg[rt] = reg[rs] & dat0;
                break;

            //00 0100 beq
            case 0x04:
                //puts("beq");
                if (reg[rt] == reg[rs]) PC += dat1 << 2;///
                break;

            //00 0101 bne
            case 0x05:
                if (reg[rt] != reg[rs]) PC += dat1 << 2;///
                break;

            //10 0000 lb
            case 0x20:
                reg[rt] = memory[reg[rs] + dat1];
                break;
            //10 1000 sb
            case 0x28:
                memory[reg[rs] + dat1] = reg[rt] & 0xFF;
                break;

            //10 0011 lw
            case 0x23:
                //puts("lw");
                reg[rt] = (memory[reg[rs] + dat1 + 0] << 24)
                         |(memory[reg[rs] + dat1 + 1] << 16)
                         |(memory[reg[rs] + dat1 + 2] << 8)
                         |(memory[reg[rs] + dat1 + 3] << 0);
                break;
            //10 1011 sw
            case 0x2B:
                memory[reg[rs] + dat1 + 0] = ((reg[rt] >> 24) & 0xFF);
                memory[reg[rs] + dat1 + 1] = ((reg[rt] >> 16) & 0xFF);
                memory[reg[rs] + dat1 + 2] = ((reg[rt] >> 8)  & 0xFF);
                memory[reg[rs] + dat1 + 3] = ((reg[rt] >> 0)  & 0xFF);
                break;

            //00 1101 ori
            case 0x0D:
                reg[rt] = reg[rs] | dat0;
                break;

            //00 1111 lui:
            case 0x0F:
                reg[rt] = dat0 << 16;
                break;

            //00 1110 xori
            case 0x0E:
                reg[rt] = reg[rs] ^ dat0;
                break;

            //00 1010 slti:
            case 0x0A:
                reg[rt] = (reg[rs] < dat1);
                break;

            //00 1011 sltiu:
            case 0x0B:
                reg[rt] = (reg[rs] < dat0);
                break;

            //End of I-type

            //00 0010 j
            case 0x02:
                //puts("j");
                PC = PC & 0xF0000000 | ((adr - 1) << 2);
                break;

            //00 0011 jal
            case 0x03:
                reg[31] = PC + 4; 	//$ra
                PC = PC & 0xF0000000 | ((adr - 1) << 2);
                break;

            default :
                assert(0);
            //End of J-type
        }
        PC += 4;
        if (step || (PC >> 2) == line) break;
    }
}

