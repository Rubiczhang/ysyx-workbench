// extern "C" {
#include <cpu/cpu.h>
#include <monitor/sdb/watchpoint.h>
#include <locale.h>
#include <micro_arch.h>
#include <memory/paddr.h>
#include <isa/riscv32/reg.h>

// }

#include <sim_npc.h>
extern "C" void trace_and_difftest(Decode *_this, vaddr_t dnpc) ;
extern void cpu_tick_half(Vtop* top);
extern void cpu_tick(Vtop *top, int n);
extern "C" void gen_trace(Decode *s);

void exec_once(Decode *s){
  // printf("init pc: 0x%x\n", getpc());
  assert(top->clk == 1);
  // if(top->clk == 0){
  //   cpu_tick_half(top.get());
  // }
  // printf("rstn: %d\n", top->rst_n);
  // printf("clk1: %d\n", top->clk);
  // cpu_tick(top.get(), 1);     //1->0->1
  cpu_tick_half(top.get());      //1->0, eval but not update regs

  // printf("clk2: %d\n", top->clk);
  // temporary choice for wbpc();

  s->pc           = getCmtpc();
  s->snpc           = s->pc + 4;
  s->isa.inst.val = paddr_read(s->pc, 4);

#ifdef CONFIG_ITRACE
  // printf("Point 0\n");
  inst_dtl_trace();
  gen_trace(s);
  // printf("Point 1\n");
#endif
  cpu_tick_half(top.get());     //0->1 
}

extern "C"{

void sim_cpu_execute(uint64_t n){
  Decode s;
  s.logbuf[0] = '\0';
  for (;n > 0; n --) {
    // printf("loop %d\n", n);
    exec_once(&s);
    // g_nr_guest_inst ++;
    trace_and_difftest(&s, getpc());
    if (sim_state.state != SIM_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }
}

// CPU_state* get_ref_isa_cpu_st(){
//   for(int i = 0; i < MUXDEF(CONFIG_RVE, 16, 32); i++)
// }

}
