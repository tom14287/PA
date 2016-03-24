#include "cpu/exec/template-start.h"

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
make_instr_helper(rm)
//make_instr_helper(cr2r)
//make_instr_helper(r2cr)

make_helper(concat(mov_cr2r_,SUFFIX)){
	uint32_t judge=instr_fetch(cpu.eip+2,1);
//	printf("0x%.2X\n",judge);
	if(judge==0xc0){//cr0
		REG(R_EAX)=cpu.cr0.val;
		print_asm("mov cr0,%%%s",REG_NAME(op_dest->reg));

	}
	else if(judge==0xd8){//cr3
		REG(R_EAX)=cpu.cr3.val;
		print_asm("mov cr3,%%%s",REG_NAME(op_dest->reg));
	}
	return 2;

}
make_helper(concat(mov_r2cr_,SUFFIX)){
	uint32_t judge=instr_fetch(cpu.eip+2,1);
	if(judge==0xc0){//cr0
		cpu.cr0.val=REG(R_EAX);
		print_asm("mov %%%s,cr0",REG_NAME(op_src->reg));
	}
	else if(judge==0xd8){//cr3
		cpu.cr3.val=REG(R_EAX);
		print_asm("mov %%%s,cr3",REG_NAME(op_src->reg));
	}
	return 2;
}
make_helper(concat(mov_r2seg_,SUFFIX)){//just read limit and base
	uint32_t judge=instr_fetch(cpu.eip+1,1);
//	printf("0x%.2X\n",judge);
	if(judge==0xd8){//ds
		cpu.DS.val=REG(op_src->reg);
		uint32_t addr1=cpu.DS.index;
		cpu.DS_DES.limit15_0=lnaddr_read(cpu.gdtr.base+(addr1<<3),2);//16
		cpu.DS_DES.base15_0=lnaddr_read(cpu.gdtr.base+(addr1<<3)+2,2);//16
		cpu.DS_DES.base23_16=lnaddr_read(cpu.gdtr.base+(addr1<<3)+4,1);//8
		cpu.DS_DES.limit19_16=lnaddr_read(cpu.gdtr.base+(addr1<<3)+6,1)&0xf;//4
		cpu.DS_DES.base31_24=lnaddr_read(cpu.gdtr.base+(addr1<<3)+7,1);//8;
		print_asm("mov %%%s,%%ds",REG_NAME(op_src->reg));

	}
	else if(judge==0xc0){//es
		cpu.ES.val=REG(op_src->reg);
		uint32_t addr2=cpu.ES.index;
		cpu.ES_DES.limit15_0=lnaddr_read(cpu.gdtr.base+(addr2<<3),2);
		cpu.ES_DES.base15_0=lnaddr_read(cpu.gdtr.base+(addr2<<3)+2,2);
		cpu.ES_DES.base23_16=lnaddr_read(cpu.gdtr.base+(addr2<<3)+4,1);
		cpu.ES_DES.limit19_16=lnaddr_read(cpu.gdtr.base+(addr2<<3)+6,1)&0xf;
		cpu.ES_DES.base31_24=lnaddr_read(cpu.gdtr.base+(addr2<<3)+7,1);
		print_asm("mov %%%s,%%es",REG_NAME(op_src->reg));

	}
	else if(judge==0xd0){//ss
		cpu.SS.val=REG(op_src->reg);
		uint32_t addr3=cpu.SS.index;
		cpu.SS_DES.limit15_0=lnaddr_read(cpu.gdtr.base+(addr3<<3),2);
		cpu.SS_DES.base15_0=lnaddr_read(cpu.gdtr.base+(addr3<<3)+2,2);
		cpu.SS_DES.base23_16=lnaddr_read(cpu.gdtr.base+(addr3<<3)+4,1);
		cpu.SS_DES.limit19_16=lnaddr_read(cpu.gdtr.base+(addr3<<3)+6,1)&0xf;
		cpu.SS_DES.base31_24=lnaddr_read(cpu.gdtr.base+(addr3<<3)+7,1);
		print_asm("mov %%%s,%%ss",REG_NAME(op_src->reg));

	}
	else{//cs
		cpu.CS.val=REG(op_src->reg);
		uint32_t addr4=cpu.CS.index;
		cpu.CS_DES.limit15_0=lnaddr_read(cpu.gdtr.base+(addr4<<3),2);
		cpu.CS_DES.base15_0=lnaddr_read(cpu.gdtr.base+(addr4<<3)+2,2);
		cpu.CS_DES.base23_16=lnaddr_read(cpu.gdtr.base+(addr4<<3)+4,1);
		cpu.CS_DES.limit19_16=lnaddr_read(cpu.gdtr.base+(addr4<<3)+6,1)&0xf;
		cpu.CS_DES.base31_24=lnaddr_read(cpu.gdtr.base+(addr4<<3)+7,1);
		print_asm("mov %%%s,%%cs",REG_NAME(op_src->reg));

	}
	return 2;

}
make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX));

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#include "cpu/exec/template-end.h"
