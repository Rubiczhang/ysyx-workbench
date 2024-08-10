/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <stdbool.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>


enum {
  TK_NOTYPE = 256, 
  TK_EQ,
  TK_DINT,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"[0-9]+", TK_DINT},        // decimal int
  {"-", '-'},
  {"\\*", '*'},
  {"/", '/'},
  {"\\(", '('},
  {"\\)", ')'}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;


static bool isSingleOperator(Token token);

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    /* Only deal with the match at begining*/
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: //space, do nothing
            break;
          default:
            strncpy(tokens[nr_token++].str, substr_start, substr_len);
          #ifdef UT
            Log("added token: #%d: %.*s", nr_token-1, substr_len, substr_start);
          #endif
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


static void print_tokens(Token* tokens, int32_t beg, int32_t end ){
  Log("Illegal expression: ");
  for(int i = beg; i < end; i++){
    printf("%s ", tokens[i].str);
  }
  printf("\n");
}

static int32_t getEndOfParnth(Token* tokens, int beg, int end){
  assert(tokens[beg].type == '(');
  int cnt_left_prntth = 1;
  int i = 0;
  for(i = beg; i <= end; i++){
    if(tokens[i].type == '(')
      cnt_left_prntth ++;
    else if(tokens[i].type == ')')
      cnt_left_prntth--;
    if(cnt_left_prntth == 0){
      return i;
    }
  }
  return -1;
}

// static int32_t getEndOfFirstExpr(Token* tokens, int beg, int end){
//   //return beg if fisrt token is decimal int
//   //return pos of corrosponding ')' if first token is '('
//   assert( 
//           tokens[beg].type == TK_DINT ||
//           tokens[beg].type == '(' ||
//           isSingleOperator(tokens[beg])
//   );
//   if(tokens[beg].type == TK_DINT){
//     return beg;
//   }
//   else if(tokens[beg].type == '('){
//     return getEndOfParnth(tokens, beg, end);
//   }
//   else if(isSingleOperator(tokens[beg])){
//     return getEndOfFirstExpr(tokens, beg+1, end);
//   }
// }

static bool isBinOperator(Token token){
  bool res = false;
  switch(token.type){
    case '+':
    case '-':
    case '*':
    case '/':
    case TK_EQ:
      res = true;
      break;
  }
  return res;
  
}
static bool isSingleOperator(Token token){
  bool res = false;
  switch(token.type){
    case '-':
    case '*':
      res = true;
      break;
  }
  return res;
}

static word_t getBinOprValue(word_t first, Token op_tk, word_t last){
  Assert(isBinOperator(op_tk), "Op is not binary operater, Op:%s\n", op_tk.str);

  word_t val = 0;
  switch(op_tk.type){
    case '+':
      val = first+last;
      break;
    case '-':
      val = first-last;
      break;
    case '*':
      val = first*last;
      break;
    case '/':
      val = first/last;
      break;
    case TK_EQ :
      val = first == last;
      break;
  }
  return val;
}

static word_t  getSigOprValue(Token op_tk, word_t oprnd_val){
  assert(isSingleOperator(op_tk));
  word_t val = 0;
  
  switch(op_tk.type){
    case '-':
      val = - oprnd_val;
      break;
    // case '*':
    //   val = *(word_t*)oprnd_val;
    //   break;
  }
  return val;
}

static int32_t prcdcOprtr(Token op_tk){
  assert(isBinOperator(op_tk) | isSingleOperator(op_tk));
  switch(op_tk.type){
    case '+':
    case '-':   //same precedence for sub and neg Operator
      return 1;
    case '*':   //same precedence for ptr and mul Operator
    case '/':
      return 2;
  }
  Assert(0, "Inner Error: get precendence of token: %s\n", op_tk.str);
  return 0xffff;
}

static int32_t getMainOprtr(Token* tokens, int beg, int end){
  int32_t mainOprtPos = -1;
  int32_t mainOptrPrcdc = 0xffff;
  for(int i = beg; i < end; ){
    if(tokens[i].type == '(' ){
      i = getEndOfParnth(tokens, i, end);
      if(i == -1)
        print_tokens(tokens, beg, end);
      continue;
    }
    if(isBinOperator(tokens[i]) || isSingleOperator(tokens[i])){
      if(prcdcOprtr(tokens[i]) <= mainOptrPrcdc){
        mainOprtPos = i;
      }
    }
    i++;
  }

  return mainOprtPos;
  
}

static word_t eval(Token* tokens, int beg, int end){
  word_t value = 0;
  Assert(beg <= end, "input of eval is illegal: beg:%d, end:%d\n",beg, end);
  if(beg == end){           //<number>
    assert(tokens[beg].type == TK_DINT);
    value = strtol(tokens[beg].str, NULL, 10);
  }
  else if(tokens[beg].type == '(' && (getEndOfParnth(tokens, beg, end) == end)){
    // '(' <expr> ')'
    value = eval(tokens, beg+1, end-1);
  }
  else{
    int32_t mainOptrPos = getMainOprtr(tokens, beg, end);
    Log("mainPtrPos: %d\n", mainOptrPos);
    if(mainOptrPos >= end){
      print_tokens(tokens, beg, end);
      return value;
    }
    
    word_t leftValue;
    if(mainOptrPos > beg)
      leftValue = eval(tokens, beg, mainOptrPos-1);

    Token op_token = tokens[mainOptrPos];
    // assert(isBinOperator(op_token));

    word_t rightValue = eval(tokens, mainOptrPos+1, end);
    
    if(mainOptrPos > beg)
      value = getBinOprValue(leftValue, op_token, rightValue);
    else
      value = getSigOprValue(op_token, rightValue);
  }
  return value;

}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  word_t value = eval(tokens, 0, nr_token);
  *success = true;
  return value;
}
