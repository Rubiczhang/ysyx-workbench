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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  TYPE_I, TYPE_U, TYPE_S, TYPE_B, TYPE_J, TYPE_R,
  TYPE_ISI,  //I type, shift immediate
  TYPE_FENCE,
  TYPE_N, // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | (BITS(i, 19, 12) << 12) \
                              | (BITS(i, 20, 20) << 11) | (BITS(i, 30, 21) << 1); } while(0)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | (BITS(i, 7, 7) << 11) \
                              | (BITS(i, 30, 25) << 5) | (BITS(i, 11, 8) << 1); } while(0)
#define immISI() do { *imm = BITS(i, 24, 20); } while(0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI();  break;
    case TYPE_U:                   immU();  break;
    case TYPE_S: src1R(); src2R(); immS();  break;
    case TYPE_J:                   immJ();  break;
    case TYPE_B: src1R(); src2R(); immB();  break;
    case TYPE_R: src1R(); src2R();          break;
    case TYPE_ISI: src1R();        immISI();break;
  }
}

void print_inst(vaddr_t thispc) {
  uint32_t temp[2];
  vaddr_t pc = thispc;
  temp[0] = inst_fetch(&pc, 4);
  temp[1] = inst_fetch(&pc, 4);

  uint8_t *p = (uint8_t *)temp;
  printf("invalid opcode(PC = " FMT_WORD "):\n"
      "\t%02x %02x %02x %02x %02x %02x %02x %02x ...\n"
      "\t%08x %08x...\n",
      thispc, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], temp[0], temp[1]);
}





static int decode_exec(Decode *s) {
  int rd = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;
#ifdef CONFIG_ITRACE_COND
  bool load_flag = false;
  bool store_flag = false;
  int store_len = 0;
#define SET_LS_FLAG(load, store, length) (load_flag = load, store_flag = store, store_len = length)
#else
#define SET_LS_FLAG(load, store, length) 
#endif


#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

#define TAKE_BRANCH s->dnpc = s->pc+imm;
#define DIV do{if(src1 == (1 < 31) && src2 == -1) R(rd) = src1; \
            else if(src2 == 0) R(rd) = -1; \
            else R(rd) = (sword_t) src1 / (sword_t) src2;} while(0)

#define REM do{if(src1 == (1 << 31) && src2 == -1) R(rd) = 0; \
            else if(src2 == 0) R(rd) = src1; \
            else R(rd) = (sword_t) src1 % (sword_t) src2;} while(0)


///////////// For debug
  // print_inst(s->pc);

