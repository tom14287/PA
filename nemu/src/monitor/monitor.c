#include "nemu.h"
#include "common.h"
#include <memory/cache.h>

#define ENTRY_START 0x100000

extern uint8_t entry [];
extern uint32_t entry_len;
extern char *exec_file;

void load_elf_tables(int, char *[]);
void init_regex();
void init_wp_list();
void init_ddr3();
void init_device();
void init_sdl();

FILE *log_fp = NULL;

static void init_log() {
	log_fp = fopen("log.txt", "w");
	Assert(log_fp, "Can not open 'log.txt'");
}

static void welcome() {
	printf("Welcome to NEMU!\nThe executable is %s.\nFor help, type \"help\"\n",
			exec_file);
}

void init_monitor(int argc, char *argv[]) {
	/* Perform some global initialization */

	/* Open the log file. */
	init_log();

	/* Load the string table and symbol table from the ELF file for future use. */
	load_elf_tables(argc, argv);

	/* Compile the regular expressions. */
	init_regex();

	/* Initialize the watchpoint link list. */
	init_wp_list();

	/* Display welcome message. */
	welcome();

	init_device();//init device
	init_sdl();//init sdl
}

#ifdef USE_RAMDISK
static void init_ramdisk() {
	int ret;
	const int ramdisk_max_size = 0xa0000;
	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	Assert(file_size < ramdisk_max_size, "file size(%zd) too large", file_size);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(0), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}
#endif

static void load_entry() {
	int ret;
	FILE *fp = fopen("entry", "rb");
	Assert(fp, "Can not open 'entry'");

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(ENTRY_START), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}
void init_cr0(){//shi dizhi moshi
	cpu.cr0.protect_enable=0;
	cpu.cr0.monitor_coprocessor=0;
	cpu.cr0.emulation=0;
	cpu.cr0.task_switched=0;
	cpu.cr0.extension_type=0;
	cpu.cr0.numeric_error=0;
	cpu.cr0.pad0=0;
	cpu.cr0.write_protect=0;
	cpu.cr0.pad1=0;
	cpu.cr0.alignment_mask=0;
	cpu.cr0.pad2=0;
	cpu.cr0.no_write_through=0;
	cpu.cr0.cache_disable=0;
	cpu.cr0.paging=0;
}

void restart() {
	/* Perform some initialization to restart a program */
#ifdef USE_RAMDISK
	/* Read the file with name `argv[1]' into ramdisk. */
	init_ramdisk();
#endif

	/* Read the entry code into memory. */
	load_entry();

	/* Set the initial instruction pointer. */
	cpu.eip = ENTRY_START;

	cpu.EFLAGS=0x00000002;//init eflags
	init_cache_L1();//init cache
	init_cache_L2();
	init_cr0();//init cr0
	cpu.DES[1].base15_0=0;
	cpu.DES[1].base23_16=0;
	cpu.DES[1].base31_24=0;
	cpu.DES[1].limit15_0=0xffff;
	cpu.DES[1].limit19_16=0xf;
//	init_page_L1();
	init_TLB();
  //  printf("CF:%u     tmp_1:%u    PF:%u     \n",cpu.CF,cpu.tmp_1,cpu.PF);
	/* Initialize DRAM. */
	init_ddr3();
}
