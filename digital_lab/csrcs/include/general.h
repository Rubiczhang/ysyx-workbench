#ifndef TOP_NAME
    #define TOP_NAME mux4to1_2bit
#endif //TOP_NAME


#ifndef __GENERAL_H_
#define __GENERAL_H_
#define STRINGFY(x) #x
#define TOSTRING(x) STRINGFY(x)
#define CONCATENATE(x, y) x##y
#define CONCATENATE_STR(x,y)  CONCATENATE(x, y)
#define APPEND_H(x) TOSTRING(CONCATENATE_STR(V, TOP_NAME).h)

#define VTOP    CONCATENATE_STR(V, TOP_NAME) //Vmux4to1_2bit

#define TOP_TEST_H TOSTRING(CONCATENATE_STR(TOP_NAME, _test.h))
//"mux4to1_2bit_test.h"

#define TOP_TEST_FUNC CONCATENATE_STR(TOP_NAME, _test)
//mux4to1_2bit_test

#define VTOP_HEADER_NAME  APPEND_H(TOP_NAME)
//"Vmux4to1_2bit.h"

#ifdef TRACE_ENABLE
    #include "verilated_fst_c.h"
    extern VerilatedFstC* tfp;
#endif  //TRACE_ENABLE

#include  VTOP_HEADER_NAME
#include <verilated.h>
#include TOP_TEST_H

#include <nvboard.h>


void assert_with_wave(bool x);
// void singleStep(VTOP* top);

// void reset(VTOP* top);
#endif //__GENERAL_H_