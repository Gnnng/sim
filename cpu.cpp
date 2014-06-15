#include "mmu.h"
#include "CPU.h"


//WORD mmu(word vir_addr,int write,word wdata)

/*
void CPU::init(string FileName){
	PC = 0;
	reg[0] = 0;							//$zero
	reg[29] = MEMSIZE; 					//$sp
	video = "";
	//for (int i = 0; i < 100; i++) memory[i] = i;
	FILE *COE = fopen(FileName.c_str(), "r");
	assert(COE != NULL);
	static char line[100];
	while(fscanf(COE, "%s", line) != EOF){
		int len = strlen(line);
		assert(len == 32);
		int now = 0;
		for (int i = 0; i < len; i++){
			assert(line[i] == '0' || line[i] == '1');
			if (line[i] == '1') now |= 1 << (31 - i);
		}
		IR.push_back(now);
	}
	fclose(COE);
	/////
}
//*/

//*
void CPU::init(vector<int> _IR){
	PC = 0;
	$zero = 0;							//$zero
	$sp = MEMSIZE; 						//$sp
	video = "";
	IR = _IR;
//	memory = mmu_class();				//new
}
//*/

//address of Byte

int CPU::LW(int addr){
	int row = addr / 4;
	int offset = addr % 4;
	int word0 = memory.mmu(row, 0, 0);
	int word1 = memory.mmu(row + 1, 0, 0);
	int word;
	switch (offset){
		case 0 : word = word0; break;
		case 1 : word = (word0 <<  8) | ((word1 >> 24) & 0xFF); break;
		case 2 : word = (word0 << 16) | ((word1 >> 16) & 0xFFFF); break;
		case 3 : word = (word0 << 24) | ((word1 >>  8) & 0xFFFFFF); break;
	}
	return word;
}

void CPU::SW(int addr, int data){
	int row = addr / 4;
	int offset = addr % 4;
	int word0 = memory.mmu(row, 0, 0);
	int word1 = memory.mmu(row + 1, 0, 0);
	switch (offset){
		case 0 : 
			word0 = word0;
			word1 = word1;
			break;
		case 1 :
			word0 ^= (word0 ^ (data >> 8)) & 0xFFFFFF;
			word1 ^= (((word1 >> 24) ^ data) & 0xFF) << 24; 
			break;
			
		case 2 :
			word0 ^= (word0 ^ (data >> 16)) & 0xFFFF;
			word1 ^= (((word1 >> 16) ^ data) & 0xFFFF) << 16;
			break;
			
		case 3 : 
			word0 ^= (word0 ^ (data >> 24)) & 0xFF;
			word1 ^= (((word1 >> 8) ^ data) & 0xFFFFFF) << 8;
			break;
	}
	memory.mmu(row, 1, word0);
	memory.mmu(row + 1, 1, word1);
}

int CPU::LB(int addr){
	int row = addr / 4;
	int offset = addr % 4;
	int word = memory.mmu(row, 0, 0);
	int byte = (word >> ((3 - offset) * 8)) & 0xFF;
	return byte;
}

void CPU::SB(int addr, int data){
	int row = addr / 4;
	int offset = addr % 4;
	int word = memory.mmu(row, 0, 0);
	int byte = (word >> ((3 - offset) * 8)) & 0xFF;
	word ^= (byte ^ (data & 0xFF)) << ((3 - offset) * 8);
	memory.mmu(row, 1, word);
}

void CPU::print_char(int addr){
	char ch = LB(addr);
	video += ch;
}

void CPU::print_int(int addr){
	int num = LW(addr);
	stringstream temp;
	temp << num;
	video += temp.str();
}

void CPU::print_string(int addr){
	int ch;
	//ch != '\0'
	while(ch = LB(addr)){
		video += ch;
		addr++;
	}
}

