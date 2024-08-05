#include "general.h"
#include <cstdio>

void dumpTrace(VTOP* top){
#ifdef TRACE_ENABLE
    top->contextp()->timeInc(1);
    if(tfp){
        tfp->dump(top->contextp()->time());
    }
#endif
}

unsigned int getOddParity(unsigned char data){
//input: 8bit data
//return value:~(^(data))    1 bit odd parity
    unsigned int xor_res = data & 1;
    for(int i = 0; i < 7; i++){
        data  >>= 1;
        xor_res ^= data & 1;
    }
    return xor_res ? 0: 1;
}

unsigned int fmtPs2Data(unsigned char data){
    unsigned int wide_data = data;
    wide_data = wide_data << 1; //start bit
    unsigned odd_parity = getOddParity(data);
    wide_data |= (odd_parity << 9);
    wide_data |= (1 << 10);
    return wide_data;
}


void tick( VTOP* top, int &clk_nums, int sending){
    top->clk = top->clk ? 0: 1;
    if(!sending)
        return;
    clk_nums++;
    if(clk_nums == 10) clk_nums = 0;
    //get ps2_clk
    if(clk_nums == 0){
        top->ps2_clk = 0;
    }
    else if(clk_nums == 5)
        top->ps2_clk = 1;
}

void tick_eval_dump(VTOP* top, int n){
    while(n--){
        top->clk = top->clk ? 0: 1;
        top->eval();
        dumpTrace(top);
    }
}


//Testcase0: clrn
void clrn(VTOP* top){
    int n = 10;
    top->clrn = 0;
    while(n--){
        top->clk = top->clk? 0 : 1;
        top->eval();
        dumpTrace(top);
    }
}

void sendToPs2(VTOP* top, unsigned char data){
    int send_least = 11;
    int clk_nums = -1;
    unsigned int fmt_ps2_data = fmtPs2Data(data);
    bool sending = true;
    while(sending){
        tick(top, clk_nums, sending);
        if(clk_nums ==0){      //ugly hack, ps2_clk negedge
            if(send_least == 11){ //fisrt bit, not exactly same as the real sender.
                top->ps2_data = fmt_ps2_data & 1;
                fmt_ps2_data >>= 1;
                send_least--;
            }

        }
        if(clk_nums == 5){      //ugly hack, ps2_clk posedge
            if(send_least == 0){    //a little tricky, pleae check the wave.
                sending = false;
            }
            top->ps2_data = fmt_ps2_data & 1;
            fmt_ps2_data >>= 1;
            send_least--;
        }
        top->eval();
        dumpTrace(top);
    }
}

// Testcase1: simple input and output
void testCase1(VTOP* top){
    unsigned char ps2_data;

    // assert(0);
    unsigned int fmt_data;
        //test fmtPs2Data
        assert(getOddParity(0x11) == 1);
    
        ps2_data = 0x11;
        fmt_data = fmtPs2Data(ps2_data);
        assert(fmt_data == 0x622);
    sendToPs2(top, 0x11);
    top->eval();
    tick_eval_dump(top, 10);
    dumpTrace(top);
#ifdef TRACE_ENABLE
    assert_with_wave(top->data == 0x11);
    assert_with_wave(top->ready == 1);
    assert_with_wave(top->overflow== 0);
#endif
    assert(top->data == 0x11);
    assert(top->ready == 1);
    assert(top->overflow== 0);
    printf("PASSED Normal Case\n");
}

//Testcase2: wrong parity bit

//Testcase3: 4 ready data in FIFO, and read them all

//Testcase4: 9 ready data in FIFO, 
// check overflow
//and read them all
// check overflow


void test(VTOP* top){
// ps_clk is 10 times long than clk

//in each testcase, ps2_clk negedge is followed with ps2_data

//nextdata_n is set to 0, only when ready is 1, and only one cycle
    testCase1(top);
}

void TOP_TEST_FUNC(VTOP* top){
    clrn(top);
    top->clrn = 1;
    top->ps2_clk = 1;
    top->nextdata_n = 1;
    int n = 10;
    tick_eval_dump(top, 10);
    test(top);
}