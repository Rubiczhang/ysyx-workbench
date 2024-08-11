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

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static int buf_len = 0;
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


static const char* const bin_oprator[] = {
  "+", "-", "*", "/", "=="
};

static const char* const sig_oprator[] = {
  "-"
};
static int32_t  nr_bin_oprator = sizeof(bin_oprator)/sizeof(char*);
static int32_t  nr_sig_oprator = sizeof(sig_oprator)/sizeof(char*);

void gen_num(){
  int32_t r = rand();
  sprintf(buf+buf_len, "%u", r);
  buf_len = strlen(buf);
}

void gen_str(const char* str){
  sprintf(buf+buf_len, "%s", str);
  buf_len+= strlen(str);
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

static void gen_rand_expr(int deps) {
  if(deps >= 30){
    gen_num();
    return;
  }
  int r = rand();
  switch(r%4) {
    case 0: 
      gen_num(); break;
    case 1:
      gen_str("(");
      gen_rand_expr(deps+1);
      gen_str(")");
      break;
    case 2:
      // gen_str("(");
      gen_rand_expr(deps+1);
      // gen_str(")");
      gen_bin_oprator();
      // gen_str("(");
      gen_rand_expr(deps+1);
      // gen_str(")");
      break;
    case 3:
      gen_sig_oprator();
      gen_rand_expr(deps+1);
      break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf_len = 0;
    gen_rand_expr(0);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
