#include "general.h"
#include <iostream>

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

void singleTest(VTOP* top, int x, int en, int y, int outEn){
    top->contextp()->timeInc(1);
    top->x = x;
    top->en = en;
    
    top->eval();
#ifdef TRACE_ENABLE
        // std::cout<<"a" ;
        if(tfp){
            tfp->dump(top->contextp()->time());
        }
#endif //TRACE_ENABLE
    assert(top->y == y);
    assert(top->outEn == outEn);
    std::cout << "PASSED \n";
}

void test(VTOP* top){
    int x, en, y, outEn;
    x = 0b0110'1001;
    en = 1;
    y = 6;
    outEn = 1;
    singleTest(top, x, en, y, outEn);
    en = 0;
    y = 0;
    outEn = 0;
    singleTest(top, x, en, y, outEn);
    x = 0b0;
    en = 1;
    y = 0;
    outEn = 0;
    singleTest(top, x, en, y, outEn);
}

void encode83_test(VTOP* top){
    test_board(top);
    // test(top);
}