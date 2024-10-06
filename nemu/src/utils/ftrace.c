#include <common.h>
#include <monitor/elf_utils.h>

extern int ftrace_call_depth;

extern int elf_nr_sym;

#ifdef  CONFIG_FTRACE
static void _log_ftrace(uint32_t des, uint32_t this_pc, uint32_t rd, uint32_t rs1){
  if(rd == 0 && rs1 != 1){
    return;
  }
  for(int i = 0; i < ftrace_call_depth; i++){
    xlog_write(F_TRACE, "  ");
  }

  char* sym_name;
  if(rd == 0 && rs1 == 1){ //ret
    sym_name = get_fname(this_pc, elf_nr_func);
    xlog_write(F_TRACE, "ret %s @0x%8x\n", sym_name, this_pc);
    ftrace_call_depth--;
  } else if(rd != 0){
    sym_name = get_fname(des, elf_nr_func);
    xlog_write(F_TRACE, "  ");
    xlog_write(F_TRACE, "call %s @0x%8x\n", sym_name, this_pc);
    ftrace_call_depth++;
  }
}
#endif


void log_ftrace_jalr(uint32_t des, uint32_t this_pc, uint32_t rd, uint32_t rs1){
  IFDEF(CONFIG_FTRACE, _log_ftrace(des, this_pc, rd, rs1));
}

#define NOTX1 2
void log_ftrace_jal(uint32_t des, uint32_t this_pc, uint32_t rd){
  IFDEF(CONFIG_FTRACE, _log_ftrace(des, this_pc, rd, NOTX1));
}