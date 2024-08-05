#include "general.h"
#include <iostream>
#include <random>

void singleTest(VTOP* top, int A, int B, int sel, int ans){
    top->contextp()->timeInc(1);
    top->A = A;
    top->B = B;
    top->sel = sel;
    top->eval();
    std::printf("TESTING: A:%d, B:%d, sel:%d, ans:%d\n", A, B, sel, ans);
#ifdef TRACE_ENABLE
        // std::cout<<"a" ;
        if(tfp){
            tfp->dump(top->contextp()->time());
        }
    assert_with_wave(top->ans == ans );
#else
    assert(top->ans == ans );

#endif //TRACE_ENABLE
    std::printf("PASSED: A:%d, B:%d, sel:%d, ans:%d\n", A, B, sel, ans);
}

int logic_getAns(int A, int B, int sel){
    if(A>=8)
        A -= 16;
    if(B>=8)
        B -= 16;
    int ans;
    switch(sel){
        case 0b000:
            ans =A+B;
            break;
        case 0b001:
            ans = A-B;
            break;
        case 0b010:
            ans = ~A;
            break;
        case 0b011:
            ans = A & B;
            break;
        case 0b100:
            ans = A | B;
            break;
        case 0b101:
            ans = A ^ B;
            break;
        case 0b110:
            ans = A < B ? 1: 0;
            break;
        case 0b111:
            ans = A == B ? 1: 0;
            break;

    }
    if(ans < 0) 
        ans += 16;
    return ans;
}

void autoTest(VTOP* top, int n){
    assert(n > 0);
    while(n--){
        int A = std::rand()%16;
        int B = std::rand()%16;
        int sel = std::rand()%8;
        singleTest(top, A, B, sel, logic_getAns(A, B, sel));
    }
}


void test(VTOP *top){
    std::srand(1);
    autoTest(top, 1000);
}

void TOP_TEST_FUNC(VTOP* top){
    test(top);
}
