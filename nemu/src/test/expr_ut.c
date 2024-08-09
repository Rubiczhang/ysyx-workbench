#include <common.h>

char * test[] = {" + ==  == +   "};

void expr(char* e, bool* success);

void expr_ut(void){
    bool succ;
    for(int i = 0; i < sizeof(test)/sizeof(char*); i++){
        expr(test[i], &succ);
        if(succ){
            printf("Passed expr: %s\n", test[i]);
        }
    }
}