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

#define TOKEN_LEN_MAX 32
#define NR_TOKEN_MAX 512

typedef struct token {
  int type;
  char str[TOKEN_LEN_MAX];
} Token;

static Token tokens[NR_TOKEN_MAX] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;


static bool isSingleOperator(Token token);

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  // Log("%s\n", e);

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    /* Only deal with the match at begining*/
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        if(substr_len >= TOKEN_LEN_MAX){
          Log("Too long token, tokens least: %s\n", e+position);
        }
        
        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);
        // Log("nr_token:%d\n", nr_token);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: //space, do nothing
            break;
          default:
            strncpy(tokens[nr_token].str, substr_start, substr_len<TOKEN_LEN_MAX ? substr_len: TOKEN_LEN_MAX-1);
            tokens[nr_token].str[substr_len] = '\0';
            tokens[nr_token++].type = rules[i].token_type;
            Assert(nr_token < NR_TOKEN_MAX, "Too much Tokens\n");
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      char ch = e[position];
      printf("%d", ch);
      return false;
    }
  }

  return true;
}

#define print_tokens(tokens, beg, end ) do{\
  Log("Illegal expression: ");  \
  for(int i = beg; i <= end; i++){ \
    printf("%s ", tokens[i].str);\
  }\
  printf("\nbeg: %d end: %d\n",beg, end);\
} while(0)

static int32_t getEndOfParnth(Token* tokens, int beg, int end){
  assert(tokens[beg].type == '(');
  int cnt_left_prntth = 0;
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
  // print_tokens(&token, 0, 0);
  // printf("%d\n", token.type);
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
      if(last == 0){
        Log("Division by zero");
        return 0;
      }
      val = first/last;
      break;
    case TK_EQ :
      val = (first == last);
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

static int32_t prcdcOprtr(Token op_tk,  bool isSigOpr){
  // assert(isBinOperator(op_tk) || isSingleOperator(op_tk));
  if(!isSigOpr){
    assert(isBinOperator(op_tk));
    switch(op_tk.type){
      case TK_EQ:
        return 1;
      case '+':
      case '-':  
        return 2;
      case '*':  
      case '/':
        return 3;
    }
  } else if(isSingleOperator(op_tk)){
    switch(op_tk.type){
      case '-':
        return 6;
    }
  }

  return 0xffff;
  // Assert(0, "Inner Error: get precendence of token: %s\n", op_tk.str);
  // return 0xffff;
}

static int32_t getMainOprtr(Token* tokens, int beg, int end){
// return mainOprtPos if expression is leagle
// return -1 if expression is illeagle
  int32_t mainOprtPos = -1;
  int32_t mainOptrPrcdc = 0xffff;
  printf("----begin---------------\n");
  print_tokens(tokens, beg, end);
  bool isLastNonSpaceTkEndOfExpr = false;
  for(int i = beg; i <= end; ){
    if(tokens[i].type == '(' ){
      i = getEndOfParnth(tokens, i, end);
      isLastNonSpaceTkEndOfExpr = true; // last is ')'
      if(i == -1)
        print_tokens(tokens, beg, end);
    } else if(tokens[i].type == TK_DINT){
      isLastNonSpaceTkEndOfExpr = true; // last is ')'
    } else if(tokens[i].type == TK_NOTYPE){
      ;
    } 
    else {
      
      if(isBinOperator(tokens[i]) || isSingleOperator(tokens[i])){
        // printf("i: %d, tokens[i].str: %s, isLastNonSpaceTkEndOfExpr: %d\n", 
                // i, tokens[i].str, isLastNonSpaceTkEndOfExpr);
        bool isSingle = !isLastNonSpaceTkEndOfExpr;
        if(prcdcOprtr(tokens[i], isSingle) <= mainOptrPrcdc){
          mainOprtPos = i;
          mainOptrPrcdc = prcdcOprtr(tokens[i], isSingle);
        } else if(prcdcOprtr(tokens[i], isSingle) == 0xffff){
            return -1;
        }
      }
      isLastNonSpaceTkEndOfExpr = false;
    }
    i++;
  }
  printf("mainOprtPos: %d %s\n", mainOprtPos, tokens[mainOprtPos].str);
  
  printf("----end---------------\n");

  return mainOprtPos;
  
}

static word_t eval(Token* tokens, int beg, int end, bool* success){
  // print_tokens(tokens, beg, end);
  word_t value = 0;
  Assert(beg <= end, "input of eval is illegal: beg:%d, end:%d\n",beg, end);
  if(beg == end){           //<number>
    if(tokens[beg].type != TK_DINT)
      *success = false;
    value = strtol(tokens[beg].str, NULL, 10);
  }
  else if(tokens[beg].type == '(' && (getEndOfParnth(tokens, beg, end) == end)){
    // '(' <expr> ')'
    value = eval(tokens, beg+1, end-1, success);
  }
  else{
    int32_t mainOptrPos = getMainOprtr(tokens, beg, end);
    if(mainOptrPos < 0 || mainOptrPos >= end){
      // print_tokens(tokens, beg, end);
      
      // assert(0);
      *success = false;
      return 0;
    }
    
    word_t leftValue = 0;
    if(mainOptrPos > beg)
      leftValue = eval(tokens, beg, mainOptrPos-1, success);
    Token op_token = tokens[mainOptrPos];
    // assert(isBinOperator(op_token));

    word_t rightValue = eval(tokens, mainOptrPos+1, end, success);
    // printf("mainOptrPos: %d  ", mainOptrPos);
    // printf("leftValue: %d, op_token: %s, rightValue:%d\n", leftValue, op_token.str, rightValue);
    
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
  *success = true;

  // print_tokens(tokens, 0, nr_token-1);
  word_t value = eval(tokens, 0, nr_token-1, success);
  if(*success)
    return value;
  else{
    print_tokens(tokens, 0, nr_token-1);
    return 0;
  }

  // print_tokens(tokens, 0, nr_token-1);
  return value;

  // return 0;
  
}


// word_t getMainOptr_UT(char *e, bool *success) {
//   if (!make_token(e)) {
//     *success = false;
//     return 0;
//   }
//   /* TODO: Insert codes to evaluate the expression. */
//   int32_t pos = getMainOprtr(tokens, 0, nr_token);
//   printf("%d\n",pos);
//   return 0;
// }