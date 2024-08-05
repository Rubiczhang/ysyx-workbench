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
#include "local-include/reg.h"
#define NUM_REGS 32

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  for(int i = 0; i < NUM_REGS; i++){
    bool succ = false;
    
    word_t val = isa_reg_str2val(regs[i], &succ);
    Assert(succ, "Wrong Register Name: %s", regs[i]);
    printf("%-5s %#08x %'10u \n", regs[i], val, val);
  }
  
}

word_t isa_reg_str2val(const char *s, bool *success) {
  int idx = 0;
  for(; idx < 32; idx++){
    if(!strcmp(s, regs[idx])){ //same
      *success = true;
      break;
    }
  }
  return gpr(idx);    //no matter success or not 
}
