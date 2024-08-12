#include <common.h>

char * test[] = {" + ==  == +   ",
                " +== ==+ ",
                " -1100+124321 1",
                " -1100*+/124321 1",
                " -1100*(124+ ==)+/124321 1"
                };

char *test1[] = {
            "(- - - (- (((- - - 885390357* (659302715)))))- 1396816825- - 1830826051)/ - - (- - (833704168))" // 0,
            // "0---1" //1,
            // "**0x80000000",
            // "(715297071/ 1782224548- 50696570)/ (((- - 1343783997== (- 542387304)- 177128205== 227824775+ (1800242619== - 306024887)+ ((2101948727))+ 1298249076+ (1756170868)/ 168728087* (1820495742)))/ - - - 179891807+ - 1861879517== - - 1329539396/ - 336703603* (- 63898082))/ 1311081201* - ((- 17600449* 1336875997/ - 1752608075/ ((- ((((744053253== - (1809660889)== 1827261338))))))))* - ((- (- 619913351/ ((- 539146097/ 201323338/ (1378318547)- - (715276128)))* (- - - (540422640)))))== - - (677739675)* - 1718926816+ ((1985792762))/ - (- ((- 486827565)== - - (- - (- - (562318920))+ (1637301488+ (((- (1433372336== 60676211)))))/ ((484186856* (659338793)))* (((- - - 654569578== 1794539630/ 611765740)/ (804529983== (2024247379)== ((86454613))/ - (1216832262)== (461033023)))+ ((221180973))))- 179604720* (1488549458)== 1365313190== - - 494785706== (623329096)== - 786721546+ ((- - 1518495415- (((- (1246705769))))== - 1792351280+ ((377896079)/ (1303334038))))- ((693149285)/ 556938361- ((1633604713))- (- 1402361695- 1781233601)- 1284671574/ ((- 1567651809))* (- 1013223868* - (- 227051014))* ((402705783)- - - (474045587/ 493308098)== 1180549723- (- - 802567724))/ 784760597+ - 773672494* 1277289461- (- - - (2088593858)))- ((((171559839/ ((- 1813907479)== 1251814088)))))/ (- 884826273)))- (720332965)+ ((- 1631888701+ - - 602617031))== - - 232692359== 1117518632+ (582988808)",
            // "1+2*3-5",//2
            // "-1+2*3-5",//0
            // "(-1   + 2)- 3    -5",//-7
            // "(-1+2)*3-5",//-2
            // "-(-1+2)*3-5",//-8
            // "-((-1+2)*3)-5",//--8
            // "--1" //1
};

word_t expr(char* e, bool* success);

word_t getMainOptr_UT(char *e, bool *success);
bool make_token(char *e) ;

#define MAX_BUF_SIZE 262104
static char buff[MAX_BUF_SIZE];
static char ans_buf[32];

int32_t loop = 1000;
void expr_ut(void){
    bool succ;
    // for(int i = 0; i < sizeof(test)/sizeof(char*); i++){
    //     // getMainOptr_UT(test1[i], &succ);
    //     word_t val = expr(test[i], &succ);
    //     if(succ){
    //         printf("Get value: %d\n", val);
    //     }
    // }

    // for(int i = 0; i < sizeof(test1)/sizeof(char*); i++){
    //         // getMainOptr_UT(test1[i], &succ);
    //     word_t val = expr(test1[i], &succ);
    //     if(succ){
    //         printf("Get value: %d\n", val);
    //     }
    // }
    for(int i = 0; i < loop; i++){
        if(!fgets(buff, MAX_BUF_SIZE-1, stdin)){
            printf("%d", buff[strlen(buff)-1]);
            Log("Wrong input:%s", buff);
        }
        assert(buff[strlen(buff)-1] == '\n'  || //input from file
                    buff[strlen(buff)-1] == EOF ); //input from end line of file
        buff[strlen(buff)-1] = '\0';
        sscanf(buff, "%s", ans_buf);
        word_t ans = strtol(ans_buf, NULL, 10);
        bool success = false;
        word_t eval_ans = expr(buff+strlen(ans_buf), &success);
        if(success){
            FILE* fp = fopen("wrong_expr.txt", "w");
            if(fp){
                fprintf(fp, "%s", buff);
                fclose(fp);
            }
            Assert(ans == eval_ans, "Wrong Ans: %u vs Right Ans %u\n expression:%s\n", eval_ans, ans, buff+strlen(ans_buf));
            // Log("PASSED, Expression: %s", buff+strlen(ans_buf));
        } else{
            Log("Something Wrong Happend, Expression:%s", buff+ strlen(ans_buf));
        }
    }
}