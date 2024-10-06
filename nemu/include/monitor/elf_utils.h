#ifndef __ELF_UTILS_H__
#define __ELF_UTILS_H__
#include <elf.h>
#include <stdio.h>
#include <common.h>
#include <memory/paddr.h>


typedef struct {
  vaddr_t addr_lo;
  vaddr_t addr_hi;
  char* name;
} Addr2Sym;
extern Addr2Sym* addr2sym_tab;
extern char* strtab ;
extern char* shstrtab;
extern int elf_nr_func;

int read_elf_hdrs(FILE* fp, Elf32_Ehdr* hdr_p, Elf32_Phdr** elf_phdrs, Elf32_Shdr** elf_shdrs, char** shstrtab, char** strtab, Elf32_Sym** symtab, int* nr_phdr, int* nr_shdr);

int read_elf_symtab(FILE* fp,const Elf32_Ehdr* hdr_p, const Elf32_Shdr* elf_shdrs,  const char* strtab, const char* shstrtab, Elf32_Sym** symtab, int* nr_sym);

void build_addr2fname_tab(Elf32_Sym* symtab, char* strtab, int nr_symtab);

char* get_fname(Elf32_Addr v_addr, int nr_sym);

int load_segments(FILE* fp,const Elf32_Phdr* elf_phdrs, const Elf32_Shdr* elf_shdrs, int nr_phdr);

#endif //__ELF_UTILS_H__