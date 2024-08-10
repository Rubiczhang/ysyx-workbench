#include <common.h>

char * test[] = {" + ==  == +   ",
                " +== ==+ ",
                // " -1100+124321 1",
                // " -1100*+/124321 1",
                // " -1100*(124+ ==)+/124321 1"
                };

char *test1[] = {
            "1+2*3-5",//2
            "1+2*3-5"//2
};

word_t expr(char* e, bool* success);

void expr_ut(void){
    bool succ;
    for(int i = 0; i < sizeof(test)/sizeof(char*); i++){
        expr(test1[i], &succ);
        if(succ){
            printf("Passed expr: %s\n", test[i]);
        }
    }
}