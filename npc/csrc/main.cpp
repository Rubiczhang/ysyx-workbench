#include <stdio.h>
#include <nvboard.h>
#include <verilated.h>


#include "Vtop.h"

void nvboard_bind_all_pins(Vtop *top);
static Vtop dut;



void single_cycle(Vtop* top){
  top->clk = 0; 
  top->eval();
  top->clk = 1; 
  top->eval();
}

void reset(Vtop* top, int n){
  top->rst = 1;
  while(n--) single_cycle(top);
  top->rst = 0;
}

int main(int argc, char** argv) {
  nvboard_bind_all_pins(&dut);
  nvboard_init();
  
  const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
  contextp->traceEverOn(true);
  contextp->commandArgs(argc, argv);

  // auto top = std::make_unique<Vtop>(contextp.get(), "TOP");
  const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};
  reset(top.get(), 10);
  int N = 1000000;
  while(N--){
    contextp->timeInc(1);
    single_cycle(top.get());
    nvboard_update();
  }
  nvboard_quit();
  return 0;
}
