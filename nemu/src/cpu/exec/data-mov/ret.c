#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
//#include "cpu/exec/template-start.h"
make_helper(ret){
	if(ops_decoded.opcode==0xc3){
		if(ops_decoded.is_data_size_16){
			//OPERAND_W(cpu.eip,cpu.esp&0xff);
		//	cpu.eip=cpu.esp&0xff;
		//
		   // swaddr_write(cpu.eip,2,cpu.esp&0xff);
			cpu.eip=swaddr_read(cpu.esp&0xff,2)-1;//unchecked
			cpu.esp += 2;
			cpu.eip = cpu.eip & 0xffff;
			print_asm("ret");
		}
		else{
		//	printf("here\n");
		//	swaddr_write(cpu.eip,4,cpu.esp);
		//	OPERAND_W(cpu.eip,cpu.esp);
		   // cpu.eip=cpu.esp;
			cpu.eip=swaddr_read(cpu.esp,4)-1;
			cpu.esp += 4;
			print_asm("ret");
		}
	}
	return 1;
}

//#include "cpu/exec/template-end.h"	
