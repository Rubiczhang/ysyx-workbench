#include <isa.h>
#include <memory/paddr.h>
#include <npc_tb.hh>
#include <iostream>
#include "Vtop__Syms.h"
#include <utils.h>

extern std::unique_ptr<Vtop> top;

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
  // printf("------------Point1\n");
  int x = n*2;
  while(x--){
    // printf("------------loop %d\n", x);
    cpu_tick_half(top);
  }
}

extern "C" void handle_ebreak(){
  std::cout << "EBREAK encountered!" <<std::endl;
  sim_state.state = SIM_END;
  // exit_routine();
  // exit(0);
}

static void reset(Vtop* top){
  // printf("--------------------Point2\n");
  top->rst_n = 0;
  top->clk = 0;
  // top->u_npc->u_ifu->instr_addr_icache_o =  0x80000000;
  // top->rootp->top__DOT__u_npc__DOT__u_ifu__DOT__pc = 0x80000000;
  cpu_tick(top, 5);
  top->rst_n = 1;
}


extern "C"{
void sim_restart() {
  // printf("***********hello\n");
  reset(top.get());
  // printf("xxxxxxxxxxxworld\n");
}

} //extern "C"
