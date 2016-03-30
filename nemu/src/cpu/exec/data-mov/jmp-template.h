#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute(){
	if(ops_decoded.opcode==0xff){
		//	if(DATA_BYTE==2){
		//		cpu.eip = cpu.eip&0x0000ffff;
		//	}
		//	else{
				cpu.eip=op_src->val;
		//	}
			if(op_src->type==OP_TYPE_REG){
				cpu.eip -= 2;
			}
			else{
				cpu.eip -= 7;
			}
		
	}
	if(ops_decoded.opcode==0xeb||ops_decoded.opcode==0xe9){
		if(DATA_BYTE==1){
			char t1=op_src->val&0x000000ff;
			cpu.eip += t1;
		}
		else if(DATA_BYTE==2){
			short t2=op_src->val&0x0000ffff;
			cpu.eip += t2;
		}
		else{
			cpu.eip += op_src->val;
		}
	}
/*	swaddr_t opcode=instr_fetch(cpu.eip,1);
	DATA_TYPE_S addr=instr_fetch(cpu.eip+1,DATA_BYTE);
	if(opcode==0xeb||opcode==0xe9){
		cpu.eip += addr;
		if(DATA_BYTE==2){
			cpu.eip = cpu.eip&0x0000ffff;
		}
	}
	else{
		cpu.eip=op_src->val;
		if(op_src->type==OP_TYPE_REG){
			cpu.eip -= 2;
		}
		else{
			cpu.eip -= 7;
		}
	}*/
	print_asm_template1();
}

make_instr_helper(i)
make_instr_helper(rm)


make_helper(concat(jmp_ptr_,SUFFIX)){
	hwaddr_t addr1=instr_fetch(cpu.eip+1,DATA_BYTE);
	//printf("0x%.8X\n",addr);
	hwaddr_t addr2=instr_fetch(cpu.eip+DATA_BYTE+1,S_ES);
	cpu.eip = addr1;
//	cpu.eip -= DATA+BYTE+3;
	cpu.CS.val=addr2&0x0000ffff;
	cpu.eip -= DATA_BYTE+3;
	if(DATA_BYTE==2){
		cpu.eip &= 0x0000ffff;
	}
	uint32_t addr3=cpu.CS.index;
	cpu.DES[0].limit15_0=lnaddr_read(cpu.gdtr.base+(addr3<<3),2);
	cpu.DES[0].base15_0=lnaddr_read(cpu.gdtr.base+(addr3<<3)+2,2);
	cpu.DES[0].base23_16=lnaddr_read(cpu.gdtr.base+(addr3<<3)+4,1);
	cpu.DES[0].limit19_16=lnaddr_read(cpu.gdtr.base+(addr3<<3)+6,1)&0xf;
	cpu.DES[0].base31_24=lnaddr_read(cpu.gdtr.base+(addr3<<3)+7,1);
	print_asm("ljmp $0x%x,0x%x",addr2,cpu.eip+7);
	return DATA_BYTE+3;
}
#include "cpu/exec/template-end.h"


