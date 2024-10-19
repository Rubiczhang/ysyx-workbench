// extern "C" {
#include <cpu/cpu.h>
#include <monitor/sdb/watchpoint.h>
#include <locale.h>
#include <micro_arch.h>
#include <memory/paddr.h>
// }

#include <sim_npc.h>
extern "C" void trace_and_difftest(Decode *_this, vaddr_t dnpc) ;
extern void cpu_tick_half(Vtop* top);
extern void cpu_tick(Vtop *top, int n);
extern "C" void gen_trace(Decode *s);

void exec_once(Decode *s){
  if(top->clk == 0){
    cpu_tick_half(top.get());
  }
  cpu_tick(top.get(), 1);     //1->0->1
  // temporary choice for wbpc();

  s->pc           = getCmtpc();
  s->snpc           = s->pc + 4;
  s->isa.inst.val = paddr_read(s->pc, 4);

#ifdef CONFIG_ITRACE
  // printf("Point 0\n");
  gen_trace(s);
  // printf("Point 1\n");
#endif
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

}
