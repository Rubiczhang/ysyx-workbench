#include "general.h"
#include <iostream>

void test(VTOP* top, int s, int* x){
    top->contextp()->timeInc(1);
    top->sel = s;
    for(int i = 0; i < 4; i++){
        top->x[i] = x[i];
    }
    top->eval();
    nvboard_update();
    assert(x[s] == top->out);
    std::cout <<"PASSED" <<std::endl;
}

void test_board(VTOP* top){
    while(1){
        top->contextp()->timeInc(1);
        top->eval();
        nvboard_update();
#ifdef TRACE_ENABLE
        // std::cout<<"a" ;
        if(tfp){
            tfp->dump(top->contextp()->time());
        }
#endif //TRACE_ENABLE
    }
}

void mux4to1_2bit_test(VTOP* top ){
    test_board(top);
}