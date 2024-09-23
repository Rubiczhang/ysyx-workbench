#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#define MAX_BUF_LEN (1 << 15)

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


static const char * validPlchdr = "%sdp"; //valid place holder

static bool checkValid(const char *fmt, const char *validPlchdr){
  //format, valid place holder
  int len = strlen(fmt);
  int lenPlchdr = strlen(validPlchdr);
  bool lastIsPst = false; //last is %
  for(int i = 0; i < len; i++){
    if(!lastIsPst && fmt[i] == '%'){
      lastIsPst = true;
    }
    else if(lastIsPst == true){
      int j = 0;
      for(; j < lenPlchdr && fmt[i] != validPlchdr[j]; j++)
        ;
      if(j == lenPlchdr){
        return false;
      }
      lastIsPst = false;
    }
  }
  return true;
}

typedef int(*plchldr_handler)(char* out, int len, va_list *ap);

static int print_ptr(char* out, int len, va_list *ap){
  uintptr_t x = (uintptr_t)va_arg(*ap, void *);
  char buff[30];
  int slen = itoa(x, buff, 16);
  len = slen < len? slen: len;
  strncpy(out, buff, len);
  return len;
}

static int print_dint(char* out, int len, va_list *ap){
//in: out
//out: fmt_step, out_step
  int x = va_arg(*ap, int);
  char buff[30];
  int slen = itoa(x, buff, 10);
  len = slen < len? slen: len;
  strncpy(out, buff, len);
  return len;
}


static int print_str(char* out, int len, va_list *ap){
//in: out
//out: fmt_step, out_step
  char *strBuff;
  strBuff = va_arg(*ap, char *);
  int slen = strlen(strBuff);
  len = slen < len? slen: len;
  strncpy(out, strBuff, len);
  return len;
}

static int print_pst(char* out, int len, va_list *ap){
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
  {'s', print_str},
  {'%', print_pst}
};


int printf(const char *fmt, ...) {

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




int vsprintf(char *out, const char *fmt, va_list ap) {
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
  //     fmtInd += fmt_step;
  //     outInd += out_step;
  //   }
  // }
  // va_end(ap);
  // out[outInd] = '\0';
  // return outInd;
}


int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int len = vsnprintf(out, MAX_BUF_LEN, fmt, args);
  return len;
}




int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

static void handle_plchldr(char* out, const char* fmt, int* fmtInd, int* outInd, int* len_to_end, va_list *ap){

  int out_step = 0;
  int fmt_step = 0;
  size_t len_plchldr_hndlr_tb = sizeof(plchldr_hdlr_table)/sizeof(plchldr_hdlr_table[0]);
  int i = 0;
  for(; i < len_plchldr_hndlr_tb; i++){
    if(plchldr_hdlr_table[i].name == fmt[*fmtInd]){
      out_step = plchldr_hdlr_table[i].handler(out+*outInd, *len_to_end, ap);
      fmt_step = 1;
      break;
    }
  }
  if(i == len_plchldr_hndlr_tb)
    panic("Invalid placeholder");
  *fmtInd += fmt_step;
  *outInd += out_step;
  *len_to_end -= out_step;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic_on(!checkValid(fmt, validPlchdr), "checkNotValid");
  int fmtLen = strlen(fmt);
  int outInd = 0;
  // puts("---format:");
  // puts(fmt);
  int len_to_end = n - outInd - 2;      //vsnprintf put at most n chars, including the '\0'
  for(int fmtInd = 0 ; fmtInd < fmtLen && len_to_end > 0;){
    assert(len_to_end >= 0);
    if(fmt[fmtInd++] != '%'){
      out[outInd++] = fmt[fmtInd-1];
      len_to_end--;
    }
    else{
      handle_plchldr(out, fmt, &fmtInd, &outInd, &len_to_end, &ap);
    }
  }
  va_end(ap);
  out[outInd] = '\0';
  return outInd;
}
#endif
