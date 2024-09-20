#include "npc_tb.hh"
#include <iostream>

uint32_t host_mem[1024];

const uint32_t addi_test_mem[] = {0x00140413,  0x00140413, 0x00140413, 0x00100073	};

uint32_t paddr2host_addr(int paddr){
  return(paddr - 0x80000000);
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
  for(int i = 0; i < (sizeof(addi_test_mem)/4); i++){
    top->pmm_reset[i] = addi_test_mem[i];
  }
  top->rst_n = 0;
  cpu_tick(top, 1);
  top->rst_n = 1;
}


int npc_tb(Vtop* top){
  reset(top);
  init_mem(top);

  cpu_tick(top, 5);

  return  0;
}