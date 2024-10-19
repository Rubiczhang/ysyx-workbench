#include <isa.h>
#include <sim_npc.h>
#include <micro_arch.h>
#include <Vtop__Dpi.h>
#include "svdpi.h"
#define NUM_REGS 32

extern "C"{
#define getsetScope(name){ \
  const svScope scope = svGetScopeFromName(name);\
  assert(scope); \
  svSetScope(scope);\
}

word_t getpc(){
  // top.get()->u_npc->u_ifu->
  return 0;
}

word_t getExepc(){
  // word_t pc = top.get()->u_npc->u_exeu->exeu_pc;
  const svScope scope = svGetScopeFromName("TOP.top.u_npc.u_exeu");
  assert(scope);
  svSetScope(scope);

  int pc = 0;
  top.get()->readExeuPc(&pc);
  return (word_t)pc;
}

word_t getCmtpc(){
  printf("Warning: getCmtpc is temperary using getExepc\n");
  return getExepc();
}

word_t getgpr(int idx){
  const svScope scope = svGetScopeFromName("TOP.top.u_npc.u_idu.u_regFile");
  assert(scope);
  svSetScope(scope);

  getsetScope("TOP.top.u_npc.u_idu.u_regFile");

  int ret_val = 0;
  top.get()->readGpr(idx, &ret_val);
  return ret_val ;
}

word_t getCmtSrc1(){
  getsetScope("TOP.top.u_npc.u_exeu");
  int ret_val = 0;
  top.get()->readExeuSrc1(&ret_val);
  return ret_val;
}

word_t getCmtSrc2(){
  getsetScope("TOP.top.u_npc.u_exeu");
  int ret_val = 0;
  top.get()->readExeuSrc2(&ret_val);
  return ret_val;
}


word_t getCmtIsStore(){
  getsetScope("TOP.top.u_npc.u_exeu");
  int ret_val = 0;
  top.get()->readExeuIsStore(&ret_val);
  return ret_val;
}


word_t getCmtIsLoad(){
  getsetScope("TOP.top.u_npc.u_exeu");
  int ret_val = 0;
  top.get()->readExeuIsLoad(&ret_val);
  return ret_val;
}

word_t getCmtStoreLen(){
  getsetScope("TOP.top.u_npc.u_exeu");
  int ret_val = 0;
  top.get()->readExeuStoreLen(&ret_val);
  return ret_val;
}

word_t getCmtImm(){
  getsetScope("TOP.top.u_npc.u_exeu");
  int ret_val = 0;
  top.get()->readExeuImm(&ret_val);
  return ret_val;
}

}

