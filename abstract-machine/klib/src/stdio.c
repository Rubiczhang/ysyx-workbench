#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#define MAX_BUF_LEN (1 << 15)

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// #define STAR_IF_NOT_NATIVE MUXDEF(__NATIVE_USE_KLIB__, *,  )
#if defined (__NATIVE_USE_KLIB__) &&  defined(__ISA_NATIVE__)
#define STAR_IF_NOT_NATIVE 
#else
#define STAR_IF_NOT_NATIVE *
#endif

//leagel place holder format:    %, flag chars,  width(decimal int), place holaders

static const char * validPlchdr = "%csdpux"; //valid place holder

static const char* validFlagChars = "0";
static const int idValidFlagChar[129] = {['0'] = 1};

// static const char* flagVlidPlchrs[] = {"diouxXaAeEfFgG"};
static const char* flagVlidPlchrs[] = {"%csdpux", "dux"};


static int charIdxStr(const char ch, const char* str, const int len){
  int i = 0;
  for(i = 0; i < len; i++){
    if(ch == str[i]){
      break;
    }
  }
  return i;
}

static bool isCharInStr(const char ch, const char* str, const int len){
  return len != charIdxStr(ch, str, len);
}

//in: str, idx, len
//out: idx, width
//idx would be updated to the end
static int readIntFromStr(const char* str, int* idx, int len, int* width){
  int i = *idx;
  int ans = 0;
  if(width) *width = 0;
  while(i < len &&  '0' <= str[i] && str[i] <= '9'){
    ans = ans * 10 + (str[i]-'0');
    i++;
    if(width) (*width)++;
  }
  *idx = i;
  return ans;
}

static bool checkValid(const char* const fmt, const char *validPlchdr){
  //format, valid place holder
  int len = strlen(fmt);
  // int lenPlchdr = strlen(validPlchdr);
  bool lastIsPst = false; //last is %
  for(int i = 0; i < len; i++){
    if(!lastIsPst && fmt[i] == '%'){
      lastIsPst = true;
    }
    else if(lastIsPst == true){
      char flag = '\0';

      // if(strlen(validFlagChars) != charIdxStr(fmt[i], validFlagChars, strlen(validFlagChars))){
      if(isCharInStr(fmt[i], validFlagChars, strlen(validFlagChars))){
        flag = fmt[i];
        i++;
        panic_on(i >= len, "Invalid format place holders\n");
      }
      readIntFromStr(fmt, &i, len, NULL);
      panic_on(i >= len, "Invalid format place holder\n");

      if(!isCharInStr(fmt[i], flagVlidPlchrs[idValidFlagChar[(int)flag]], strlen(flagVlidPlchrs[idValidFlagChar[(int)flag]]))){
        return false;
      }

      lastIsPst = false;
    }
  }
  return true;
}

typedef int(*plchldr_handler)(char* out, int len, va_list STAR_IF_NOT_NATIVE ap);

static int print_ptr(char* out, int len, va_list STAR_IF_NOT_NATIVE ap){
  uintptr_t x = (uintptr_t)va_arg(STAR_IF_NOT_NATIVE ap, void *);
  char buff[30];
  int slen = itoa(x, buff, 16, false);
  len = slen < len? slen: len;
  strncpy(out, buff, len);
  return len;
}

static int print_dint(char* out, int len, va_list STAR_IF_NOT_NATIVE ap){
//in: out
//out: fmt_step, out_step
  int x = va_arg(STAR_IF_NOT_NATIVE ap, int);
  char buff[30];
  int slen = itoa(x, buff, 10, true);
  len = slen < len? slen: len;
  strncpy(out, buff, len);
  return len;
}

static int print_uint(char* out, int len, va_list STAR_IF_NOT_NATIVE ap){
//in: out
//out: fmt_step, out_step
  int x = va_arg(STAR_IF_NOT_NATIVE ap, int);
  char buff[30];
  int slen = itoa(x, buff, 10, false);
  len = slen < len? slen: len;
  strncpy(out, buff, len);
  return len;
}


static int print_hexint(char* out, int len, va_list STAR_IF_NOT_NATIVE ap){
//in: out
//out: fmt_step, out_step
  int x = va_arg(STAR_IF_NOT_NATIVE ap, int);
  char buff[30];
  int slen = itoa(x, buff, 16, false);
  len = slen < len? slen: len;
  strncpy(out, buff, len);
  return len;
}


static int print_str(char* out, int len, va_list STAR_IF_NOT_NATIVE ap){
//in: out
//out: fmt_step, out_step
  char *strBuff;
  strBuff = va_arg(STAR_IF_NOT_NATIVE ap, char *);
  int slen = strlen(strBuff);
  len = slen < len? slen: len;
  strncpy(out, strBuff, len);
  return len;
}

static int print_ch(char* out, int len, va_list STAR_IF_NOT_NATIVE ap){
//in: out
//out: fmt_step, out_step
  char *strBuff;
  strBuff = va_arg(STAR_IF_NOT_NATIVE ap, char *);
  int slen = 1;
  len = slen < len? slen: len;
  strncpy(out, strBuff, len);
  return len;
}

static int print_pst(char* out, int len, va_list STAR_IF_NOT_NATIVE ap){
//in: out
//out: fmt_step, out_step
  out[0] = '%';
  return 1;
}

