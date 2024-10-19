#include <npc_tb.hh>
#include <iostream>


extern "C" void init_monitor(int, char *[]);

// extern "C" void handle_ebreak(){
//   std::cout << "EBREAK encountered!" <<std::endl;
//   exit_routine();
//   exit(0);
// }

extern "C" void engine_start();

int npc_tb(Vtop* top, int argc, char* argv[]){
  init_monitor(argc, argv);

  engine_start();

  // return 0;

  // parse_args(argc, argv);

  // reset(top);

  // init_mem(top);
  // // printf("world\n");
  // cpu_tick(top, 100);
  // // printf("Bye\n");
  return  0;
}