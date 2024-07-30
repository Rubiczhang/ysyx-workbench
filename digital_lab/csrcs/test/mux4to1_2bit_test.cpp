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

void mux4to1_2bit_test(VTOP* top){
    std::cout << "testing..." << std::endl;
    int x[] = {0b11, 0b10, 0b01, 0b00};
    test(top, 0, x);
}