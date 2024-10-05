/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>
#include <monitor/elf_utils.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  // Log("Exercise: Please remove me in the source code and compile NEMU again.");
  // assert(1);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_dir = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static char *workload_fmt = NULL;
static int difftest_port = 1234;
// static void* fptr;

static long load_bin(FILE* fp) {

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  return size;
}



static long load_elf(FILE* fp){
  Elf32_Ehdr hdr;
  Elf32_Phdr* elf_phdrs = NULL;
  Elf32_Shdr* elf_shdrs = NULL;
  Elf32_Sym* elf_symtab = NULL;
  int nr_sym, nr_phdr, nr_shdr;
  char* strtab = NULL;
  char* shstrtab = NULL;
  int ret;
  //load to host memory
  ret = read_elf_hdrs(fp, &hdr, &elf_phdrs, &elf_shdrs, &shstrtab, &strtab, &elf_symtab, &nr_phdr, &nr_shdr);
  ret += read_elf_symtab(fp, &hdr, elf_shdrs,  strtab, shstrtab, &elf_symtab, &nr_sym);

  build_addr2sym_tab(elf_symtab, strtab, nr_sym);

  load_segments(fp, elf_phdrs, elf_shdrs, nr_phdr);
  return ret;
}

static long load_img(){
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }
  
  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  int ret = 0;
  if(workload_fmt && strcmp(workload_fmt, "elf") == 0){
    ret = load_elf(fp);
  }
  else        //default is "bin" format    return load_bin();
    ret = load_bin(fp);
  
  fclose(fp);
  return ret;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"           , no_argument      , NULL, 'b'},
    {"log"             , required_argument, NULL, 'l'},
    {"diff"            , required_argument, NULL, 'd'},
    {"port"            , required_argument, NULL, 'p'},
    {"help"            , no_argument      , NULL, 'h'},
    {"workloadfmt"     , required_argument, NULL, 'w'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:w:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_dir = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 'w': workload_fmt = optarg; 
            Assert(strcmp(workload_fmt, "bin") == 0 || strcmp(workload_fmt, "elf") == 0, 
                  "Wrong workload format:%s\n", workload_fmt);
            break;
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\t-w,--workloadfmt=FMT    workload format: bin or elf\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_dir);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

#ifndef CONFIG_ISA_loongarch32r
  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv,
      MUXDEF(CONFIG_RV64,      "riscv64",
                               "riscv32"),
                               "bad"))) "-pc-linux-gnu"
  ));
#endif

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
