#include "npc_tb.hh"

void init_monitor(int, char *[]);

int npc_tb(Vtop* top, int argc, char* argv[]){
  init_monitor(argc, argv);

  return 0;

  // parse_args(argc, argv);

  // reset(top);

  // init_mem(top);
  // // printf("world\n");
  // cpu_tick(top, 100);
  // // printf("Bye\n");
  // return  0;
}