////////////////////////
  INSTPAT_START();
  //check pattern untill match.
  //In each INSTPAT, firstly check wether match, than INSTPAT_MATCH is called
  //    execution is called at the second lxne of INSTPAT_MATCH
  //    'name' is not be used when running
  //    'type' is used in decode_operand
  INSTPAT("??????? ????? ????? 000 ????? 1100011" , beq                 , B, if(src1 == src2) TAKE_BRANCH );
  INSTPAT("??????? ????? ????? 001 ????? 1100011" , bne                 , B, if(src1 != src2) TAKE_BRANCH );
  INSTPAT("??????? ????? ????? 100 ????? 1100011" , blt                 , B, if((sword_t)src1 < (sword_t)src2)  TAKE_BRANCH );
  INSTPAT("??????? ????? ????? 101 ????? 1100011" , bge                 , B, if((sword_t)src1>= (sword_t)src2)  TAKE_BRANCH );
  INSTPAT("??????? ????? ????? 110 ????? 1100011" , bltu                , B, if(src1 < src2)  TAKE_BRANCH );
  INSTPAT("??????? ????? ????? 111 ????? 1100011" , bgeu                , B, if(src1 >= src2) TAKE_BRANCH );
  INSTPAT("??????? ????? ????? 000 ????? 1100111" , jalr                , I, R(rd) = s->snpc ;s->dnpc = imm + src1 );
  INSTPAT("??????? ????? ????? ??? ????? 1101111" , jal                 , J, R(rd) = s->snpc ;s->dnpc = imm + s->pc);
  INSTPAT("??????? ????? ????? ??? ????? 0110111" , lui                 , U, R(rd) = (imm) );
  INSTPAT("??????? ????? ????? ??? ????? 0010111" , auipc               , U, R(rd) = s->pc + imm );
  INSTPAT("??????? ????? ????? 000 ????? 0010011" , addi                , I, R(rd) = src1 + imm );
  INSTPAT("0000000 ????? ????? 001 ????? 0010011" , slli                , ISI, R(rd) = src1 << imm );
  INSTPAT("??????? ????? ????? 010 ????? 0010011" , slti                , I, R(rd) = ((sword_t)src1 < (sword_t)imm) ? 1 : 0);
  INSTPAT("??????? ????? ????? 011 ????? 0010011" , sltiu               , I, R(rd) = ((word_t)src1 < (word_t)imm) ? 1 : 0);
  INSTPAT("??????? ????? ????? 100 ????? 0010011" , xori                , I, R(rd) = src1 ^ imm );
  INSTPAT("0000000 ????? ????? 101 ????? 0010011" , srli                , ISI, R(rd) = (word_t) src1 >> imm );
  INSTPAT("0100000 ????? ????? 101 ????? 0010011" , srai                , ISI, R(rd) = (sword_t)src1 >> imm);
  INSTPAT("??????? ????? ????? 110 ????? 0010011" , ori                 , I, R(rd) = src1 | imm );
  INSTPAT("??????? ????? ????? 111 ????? 0010011" , andi                , I, R(rd) = src1 & imm );
  INSTPAT("0000000 ????? ????? 000 ????? 0110011" , add                 , R, R(rd) = src1 + src2 );
  INSTPAT("0100000 ????? ????? 000 ????? 0110011" , sub                 , R, R(rd) = src1 - src2 );
  INSTPAT("0000000 ????? ????? 001 ????? 0110011" , sll                 , R, R(rd) = src1 << src2 );
  INSTPAT("0000000 ????? ????? 010 ????? 0110011" , slt                 , R, R(rd) = ((sword_t)src1 < (sword_t)src2) ? 1 : 0);
  INSTPAT("0000000 ????? ????? 011 ????? 0110011" , sltu                , R, R(rd) = ((word_t)src1 < (word_t)src2) ? 1 : 0);
  INSTPAT("0000000 ????? ????? 100 ????? 0110011" , xor                 , R, R(rd) = src1 ^ src2 );
  INSTPAT("0000000 ????? ????? 101 ????? 0110011" , srl                 , R, R(rd) = (word_t) src1 >> src2 );
  INSTPAT("0100000 ????? ????? 101 ????? 0110011" , sra                 , R, R(rd) = (sword_t)src1 >> src2);
  INSTPAT("0000000 ????? ????? 110 ????? 0110011" , or                  , R, R(rd) = src1 | src2 );
  INSTPAT("0000000 ????? ????? 111 ????? 0110011" , and                 , R, R(rd) = src1 & src2 );
  INSTPAT("??????? ????? ????? 000 ????? 0000011" , lb                  , I, R(rd) = SEXT(Mr(src1 + imm, 1),8); SET_LS_FLAG(true, false, 1); );
  INSTPAT("??????? ????? ????? 001 ????? 0000011" , lh                  , I, R(rd) = SEXT(Mr(src1 + imm, 2), 16); SET_LS_FLAG(true, false, 2));
  INSTPAT("??????? ????? ????? 010 ????? 0000011" , lw                  , I, R(rd) = Mr(src1 + imm, 4); SET_LS_FLAG(true, false, 4); ) ;
  INSTPAT("??????? ????? ????? 100 ????? 0000011" , lbu                 , I, R(rd) = Mr(src1 + imm, 1); SET_LS_FLAG(true, false, 1); ) ;
  INSTPAT("??????? ????? ????? 101 ????? 0000011" , lhu                 , I, R(rd) = Mr(src1 + imm, 2); SET_LS_FLAG(true, false, 2); ) ;
  INSTPAT("??????? ????? ????? 000 ????? 0100011" , sb                  , S, Mw(src1 + imm, 1, src2);   SET_LS_FLAG(false, true, 1); );
  INSTPAT("??????? ????? ????? 001 ????? 0100011" , sh                  , S, Mw(src1 + imm, 2, src2) ;  SET_LS_FLAG(false, true, 2); );
  INSTPAT("??????? ????? ????? 010 ????? 0100011" , sw                  , S, Mw(src1 + imm, 4, src2) ;  SET_LS_FLAG(false, true, 4); );
  // INSTPAT("??????? ????? ????? ??? ???? 0001111" , fence               , FENCE, TODO() );
  INSTPAT("0000000 00000 00000 000 00000 1110011" , ecall,   N, TODO());
  INSTPAT("0000000 00001 00000 000 00000 1110011" , ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  // INSTPAT("0000000 ????? ????? 000 ????? 1110011" , uret                , R, TODO() );
  // INSTPAT("0001000 ????? ????? 000 ????? 1110011" , sret                , R, TODO() );
  // INSTPAT("0011000 ????? ????? 000 ????? 1110011" , mret                , R, TODO() );
  // INSTPAT("0111101 ????? ????? 000 ????? 1110011" , dret                , R, TODO() );
  // // INSTPAT("0001001 ????? ????? 000 ????? 1110011" , sfence.vma          , R, TODO() );
  // INSTPAT("0001000 ????? ????? 000 ????? 1110011" , wfi                 , R, TODO() );
  // // INSTPAT("1111110 ????? ????? 000 ????? 1110011" , cflush.d.l1         , R, TODO() );
  // // INSTPAT("1111110 ????? ????? 000 ????? 1110011" , cdiscard.d.l1       , R, TODO() );
  // // INSTPAT("1111110 ????? ????? 000 ????? 1110011" , cflush.i.l1         , R, TODO() );
  INSTPAT("0000001 ????? ????? 000 ????? 0110011" , mul                 , R, R(rd) = (sword_t)(src1 * src2) );
  INSTPAT("0000001 ????? ????? 001 ????? 0110011" , mulh                , R, R(rd) = (((int64_t)(sword_t)src1 * (int64_t) (sword_t)src2) >> 31) >> 1 );
  // INSTPAT("0000001 ????? ????? 001 ????? 0110011" , mulh                , R, R(rd) = (sword_t)((int64_t)src1 * (int64_t) src2) >> 32 );
  INSTPAT("0000001 ????? ????? 010 ????? 0110011" , mulhsu              , R, R(rd) = ((((int64_t) (sword_t)src1 * (uint64_t)src2) >> 31)>>1)) ;
  INSTPAT("0000001 ????? ????? 011 ????? 0110011" , mulhu               , R, R(rd) =  (word_t)(((uint64_t) src1 * (uint64_t)src2 >> 31)>>1));
  INSTPAT("0000001 ????? ????? 100 ????? 0110011" , div                 , R, DIV );
  INSTPAT("0000001 ????? ????? 101 ????? 0110011" , divu                , R, R(rd) = (src2 == 0)? ((word_t)-1): src1 / src2 );
  INSTPAT("0000001 ????? ????? 110 ????? 0110011" , rem                 , R, REM );
  INSTPAT("0000001 ????? ????? 111 ????? 0110011" , remu                , R, R(rd) = (src2 == 0)? src1 : src1 % src2);


  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

#ifdef CONFIG_ITRACE_COND
  char *p = s->logbuf;
  char *buff = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), "-----begin-------------\n");
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  p += snprintf(p, sizeof(s->logbuf)-(p-buff), "rs1: $%s: %08x, rs2: $%s: %08x, imm: %d\n", 
                reg_name(rs1), src1, reg_name(rs2), src2, imm);
  p += snprintf(p, sizeof(s->logbuf) - (p-buff), "s->dnpc: %08x\n",  s->dnpc);
  if(load_flag){
    p += snprintf(p, sizeof(s->logbuf) - (p-buff), "R(%s)<-Mem(0x%08x): 0x%08x\n", reg_name(rd), src1+imm, R(rd));
  } else if(store_flag){
    word_t mask = 0;
    switch(store_len){
      case 1:
        mask = (1 << 8) - 1;break;
      case 2:
        mask = (1 << 16) - 1; break;
      default:
        mask = -1;  break;
    }
    int rs2 = BITS(i, 24, 20);
    p += snprintf(p, sizeof(s->logbuf) - (p-buff), "Mem(0x%08x) <- R(%s): 0x%08x\n", src1+imm, reg_name(rs2), mask & src2);
  } else{
    p += snprintf(p, sizeof(s->logbuf) - (p-buff), "R(%s) <- 0x%08x\n",reg_name(rd), R(rd));
  }
  // log_write("%s", s->logbuf);
#endif

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
