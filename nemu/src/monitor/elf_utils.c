#include <monitor/elf_utils.h>
#define MAX_SHNAME_LEN 256

Addr2Sym* addr2sym_tab;

static int elf_read_eident(FILE* fp, unsigned char* e_ident_buf){
//return 32 if ELF32, 64 if ELF64, 0 if Invalid
  int ret;
  ret = fread(e_ident_buf, sizeof(unsigned char), EI_NIDENT, fp );
  Assert(ret == EI_NIDENT, "Can't read elf eident");
  bool elf_is_valid = (e_ident_buf[EI_MAG0] == ELFMAG0) &&
                      (e_ident_buf[EI_MAG1] == ELFMAG1) &&
                      (e_ident_buf[EI_MAG2] == ELFMAG2) &&
                      (e_ident_buf[EI_MAG3] == ELFMAG3);
  Assert(elf_is_valid, "wrong fmt of elf file\n");
  int res = 0;
  if(e_ident_buf[EI_CLASS] == ELFCLASS32)
    res = 32;
  else if(e_ident_buf[EI_CLASS] == ELFCLASS64)
    res = 64;
  
  return res;
}

static int elf_read_header32(FILE*fp, Elf32_Ehdr* ehdr){
  int ret;
  ret = fread(ehdr, sizeof(Elf32_Ehdr), 1, fp );
  Assert(ehdr->e_type == ET_EXEC && ret > 0, "Input elf is not executable file");
  return ret;
}


static int elf_read_phdr(FILE* fp, Elf32_Ehdr* hdr_p, Elf32_Phdr** elf_phdrs_p, int* nr_phdr){
  Elf32_Half phentsize = hdr_p->e_phentsize;
  Assert(hdr_p &&  phentsize == sizeof(Elf32_Phdr), "Wrong phdr size format\n");

  Elf32_Half phnum = hdr_p->e_phnum;
  *nr_phdr = phnum;
  *elf_phdrs_p = malloc(hdr_p->e_phentsize  * phnum);

  Elf32_Off phoff = hdr_p->e_phoff;
  Assert(phoff > 0, "Cannot read phdr succesfully");
  fseek(fp, phoff, SEEK_SET);
  int ret = fread(*elf_phdrs_p, phentsize, phnum, fp);
  Assert(ret > 0, "Cannot read phdr succesfully");
  return ret;
}

static int elf_read_shdr(FILE* fp, Elf32_Ehdr* hdr_p, Elf32_Shdr** elf_shdrs_p, int* nr_shdr){
  Elf32_Half shentsize = hdr_p->e_shentsize;
  Assert(hdr_p && shentsize == sizeof(Elf32_Shdr), "Wrong shdr size\n");

  Elf32_Half shnum = hdr_p->e_shnum;
  *nr_shdr = shnum;
  *elf_shdrs_p = malloc(hdr_p->e_shentsize * shnum);

  Elf32_Off shoff = hdr_p->e_shoff;
  Assert(shoff > 0, "Cannot read phdr succesfully");
  fseek(fp, shoff, SEEK_SET);
  int ret = fread(*elf_shdrs_p, shentsize, shnum, fp);
  Assert(ret > 0, "Cannot read shdr succesfully\n");
  return ret;
}

static int elf_read_shstrtab(FILE* fp, Elf32_Ehdr* hdr_p, const Elf32_Shdr* elf_shdrs, char** shstrtab){
  Elf32_Half e_shstrndx = hdr_p->e_shstrndx;
  // int i = 0;
  // for(; i < hdr_p->e_shnum; i++){
  //   if( elf_shdrs[i].sh_offset == e_shstrndx)
  //     break;
  // }
  Elf32_Word shstrsize = elf_shdrs[e_shstrndx].sh_size;
  *shstrtab = malloc(shstrsize);
  fseek(fp, elf_shdrs[e_shstrndx].sh_offset, SEEK_SET);
  int ret = fread(*shstrtab, sizeof(char), shstrsize, fp);
  return ret;
}

static int elf_get_section_ndx(const Elf32_Ehdr* hdr_p, const Elf32_Shdr* elf_shdrs, const char* shstrtab, const char* shname, const uint32_t sh_type){
  int i = 0;
  for(; i < hdr_p->e_shnum; i++){
    if(elf_shdrs[i].sh_type != sh_type)
      continue;
    uint32_t sh_name_offset = elf_shdrs[i].sh_name;
    if(elf_shdrs[i].sh_name != 0){
      if(strncmp(shstrtab+sh_name_offset, shname, MAX_SHNAME_LEN) == 0)
        break;
    }
  }
  Assert(i != hdr_p->e_shnum, "Elf file has no %s section\n", shname);
  return i;
}

