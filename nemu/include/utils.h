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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <common.h>

// ----------- state -----------

enum { NEMU_RUNNING, NEMU_STOP, NEMU_END, NEMU_ABORT, NEMU_QUIT };

typedef struct {
  int state;
  vaddr_t halt_pc;
  uint32_t halt_ret;
} NEMUState;

extern NEMUState nemu_state;

// ----------- timer -----------

uint64_t get_time();

// ----------- log -----------

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;35m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE

typedef enum{
  I_TRACE,
  M_TRACE,
  F_TRACE
} trace_type_t;

#define log_write(...) IFDEF(CONFIG_TARGET_NATIVE_ELF, \
  do { \
    extern FILE** log_fps; \
    extern bool log_enable(); \
    if (log_enable()) { \
      fprintf(log_fps[I_TRACE], __VA_ARGS__); \
      fflush(log_fps[I_TRACE]); \
    } \
  } while (0) \
)

#define xlog_write(trace_type, ...) IFDEF(CONFIG_TARGET_NATIVE_ELF, \
  do { \
    extern FILE** log_fps; \
    extern bool xlog_enable(trace_type_t); \
    if (xlog_enable(trace_type)) { \
      fprintf(log_fps[trace_type], __VA_ARGS__); \
      fflush(log_fps[trace_type]); \
    } \
  } while (0) \
)

#define _Log(...) \
  do { \
    printf(__VA_ARGS__); \
    log_write(__VA_ARGS__); \
  } while (0)

#if  defined(CONFIG_ITRACE_COND) && defined(CONFIG_IRINGBUF)
#define ITRACE_MAX_LEN 256
typedef struct {
  char  str_buf[CONFIG_IRINGBUFSIZE][ITRACE_MAX_LEN];
  int hdr;
  bool fulled;
} iring_buf_t;

extern iring_buf_t iring_buf;

void flush_iring_buf();

#endif  // defined(CONFIG_ITRACE_COND) && defined(CONFIG_IRINGBUF)


void log_ftrace_jalr(uint32_t des, uint32_t this_pc, uint32_t rd, uint32_t rs1);
void log_ftrace_jal(uint32_t des, uint32_t this_pc, uint32_t rd);
#endif
