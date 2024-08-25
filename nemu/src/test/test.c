#include <common.h>
#include <unit_test/unit_test.h>

void unit_test(void){

#ifdef CONFIG_EXPR_UT
    expr_ut();
#endif

#ifdef CONFIG_WTCHPNT_UT
    wtchpnt_ut();
#endif
}