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

#include <common.h>

extern uint64_t g_nr_guest_inst;

#ifndef CONFIG_TARGET_AM
FILE **log_fps;

static void init_xlog(FILE** log_fp, char* log_file ){
  *log_fp = stdout;
  if (log_file != NULL) {

#ifdef CONFIG_BATCH_ITRACE
    FILE *fp = fopen(log_file, "a");
#else
    FILE *fp = fopen(log_file, "w");
#endif
    Assert(fp, "Can not open '%s'", log_file);
    *log_fp = fp;
  }
  Log("Log is written to %s", log_file ? log_file : "stdout");
}

void init_log(const char *log_dir) {

  log_fps = malloc(sizeof(FILE*)*3);
  Assert(log_fps != NULL, "malloc error\n");
  char log_files[3][128];
  char imf[4] = "imf";
  for(int i = 0; i < 3; i++){
    sprintf(log_files[i], "%s/%c_trace.log", log_dir, imf[i]);
  }
  for(int i = 0; i <= F_TRACE; i++)
    init_xlog(log_fps+i, log_files[i]);

//   log_fp = stdout;
//   if (i_log_file != NULL) {

// #ifdef CONFIG_BATCH_ITRACE
//     FILE *fp = fopen(i_log_file, "a");
// #else
//     FILE *fp = fopen(i_log_file, "w");
// #endif

//     Assert(fp, "Can not open '%s'", i_log_file);
//     log_fp = fp;
//   }
//   Log("Log is written to %s", i_log_file ? i_log_file : "stdout");
}
//log is default regar
bool log_enable() {
  return MUXDEF(CONFIG_TRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
         (g_nr_guest_inst <= CONFIG_TRACE_END), false);
}

bool xlog_enable(trace_type_t trace_type){
  assert(trace_type <= F_TRACE);
  switch(trace_type){
    case I_TRACE:
      return MUXDEF(CONFIG_TRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
             (g_nr_guest_inst <= CONFIG_TRACE_END), false);
    case M_TRACE:
      return MUXDEF(CONFIG_MTRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
             (g_nr_guest_inst <= CONFIG_TRACE_END), false);
    case F_TRACE:
      return MUXDEF(CONFIG_FTRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
             (g_nr_guest_inst <= CONFIG_TRACE_END), false);
  }
  return false;
}


#if  defined(CONFIG_ITRACE_COND) && defined(CONFIG_IRINGBUF)
void flush_iring_buf(){
  int hdr = iring_buf.hdr;
  if(iring_buf.fulled){
    for(int i = hdr + 1; i < CONFIG_IRINGBUFSIZE; i++){
      log_write("%s\n", iring_buf.str_buf[i]);
    }
  }
  for(int i = 0; i < hdr; i++){
      log_write("%s\n", iring_buf.str_buf[i]);
  }
}
#endif //defined(CONFIG_ITRACE_COND) && defined(CONFIG_IRINGBUF)

#endif  //CONFIG_TRAGET_AM