void CPU::run(int step, int line){	
	//step = 0 && line = 0 means RUN ALL
	//step = 1 means RUN ONCE
	//line = X, means Breakpoint at line X;
	int op, rs, rt, rd, sft, func, adr, dat0, dat1;
	while((PC >> 2) < IR.size()){
		//printf("%d\n", PC >> 2);
		int now = IR[PC >> 2];
		op = (now >> 26) & 0x3F;		//6
		rs = (now >> 21) & 0x1F;		//5
		rt = (now >> 16) & 0x1F;		//5
		rd = (now >> 11) & 0x1F;    	//5
		sft = (now >> 6) & 0x1F;        //5
		func = now & 0x3F;				//6
		PC += 4;
		
		dat0 = (now & 0xFFFF);			//16
		dat1 = dat0;
		if (dat0 & (1 << (16 - 1))) dat1 |= 0xFFFF0000;
		
		adr = (now & 0x3FFFFFF); 		//26 address of PC
		//printf("%d\n", op);
		
		int addr = reg[rs] + dat1;		//32 address of MEM
		/*
		int row, offset, temp;			///
		row = (reg[rs] + dat1) / 4;
		offset =  (reg[rs] + dat1) % 4;
		*/
		
		if (now == 0) continue;			//nop;
		switch(op){
			//00 0000 R-type
			case 0x00: 	
				switch(func){
					//Syscall $v0 $a0
					case 0x0C:
						//$v0
						switch($v0){
							//print_int
							case 1 :
								print_int($a0);
								break;
							
							//print_string
							case 4 :
								print_string($a0);
								break;
							
							//print_char
							case 11 : 
								print_char($a0);
								break;
								
							default :
								assert(0);
								break;
						}
						
						break;
				
					//10 0000 add
					case 0x20: 
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
						PC = reg[rs]; //##
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
				if (reg[rt] == reg[rs]) PC += dat1 << 2;///
				break;
			
			//00 0101 bne
			case 0x05:
				if (reg[rt] != reg[rs]) PC += dat1 << 2;///
				break;
				
			//10 0000 lb
			case 0x20:
				reg[rt] = LB(addr);
				///reg[rt] = (memory[row] >> ((3 - offset) * 8)) & 0xFF; 
				break;
				
			//10 1000 sb
			case 0x28:
				SB(addr, reg[rt]);
				///temp = (memory[row] >> ((3 - offset) * 8)) & 0xFF; 
				///memory[row] ^= (temp ^ (reg[rt] & 0xFF)) << ((3 - offset) * 8);
				break;
			
			//10 0011 lw
			case 0x23:
				reg[rt] = LW(addr);
				/*
				switch (offset){
					case 0 : reg[rt] = memory[row]; break;
					case 1 : reg[rt] = (memory[row] << 8) | ((memory[row + 1] >> 24) & 0xFF); break;
					case 2 : reg[rt] = (memory[row] << 16) | ((memory[row + 1] >> 16) & 0xFFFF); break;
					case 3 : reg[rt] = (memory[row] << 24) | ((memory[row + 1] >> 8) & 0xFFFFFF); break;
				}
				*/
				/*
					Byte
				reg[rt] = (memory[reg[rs] + dat1 + 0] << 24)
						 |(memory[reg[rs] + dat1 + 1] << 16)
						 |(memory[reg[rs] + dat1 + 2] << 8)
						 |(memory[reg[rs] + dat1 + 3] << 0);
				*/
				break;
			//10 1011 sw
			case 0x2B:
				SW(addr, reg[rt]);
				/*
				switch (offset){
					case 0 : 
						memory[row] = reg[rt];
						break;
					case 1 : 
						memory[row] ^= (memory[row] ^ (reg[rt] >> 8)) & 0xFFFFFF;
						memory[row + 1] ^= (((memory[row + 1] >> 24) ^ reg[rt]) & 0xFF) << 24;
						break;
					case 2 : 
						memory[row] ^= (memory[row] ^ (reg[rt] >> 16)) & 0xFFFF;
						memory[row + 1] ^= (((memory[row + 1] >> 16) ^ reg[rt]) & 0xFFFF) << 16;
						break;
					case 3 : 
						memory[row] ^= (memory[row] ^ (reg[rt] >> 24)) & 0xFF;
						memory[row + 1] ^= (((memory[row + 1] >> 8) ^ reg[rt]) & 0xFFFFFF) << 8;
						break;
				}
				*/
				/*
					Byte
				memory[reg[rs] + dat1 + 0] = ((reg[rt] >> 24) & 0xFF);
				memory[reg[rs] + dat1 + 1] = ((reg[rt] >> 16) & 0xFF);
				memory[reg[rs] + dat1 + 2] = ((reg[rt] >> 8)  & 0xFF);
				memory[reg[rs] + dat1 + 3] = ((reg[rt] >> 0)  & 0xFF);
				*/
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
				PC = PC & 0xF0000000 | (adr << 2);
				break;
				
			//00 0011 jal
			case 0x03:
				$ra = PC + 4; 	//$ra
				PC = PC & 0xF0000000 | (adr << 2);
				break;
			
			default :
				assert(0);
			//End of J-type	
		}
		if (step || (PC >> 2) == line) break;
	}
}
