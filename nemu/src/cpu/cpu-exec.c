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

#include <cpu/cpu.h>
#include <cpu/difftest.h>
#include <monitor/sdb/watchpoint.h>
#include <locale.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
// #define MAX_INST_TO_PRINT 10
// extern bool wtchpntWorking;

// #if  defined(CONFIG_ITRACE_COND) && defined(CONFIG_IRINGBUF)
// iring_buf_t iring_buf;
// #endif

CPU_state cpu = {};
extern uint64_t g_nr_guest_inst ;
// static uint64_t g_timer = 0; // unit: us
extern bool g_print_step;


void device_update();
void gen_trace(Decode *s);
void trace_and_difftest(Decode *_this, vaddr_t dnpc) ;
// static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
// #if  defined(CONFIG_ITRACE_COND) && defined(CONFIG_IRINGBUF)
//   strncpy(iring_buf.str_buf[iring_buf.hdr], _this->logbuf, ITRACE_MAX_LEN);
//   iring_buf.hdr = (iring_buf.hdr + 1) % CONFIG_IRINGBUFSIZE;
//   if(iring_buf.hdr == 0)
//     iring_buf.fulled = true;
// #elif  defined(CONFIG_ITRACE_COND)
//   log_write("%s\n", _this->logbuf); 
// #endif
// #ifdef CONFIG_WATCHPOINT
//   if(wtchpntWorking){
//     if(check_wtchpnt_chngd(_this)){
//       if(nemu_state.state == NEMU_RUNNING)
//         nemu_state.state = NEMU_STOP;
//     }
//   }
// #endif
  
//   if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
//   IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
// }

// //s->logbuf
// void gen_trace(Decode *s){
//   char *p = s->logbuf;
//   int len = strlen(s->logbuf);
//   p += len;
//   p += snprintf(p, sizeof(s->logbuf)-len, FMT_WORD ":", s->pc);
//   int ilen = s->snpc - s->pc;
//   int i;
//   uint8_t *inst = (uint8_t *)&s->isa.inst.val;
//   for (i = ilen - 1; i >= 0; i --) {
//     p += snprintf(p, 4, " %02x", inst[i]);
//   }
//   int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
//   int space_len = ilen_max - ilen;
//   if (space_len < 0) space_len = 0;
//   space_len = space_len * 3 + 1;
//   memset(p, ' ', space_len);
//   p += space_len;

// #ifndef CONFIG_ISA_loongarch32r
//   void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
//   disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
//       MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
// #else
//   p[0] = '\0'; // the upstream llvm does not support loongarch32r
// #endif
// }

static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s);
  cpu.pc = s->dnpc;
  // Assert(s->dnpc == 0x88fffffc, "s->pc: %x", s->pc);
#ifdef CONFIG_ITRACE

  gen_trace(s);
  // char *p = s->logbuf;
  // int len = strlen(s->logbuf);
  // p += len;
  // p += snprintf(p, sizeof(s->logbuf)-len, FMT_WORD ":", s->pc);
  // int ilen = s->snpc - s->pc;
  // int i;
  // uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  // for (i = ilen - 1; i >= 0; i --) {
  //   p += snprintf(p, 4, " %02x", inst[i]);
  // }
  // int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  // int space_len = ilen_max - ilen;
  // if (space_len < 0) space_len = 0;
  // space_len = space_len * 3 + 1;
  // memset(p, ' ', space_len);
  // p += space_len;
// #ifndef CONFIG_ISA_loongarch32r
//   void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
//   disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
//       MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
// #else
//   p[0] = '\0'; // the upstream llvm does not support loongarch32r
// #endif
#endif //CONFIG_ITRACE
}

void sim_cpu_execute(uint64_t n) {
  Decode s;
  for (;n > 0; n --) {
    exec_once(&s, cpu.pc);
    g_nr_guest_inst ++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }
}

// void log_statistic() {
//   IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
// #define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
//   Log("host time spent = " NUMBERIC_FMT " us", g_timer);
//   Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
//   if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
//   else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
// }

// void assert_fail_msg() {
//   isa_reg_display();
//   statistic();
// }

// /* Simulate how the CPU works. */
// void cpu_exec(uint64_t n) {
//   g_print_step = (n < MAX_INST_TO_PRINT);
//   switch (nemu_state.state) {
//     case NEMU_END: case NEMU_ABORT:
//       printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
//       return;
//     default: nemu_state.state = NEMU_RUNNING;
//   }

//   uint64_t timer_start = get_time();

//   execute(n);

//   uint64_t timer_end = get_time();
//   g_timer += timer_end - timer_start;

//   IFDEF(CONFIG_IRINGBUF, flush_iring_buf());
//   switch (nemu_state.state) {
//     case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

//     case NEMU_END: case NEMU_ABORT:
//       Log("nemu: %s at pc = " FMT_WORD,
//           (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
//            (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
//             ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
//           nemu_state.halt_pc);
//       // fall through
//     case NEMU_QUIT: statistic();
//   }
// }
