#include "general.h"

void test_board(VTOP* top){
    top->rst_val = 1;
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

void TOP_TEST_FUNC(VTOP* top){
    test_board(top);
}

