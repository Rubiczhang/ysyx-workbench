#include <common.h>

char * test[] = {" + ==  == +   ",
                " +== ==+ ",
                " -1100+124321 1",
                " -1100*+/124321 1",
                " -1100*(124+ ==)+/124321 1"
                };

char *test1[] = {
            "1+2*3-5",//2
            "-1+2*3-5",//2
            "(-1   + 2)- 3    -5",//-7
            "(-1+2)*3-5",//-2
            "-(-1+2)*3-5",//-8
            "-((-1+2)*3)-5",//--8
            "--1" //1
};

word_t expr(char* e, bool* success);

word_t getMainOptr_UT(char *e, bool *success);
bool make_token(char *e) ;


void expr_ut(void){
    bool succ;
    for(int i = 0; i < sizeof(test)/sizeof(char*); i++){
        // getMainOptr_UT(test1[i], &succ);
        word_t val = expr(test[i], &succ);
        if(succ){
            printf("Get value: %d\n", val);
        }
    }
    for(int i = 0; i < sizeof(test1)/sizeof(char*); i++){
            // getMainOptr_UT(test1[i], &succ);
            word_t val = expr(test1[i], &succ);
            if(succ){
                printf("Get value: %d\n", val);
            }
    }
}