static int elf_read_strtab(FILE* fp, Elf32_Ehdr* hdr_p, const char* shstrtab, const Elf32_Shdr* elf_shdrs, char** strtab){
  int i = elf_get_section_ndx(hdr_p, elf_shdrs, shstrtab, ".strtab", SHT_STRTAB);

  Elf32_Word strsize = elf_shdrs[i].sh_size;
  *strtab = malloc(strsize);
  fseek(fp, elf_shdrs[i].sh_offset, SEEK_SET);
  int ret = fread(*strtab, sizeof(char), strsize, fp);
  return ret;
}

int read_elf_hdrs(FILE* fp, Elf32_Ehdr* hdr_p, Elf32_Phdr** elf_phdrs, Elf32_Shdr** elf_shdrs, char** shstrtab, char** strtab, Elf32_Sym** symtab, int* nr_phdr, int* nr_shdr){

  
  unsigned char e_ident_buf[EI_NIDENT];
  //读e_ident[EI_CLASS]
  int elf_class = elf_read_eident(fp, e_ident_buf);
  Assert(elf_class == 32, "NEMU only support ELF32");

  fseek(fp, 0, SEEK_SET);
  int ret = 0;
  ret += elf_read_header32(fp, hdr_p);
  
  ret += elf_read_phdr(fp, hdr_p, elf_phdrs, nr_phdr);

  ret += elf_read_shdr(fp, hdr_p, elf_shdrs, nr_shdr);
  ret += elf_read_shstrtab(fp, hdr_p,  *elf_shdrs, shstrtab);
  ret += elf_read_strtab(fp, hdr_p, *shstrtab, *elf_shdrs,  strtab);
  return ret;
}

int read_elf_symtab(FILE* fp,const Elf32_Ehdr* hdr_p, const Elf32_Shdr* elf_shdrs,  const char* strtab, const char* shstrtab, Elf32_Sym** symtab, int32_t *nr_sym){
  int i = elf_get_section_ndx(hdr_p, elf_shdrs, shstrtab, ".symtab", SHT_SYMTAB);
  
  Elf32_Word size = elf_shdrs[i].sh_size;
  Elf32_Word entsize = elf_shdrs[i].sh_entsize;
  *nr_sym = size/ entsize;

  *symtab = malloc(size);
  fseek(fp, elf_shdrs[i].sh_offset, SEEK_SET);
  int ret = fread(*symtab, entsize, size/entsize, fp);
  return ret;
}


void build_addr2sym_tab(Elf32_Sym* symtab, char* strtab, int nr_symtab){
  addr2sym_tab = malloc(sizeof(Addr2Sym)*nr_symtab);
  for(int i = 0; i < nr_symtab; i++){
    addr2sym_tab[i].addr = symtab[i].st_value;
    addr2sym_tab[i].name= symtab[i].st_name + strtab;
  }
}

char* get_sym_name(Elf32_Addr v_addr, int nr_sym){
  for(int i = 0; i < nr_sym; i++){
    if(v_addr == addr2sym_tab[i].addr){
      return addr2sym_tab[i].name;
    }
  }
  return NULL;
}


int load_segments(FILE* fp,const Elf32_Phdr* elf_phdrs, const Elf32_Shdr* elf_shdrs, int nr_phdr){
//ret is about file size, not memory size
  int ret = 0;
  for(int i = 0; i < nr_phdr; i++){
    // printf("Hello\n");
    uint32_t memsz = elf_phdrs[i].p_memsz;
    uint32_t filesz = elf_phdrs[i].p_filesz;
    uint32_t vaddr = elf_phdrs[i].p_vaddr;
    uint32_t off = elf_phdrs[i].p_offset;
    printf("memsz: %x, filesz: %x, vaddr: %x\n", memsz, filesz, vaddr);
    fseek(fp, off, SEEK_SET);
    ret += fread(guest_to_host(vaddr), filesz < memsz ? filesz: memsz, 1, fp);
    if(memsz > filesz){
      memset(guest_to_host(vaddr+filesz), 0, memsz-filesz);
    }
  }
  return ret;
}