#include "irq.h"

#include <sys/syscall.h>
#include <unistd.h>

void add_irq_handle(int, void (*)(void));
void mm_brk(uint32_t);

int fs_open(const char*,int);
int fs_read(int,void*,int);
int fs_write(int,void*,int);
int fs_lseek(int,int,int);
int fs_close(int);
static void sys_brk(TrapFrame *tf) {
#ifdef IA32_PAGE
	mm_brk(tf->ebx);
#endif
	tf->eax = 0;
}
extern void serial_printc(char);
int sys_write(int fd,void *buf,int len){
	if(fd==1||fd==2){
	//	asm volatile (".byte 0xd6" : : "a"(2),"c"(buf),"d"(len));
		int i;
		for(i=0;i<len;i++){
			serial_printc(((char*)buf)[i]);
		}
	}
//	asm volatile("int3");
	return len;
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The ``add_irq_handle'' system call is artificial. We use it to 
		 * let user program register its interrupt handlers. But this is 
		 * very dangerous in a real operating system. Therefore such a 
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;
		case SYS_open: tf->eax=fs_open((char *)tf->ebx,tf->ecx); break; 
		case SYS_write: tf->eax=sys_write(tf->ebx,(void *)tf->ecx,tf->edx);break;

		/* TODO: Add more system calls. */

		default: panic("Unhandled system call: id = %d", tf->eax);
	}
}

