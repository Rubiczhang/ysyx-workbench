#include "npc_tb.hh"
#include <iostream>
#include <unistd.h>
#include <getopt.h>

uint32_t host_mem[1024];

const uint32_t addi_test_mem[] = {0x00140413,  0x00140413, 0x00140413, 0x00100073	};
uint32_t init_img[MEM_SIZE] = {0x00140413,  0x00140413, 0x00140413, 0x00100073	};

uint32_t paddr2host_addr(int paddr){
  return(paddr - 0x80000000);
}


static char* img_file = NULL;
static char* workload_fmt = NULL;
static char* log_dir = NULL;



static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"log"             , required_argument, NULL, 'l'},
    {"help"            , no_argument      , NULL, 'h'},
    {"workloadfmt"     , required_argument, NULL, 'w'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-lhw:", table, NULL)) != -1) {
    switch (o) {
      case 'l': log_dir = optarg; break;
      case 'w': workload_fmt = optarg; 
            Assert(strcmp(workload_fmt, "bin") == 0 || strcmp(workload_fmt, "elf") == 0, 
                  "Wrong workload format:%s\n", workload_fmt);
            break;
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-w,--workloadfmt=FMT    workload format: bin or elf\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

static long load_bin(FILE* fp) {

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(init_img, size, 1, fp);
  // printf("init_img :%x, size: %d\n", init_img, size);
  assert(ret == 1);

  return size;
}


static long load_elf(FILE* fp){
  Assert(0, "Please Implement me\n");
  return 0;
}

static long load_img(){
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image. \n");
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


// void write_mem(Vtop *top, int index, int value) {
//   std::string signal_name = "TOP.top.pmem.mem[" + std::to_string(index) + "]";

//   vpiHandle vh = vpi_handle_by_name(signal_name.c_str(), NULL);
//   if (!vh) { std::cerr << "Error: No handle found for " << signal_name << std::endl; return; }

//   s_vpi_value v;
//   v.format = vpiIntVal;
//   v.value.integer = value;
//   vpi_put_value(vh, &v, NULL, vpiNoDelay);
// }

void cpu_tick_half(Vtop* top){
  top->clk = ~top->clk;
  top->eval();
#ifdef TRACE_ENABLE
  top->contextp()->timeInc(1);
  if(tfp)
    tfp->dump(top->contextp()->time());
#endif
}

void cpu_tick(Vtop *top, int n){
  int x = n*2;
  while(x--){
    cpu_tick_half(top);
  }
}

extern "C" void handle_ebreak(){
  std::cout << "EBREAK encountered!" <<std::endl;
  exit_routine();
  exit(0);
}

void reset(Vtop* top){
  top->rst_n = 0;
  top->clk = 0;
  cpu_tick(top, 5);
  top->rst_n = 1;
}


void init_mem(Vtop* top){
  uint32_t size = load_img();
  char* test_mem = (char*) init_img;
  for(int i = 0; i < size; i++){
    top->pmm_reset[i] = test_mem[i];
  }

  printf("file sizea %d\n", size);
  top->rst_n = 0;
  cpu_tick(top, 1);
  top->rst_n = 1;
  // printf("hello\n");
}


int npc_tb(Vtop* top, int argc, char* argv[]){
  parse_args(argc, argv);

  reset(top);

  init_mem(top);
  // printf("world\n");
  cpu_tick(top, 100);
  // printf("Bye\n");
  return  0;
}