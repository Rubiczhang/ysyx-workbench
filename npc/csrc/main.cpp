
#include <verilated.h>
#include <Vtop.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <signal.h>
#include <nvboard.h>
#include <npc_tb.hh>


#ifdef TRACE_ENABLE
#include "verilated_fst_c.h"
#endif  //TRACE_ENABLE

std::unique_ptr<Vtop> top;
// const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};

void exit_routine(){
    std::cout << "exting..." <<std::endl;
    #ifdef TRACE_ENABLE
        if(tfp){
            tfp->flush();
            tfp->close();
        }
    #endif
    top.reset();
}

#ifdef TRACE_ENABLE
VerilatedFstC* tfp;

void exit_handler(int signum){
    // assert(signum == SIGINT);
    exit_routine();
    std::cout<< "exiting\n" ;
    exit(0);
}

void assert_with_wave(bool x){
    if(!x){
        exit_routine();
    }
    assert(x);
}

#endif //TRACE_ENABLE


void nvboard_bind_all_pins(Vtop* top);


int main(int argc, char** argv){
    std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};

    contextp->commandArgs(argc, argv);

#ifdef TRACE_ENABLE
    contextp->traceEverOn(true);
    signal(SIGINT, exit_handler);
    signal(SIGABRT, exit_handler);
#endif
    top = std::make_unique<Vtop>(contextp.get(), "TOP");
    // const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};

    // nvboard_bind_all_pins(top.get());
    // nvboard_init();
#ifdef TRACE_ENABLE
    top->contextp()->traceEverOn(true);
    // const char* flag = Verilated::commandArgsPlusMatch("trace");
    // if(flag && 0 == std::strcmp(flag, "+trace")){
    std::cout << "Enabling waves into logs/vlt_dump.fst .... \n";
    tfp = new VerilatedFstC;
    top->trace(tfp, 99);
    Verilated::mkdir("logs");
    tfp->open("logs/vlt_dump.fst");
    // }
#endif
    npc_tb(top.get(), argc, argv);
    // printf("Bye at main\n");
    exit_routine();
    printf("Bye at end\n");
    return 0;
}
