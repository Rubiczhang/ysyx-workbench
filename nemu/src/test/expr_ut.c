#include <common.h>

char * test[] = {" + ==  == +   ",
                " +== ==+ ",
                " -1100+124321 1",
                " -1100*+/124321 1",
                " -1100*(124+ ==)+/124321 1"
                };

char *test1[] = {
            "1+2*3-5",//2
            "-1+2*3-5",//0
            "(-1   + 2)- 3    -5",//-7
            "(-1+2)*3-5",//-2
            "-(-1+2)*3-5",//-8
            "-((-1+2)*3)-5",//--8
            "--1" //1
};

word_t expr(char* e, bool* success);

word_t getMainOptr_UT(char *e, bool *success);
bool make_token(char *e) ;

#define MAX_BUF_SIZE 65536
static char buff[MAX_BUF_SIZE];
static char ans_buf[32];

int32_t loop = 32;
void expr_ut(void){
    // for(int i = 0; i < sizeof(test)/sizeof(char*); i++){
    //     // getMainOptr_UT(test1[i], &succ);
    //     word_t val = expr(test[i], &succ);
    //     if(succ){
    //         printf("Get value: %d\n", val);
    //     }
    // }
    // for(int i = 0; i < sizeof(test1)/sizeof(char*); i++){
    //         // getMainOptr_UT(test1[i], &succ);
    //         word_t val = expr(test1[i], &succ);
    //         if(succ){
    //             printf("Get value: %d\n", val);
    //         }
    // }
    for(int i = 0; i < loop; i++){
        if(!fgets(buff, MAX_BUF_SIZE-1, stdin)){
            Log("Wrong input");
        }
        assert(buff[strlen(buff)-2] == '\n');
        buff[strlen(buff)-2] = '\0';
        sscanf(buff, "%s", ans_buf);
        word_t ans = strtol(ans_buf, NULL, 10);
        bool success = false;
        word_t eval_ans = expr(buff+strlen(ans_buf), &success);
        if(success){
            Assert(ans == eval_ans, "Wrong Ans: %d vs Right Ans %d\n expression:%s\n", eval_ans, ans, buff+strlen(ans_buf));
        }
    }
}