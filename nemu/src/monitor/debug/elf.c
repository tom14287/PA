#include "common.h"
#include <stdlib.h>
#include <elf.h>
#include <monitor/exprelf.h>

char *exec_file = NULL;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;

void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

//	uint8_t buf[4096];
	/* Read the first 4096 bytes from the exec_file.
	 * They should contain the ELF header and program headers. */
//	ret = fread(buf, 4096, 1, fp);
	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret =fread(buf,sizeof(Elf32_Ehdr),1,fp);
	assert(ret == 1);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	Elf32_Shdr *sh = malloc(sh_size);
	fseek(fp, elf->e_shoff, SEEK_SET);
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
		}
		else if(sh[i].sh_type == SHT_STRTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
	}
//	printf("TEST:%s\n",buf);


//	printf("TEST:%d\n",symtab[2].st_size);
	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);

	fclose(fp);
}
unsigned int give_num(char *s){
	int i=0;
	int pos=0;
	for(i=0;i<nr_symtab_entry;i++){
	pos=symtab[i].st_name;
	if(strcmp(s,strtab+pos)==0){
	//	if((symtab[i].st_info&0x11)==1){
			return symtab[i].st_value;
	//		 }
	//	else {
	//		printf("Warning:This is not a data object!\n");
//			return symtab[i].st_value;
	//		}
		}
	}
	printf("Error variable name!\n");
	return 0;
}

void backtrace(unsigned int ebp,unsigned int eip){
	uint32_t work=ebp;
	int count=0;
	int i=0;
	bool flag=false;
	uint32_t ret=0;
	int j=0;
	uint32_t var;
	for(i=0;i<nr_symtab_entry;i++){
		if(work==0){
			break;
		}
		if(eip>=symtab[i].st_value&&eip<=(symtab[i].st_value+symtab[i].st_size)){
			printf("# current address:0x%.8X at %s\n",eip,strtab+symtab[i].st_name);
			flag=true;
			break;
		}
	}
	if(flag==false){
		printf("No stack.\n");
		return ;
	}
	for(j=1;j<=4;j++){
		var=swaddr_read(work+4+4*j,4,S_SS);
		printf("i%d : %u ",j,var);
	}
	printf("\n");
	while(work!=0){
		ret=swaddr_read(work+4,4,S_SS);
			for(i=0;i<nr_symtab_entry;i++){
				if(swaddr_read(work,4,S_SS)==0){
					break;
				};
			//	if((symtab[i].st_info&0x11)==2){
					if(ret>=symtab[i].st_value&&ret<=(symtab[i].st_value+symtab[i].st_size)){
						printf("# %d: %s\n",count++,strtab+symtab[i].st_name);
						break;
			}
			}
			work=swaddr_read(work,4,S_SS);
		}
	}


		


