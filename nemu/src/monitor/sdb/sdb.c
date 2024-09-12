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
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/vaddr.h>
#include "sdb.h"
#include <monitor/sdb/watchpoint.h>
#include <utils.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();


/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return -1;
}


static int cmd_q(char *args) {
  Log("Quit\n");
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args) ;

static int cmd_wp(char* args);

static int cmd_d(char *args);


static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if(arg){
    if(!strcmp(arg, "r")){
      isa_reg_display();
    }
    if(!strcmp(arg, "b")){
      show_all_working();
    }
  }
  return 0;
}

static int cmd_p(char* args){
  char* expr_str = strtok(NULL, "\0");
  if(!expr_str){
    printf("Wrong usage for p cmd\n");
    return 0;
  }
  bool succ = false;
  word_t val= expr(expr_str, &succ);
  if(!succ){
    printf("Wrong usage of p cmd, Wrong expression:%s\n", expr_str);
    return 0;
  }
  printf("0x%08x\n", val);
  return 0;
}

static int cmd_x(char* args){
  char* n_str = strtok(NULL, " ");
  char* addr_str = strtok(NULL, "\0");
  if(!(n_str && addr_str)){
    printf("Wrong usage for x cmd\n");
    return 0;
  }
  char *endptr = NULL;
  int n = strtol(n_str, &endptr, 10);
  Assert(endptr && *endptr == '\0' , "Inner error\n");

  bool succ = false;
  vaddr_t addr = expr(addr_str, &succ);
  // Assert(succ , "Inner error\n");
  if(!succ){
    printf("Wrong usage of x cmd, Wrong expression:%s\n", addr_str);
    return 0;
  }
  for(int i = 0; i < n; i++){
    word_t val = vaddr_read(addr, 4);
    printf("0x%08x ", val);
    addr += 4;
  }
  printf("\n");
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step instrution", cmd_si },
  { "info",  "Generic command for showing things about the program being debugged.", cmd_info },
  { "x", "Examine memory: x LENGTH ADDRESS. Output LENGHT * 32bit after ADDRESS",  cmd_x},
  { "watch", "Add watch point", cmd_wp},
  { "d", "delete break point", cmd_d},
  { "p", "Print value of expression", cmd_p}
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  long long step = 1;
  char *arg = strtok(NULL, " ");
  char *endptr = NULL;
  
  if(arg){ //There are args
    step = strtoll(arg, &endptr,  10);
    if(!endptr || *endptr != '\0'){
      printf("%s, Step instrution format: si [N]\n e.g.: si or si 10\n"
       , ANSI_FMT(str(Wrong instr fmt), ANSI_FG_RED));
      return 0;
    }
  }
  cpu_exec(step);
  return 0;
}

static int cmd_wp(char *args) {
  char* expr = strtok(NULL, "\0");
  if(expr == NULL){
    printf("Wrong usage of whatchpoint\n");
    return -1;
  }
  new_wp(expr);
  return 0;
}

static int delete_wp(char* args){
  char* no_str = strtok(NULL, " ");
  int no = -1;
  if(no_str == NULL){
    printf("Wrong usage of cmd delete");
    return -1;
  }
  char* endptr = NULL;
  no = strtol(no_str, &endptr,  10);
  if(!endptr || *endptr != '\0'){
      printf("Wrong usage of delete wp");
  }
  free_wp_by_no(no);
  return 0;
}

static int cmd_d(char* args){
  return delete_wp(args);
}



void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
