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
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>

typedef struct diff_context {
  word_t gpr[MUXDEF(CONFIG_RVE, 16, 32)];
  word_t pc;
} diff_context_t;

#ifdef CONFIG_RVE
#define NR_REGS 16
#else
#define NR_REGS 32
#endif


__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  // printf("getpc():" FMT_WORD " cpu.pc: " FMT_WORD "\n", getpc(), cpu.pc);
  size_t i = 0;
  for( ; i + 4 < n; i+=4){
    if(direction == DIFFTEST_TO_REF) {
      paddr_write(addr+i, 4, *(uint32_t*)(buf+i));
    } else{
      *(uint32_t*) (buf+i) = paddr_read(addr+i, 4);
    }
  }
  for( ; i < n; i++){
    if(direction == DIFFTEST_TO_REF) {
      paddr_write(addr+i, 1, *(uint32_t*)(buf+i));
    } else{
      *(char*) (buf+i) = paddr_read(addr+i, 4);
    }
  }
  
  // printf("-----Point 2: getpc():" FMT_WORD " cpu.pc: " FMT_WORD "\n", getpc(), cpu.pc);
}

__EXPORT void difftest_regcpy(void *dut, bool direction) {

  // printf("-----Point 3: getpc():" FMT_WORD " cpu.pc: " FMT_WORD "\n", getpc(), cpu.pc);
  diff_context_t* dut_ptr = (diff_context_t*) dut;
  if(direction == DIFFTEST_TO_REF) {
    for(int i = 0; i < NR_REGS; i++){
      setgpr(i, dut_ptr->gpr[i]);
      // printf("dut_ptr->gpr[%d] " FMT_WORD "->" "gpr[%d]: " FMT_WORD "\n",i, dut_ptr->gpr[i],  i, getgpr(i) );
    }
  } else {
    for(int i = 0; i < NR_REGS; i++){
      dut_ptr->gpr[i] = getgpr(i);
      // printf("dut_ptr->gpr[%d] " FMT_WORD "<- gpr[%d]: " FMT_WORD "\n",i, dut_ptr->gpr[i],  i, getgpr(i) );
    }
  }
  dut_ptr->pc = getpc();

  // printf("-----" CONFIG_UPPER_SIM " Point 4: getpc():" FMT_WORD " cpu.pc: "  FMT_WORD "\n", getpc(), cpu.pc);
}

__EXPORT void difftest_exec(uint64_t n) {
  cpu_exec(n);
}

__EXPORT void difftest_raise_intr(word_t NO) {
  assert(0);
}

__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();
  /* Perform ISA dependent initialization. */
  init_isa();
}
