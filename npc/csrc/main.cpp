// #include <stdio.h>
#include <nvboard.h>
#include <verilated.h>
#include <string>

#define STRINGFY(x) #x
#define TO_STRING(x) STRINGFY(x)

#ifdef TRACE_ENABLE
#include "verilated_fst_c.h"
VerilatedFstC* tfp = nullptr;
#endif

#include "Vtop.h"
#include <iostream>

void nvboard_bind_all_pins(Vtop *top);
static Vtop dut;



void reset(Vtop* top, int n){
  top->rst = 1;
  // while(n--) single_cycle(top);
  top->rst = 0;
}

int main(int argc, char** argv) {
  nvboard_bind_all_pins(&dut);
  nvboard_init();
  
  const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
  contextp->commandArgs(argc, argv);
  const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};

#ifdef TRACE_ENABLE

  const char* flag = contextp->commandArgsPlusMatch("trace");
  const char* traceDir = TO_STRING(TRACE_DIR) ;
  if(flag && (std::strcmp(flag, "+trace") == 0)){
    contextp->traceEverOn(true);
    VL_PRINTF("Enabling waves into %s/waves.fst\n", traceDir);
    tfp = new VerilatedFstC;
    top->trace(tfp, 99);
    Verilated::mkdir(traceDir);
    std::string trace_file = std::string(traceDir) + std::string("/wave.fst");
    tfp->open(trace_file.c_str());
  }

#endif  //TRACE_ENABLE

  // auto top = std::make_unique<Vtop>(contextp.get(), "TOP");
  // reset(top.get(), 10);
  while(1){
    contextp->timeInc(1);
    top->clk = ~top->clk & 0x1;
    top->eval();
    nvboard_update();
#ifdef TRACE_ENABLE
    if(tfp)
      tfp->dump(contextp->time());
#endif
  }

#ifdef TRACE_ENABLE
    if (tfp) {
        tfp->close();
        delete(tfp);
    }
#endif
  nvboard_quit();

  top->final();
  return 0;
}
