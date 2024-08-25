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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#ifndef TEST_LOOP
#define TEST_LOOP 100
#endif

// this should be enough
static char output_buf[65536] = {};
static char cal_buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `output_buf`
static int out_buf_len = 0;
static int cal_buf_len = 0;
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


static const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
static const int nr_regs = sizeof(regs)/sizeof(regs[0]);
static const char* const bin_oprator[] = {
  "+", "-", "*", "/", "==", "!=", "&&"
};

static const char* const sig_oprator[] = {
  "-"
};
static int32_t  nr_bin_oprator = sizeof(bin_oprator)/sizeof(char*);
static int32_t  nr_sig_oprator = sizeof(sig_oprator)/sizeof(char*);

void gen_num(){
  int32_t r = rand();
  sprintf(output_buf+out_buf_len, "%u", r);
  sprintf(cal_buf+cal_buf_len, "%uu", r);
  out_buf_len = strlen(output_buf);
  cal_buf_len = strlen(cal_buf);
}

void gen_hex_num(){
  int32_t r = rand();
  sprintf(output_buf+out_buf_len, "0x%x", r);
  sprintf(cal_buf+cal_buf_len, "0x%xu", r);
  out_buf_len = strlen(output_buf);
  cal_buf_len = strlen(cal_buf);
}

void gen_str(const char* str){
  sprintf(output_buf+out_buf_len, "%s", str);
  out_buf_len+= strlen(str);
  sprintf(cal_buf+cal_buf_len, "%s", str);
  cal_buf_len+= strlen(str);
}

void gen_bin_oprator(){
  int32_t r = rand();
  r = r % nr_bin_oprator;
  gen_str(bin_oprator[r]);
  gen_str(" ");
}

void gen_sig_oprator(){
  int32_t r = rand();
  r = r % nr_sig_oprator;
  gen_str(sig_oprator[r]);
  gen_str(" ");
}

void gen_reg(){
  int32_t r = rand()%nr_regs;
  sprintf(output_buf+out_buf_len, "$%s", regs[r]);
  sprintf(cal_buf+cal_buf_len, "0");
  out_buf_len = strlen(output_buf);
  cal_buf_len = strlen(cal_buf);
}

static void gen_rand_space(){
  char buff[10];
  int n = rand()% 4;
  buff[n] = '\0';
  while(--n >= 0){
    buff[n] = ' ';
  }
  gen_str(buff);
}

static void gen_rand_expr(int deps) {
  if(deps >= 15){
    gen_num();
    return;
  }
  gen_rand_space();
  int r = rand();
  switch(r%8) {
    case 0: 
      gen_num(); break;
    case 1:
      gen_str("(");
      gen_rand_expr(deps+1);
      gen_str(")");
      break;
    case 2:
      gen_sig_oprator();
      gen_rand_expr(deps+1);
      break;
    case 3:
      gen_hex_num();
      break;
    case 4:
      gen_reg();
      break;
    default:
      // gen_str("(");
      gen_rand_expr(deps+1);
      // gen_str(")");
      gen_bin_oprator();
      // gen_str("(");
      gen_rand_expr(deps+1);
      // gen_str(")");
      break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = TEST_LOOP;

  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  // For log //
  FILE* fp_log;
  fp_log = fopen("temp/formatted_output.txt", "w");
  ////////////
  for (i = 0; i < loop; i ++) {
    out_buf_len = 0;
    cal_buf_len = 0;
    gen_rand_expr(0);
    // //for test//
    // char* cal_buf;
    // cal_buf = "   0x6ffb0a88u/  0 ";
    // char* output_buf;
    // output_buf = " 0x6ffb0a88 / $s7 ";
    // ///////////////
    sprintf(code_buf, code_format, cal_buf);

    FILE* fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr -Werror=div-by-zero");

    if (ret != 0){
      fprintf(stderr, "Divided by 0\n");
      continue;
    }

    fp = popen("/tmp/.expr 2>&1", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%u", &result);
    ret = pclose(fp);
    // setvbuf(stdout, NULL, _IONBF, 0);
    // if(ret !=0 ) {
    //   printf("Div-0: %u %s\n",result, output_buf);
    //   continue;
    // }
    printf("%u %s\n", result, output_buf);
    // TEST LOG
    fputs(output_buf, fp_log);
    fputs("\n", fp_log);
    fputs(cal_buf, fp_log);
    fputs("\n", fp_log);
    //////////////
  }

  fclose(fp_log);
  return 0;
}