static struct{
  const char name;
  plchldr_handler handler;
} plchldr_hdlr_table[] = {
  {'x', print_ptr},
  {'d', print_dint},
  {'u', print_uint},
  {'x', print_hexint},
  {'s', print_str},
  {'c', print_ch},
  {'%', print_pst}
};


int printf(const char* const fmt, ...) {

  // panic("Not implemented");
  panic_on(!checkValid(fmt, validPlchdr), "checkNotValid");
  char out[1<<15];
  out[0] = '\0';
  va_list args;
  va_start(args, fmt);
  int len = vsnprintf(out, MAX_BUF_LEN, fmt, args);
  assert(len < (1<<15));
  for(int i = 0; out[i] != '\0'; i++){
    putch(out[i]);
  }
  return len;
}




int vsprintf(char *out, const char* const fmt, va_list ap) {
  return 0;
  // panic_on(!checkValid(fmt, validPlchdr), "checkNotValid");
  // int fmtLen = strlen(fmt);
  // int outInd = 0;
  // // puts("---format:");
  // // puts(fmt);
  // int out_step = 0;
  // int fmt_step = 0;
  // for(int fmtInd = 0 ; fmtInd < fmtLen;){
  //   if(fmt[fmtInd++] != '%'){
  //     out[outInd++] = fmt[fmtInd-1];
  //   }
  //   else{
  //     switch(fmt[fmtInd]){
  //        case 'd':
  //          print_dint(out, &fmt_step, &out_step, ap);
  //          break;
  //       case '%':
  //         out[outInd+1] = '%';
  //         out_step = 1;
  //         fmt_step = 1;
  //         break;
  //       case 's':
  //         print_str(out, &fmt_step, &out_step, ap);
  //         break;
  //       case 'p':
  //         print_ptr(out, &fmt_step, &out_step, ap);
  //         break;
  //       default:
  //         panic("Invalid placeholder");
  //     }
  //     fmtInd += fmt_ste;
  //     outInd += out_step;
  //   }
  // }
  // va_end(ap);
  // out[outInd] = '\0';
  // return outInd;
}


int sprintf(char *out, const char* const fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int len = vsnprintf(out, MAX_BUF_LEN, fmt, args);
  return len;
}




int snprintf(char *out, size_t n, const char* const fmt, ...) {
  panic("Not implemented");
}

static void handle_plchldr(char* out, const char* fmt, int* fmtInd, int* outInd, int* len_to_end, va_list STAR_IF_NOT_NATIVE ap){

  int out_step = 0;
  int fmt_step = 0;
  size_t len_plchldr_hndlr_tb = sizeof(plchldr_hdlr_table)/sizeof(plchldr_hdlr_table[0]);
  int i = 0;
  // for(; i < len_plchldr_hndlr_tb; i++){
  //   if(plchldr_hdlr_table[i].name == fmt[*fmtInd]){
  //     // char flag = '\0';
  //     out_step = plchldr_hdlr_table[i].handler(out+*outInd, *len_to_end, ap);
  //     fmt_step = 1;
  //     break;
  //   }
  // }
  // if(i == len_plchldr_hndlr_tb)
  //   panic("Invalid placeholder");


// Flag chars
  bool zr_padded = false;
  if(isCharInStr(fmt[*fmtInd], validFlagChars, strlen(validFlagChars))){
    if(fmt[*fmtInd] == '0'){
      zr_padded = true;
      // fmt_step++;
      (*fmtInd)++;
    }
    else{
      panic_on(!zr_padded, "Place Flags should be implemeted\n");
    }
  }

// Width chars
  // int fild_width = 0;
  // fild_width = readIntFromStr(fmt, fmtInd, strlen(fmt), NULL);
  readIntFromStr(fmt, fmtInd, strlen(fmt), NULL);


  for(; i < len_plchldr_hndlr_tb; i++){
    if(plchldr_hdlr_table[i].name == fmt[*fmtInd]){
      // char flag = '\0';
      out_step = plchldr_hdlr_table[i].handler(out+*outInd, *len_to_end, ap);
      fmt_step = 1;
      break;
    }
  }
  if(i == len_plchldr_hndlr_tb){
    // printf("\nfmt: %c", fmt[*fmtInd]);
    panic("Invalid placeholder");
  }

  *fmtInd += fmt_step;
  *outInd += out_step;
  *len_to_end -= out_step;
}

int vsnprintf(char *out, size_t n, const char* const fmt, va_list ap) {
  panic_on(!checkValid(fmt, validPlchdr), "checkNotValid");
  int fmtLen = strlen(fmt);
  int outInd = 0;
  // puts("---format:");
  // puts(fmt);
  int len_to_end = n - outInd - 2;      //vsnprintf put at most n chars, including the '\0'
  // char* str;

  // str = va_arg(STAR_IF_NOT_NATIVE ap, char *);
  // str = print_str(out, 100, (va_list*)ap);
  for(int fmtInd = 0 ; fmtInd < fmtLen && len_to_end > 0;){
    assert(len_to_end >= 0);
    if(fmt[fmtInd++] != '%'){
      out[outInd++] = fmt[fmtInd-1];
      len_to_end--;
    }
    else{

#if defined (__NATIVE_USE_KLIB__) &&  defined(__ISA_NATIVE__)
      handle_plchldr(out, fmt, &fmtInd, &outInd, &len_to_end,ap);
#else
      handle_plchldr(out, fmt, &fmtInd, &outInd, &len_to_end, (va_list*)&ap);
#endif
    }
  }
  va_end(ap);
  out[outInd] = '\0';
  return outInd;
}
#endif
