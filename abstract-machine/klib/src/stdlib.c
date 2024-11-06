#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int itoa(int x, char* str, int base, bool isSigned){
  if(x == 0){
    str[0] = '0';
    str[1] = '\0';
    return 1;
  }
  //return the number of converted digits.
  bool negFlag = false;
  if(isSigned){
    negFlag= (x < 0);
    x = abs(x);
  }
  int len = 0;
  while(x != 0){
    char ascii_base = (x % base < 10)? '0' : ('A'-10);
    str[len++] = x % base +  ascii_base;
    x = x / base;
  }
  if(isSigned && negFlag) str[len++] = '-';
  for(int j = 0; j < len/2; j++){
    char temp = str[j];
    str[j] = str[len-j - 1]; 
    str[len-j-1] = temp;
  }
  str[len] = '\0';
  return len;
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  panic("Not implemented");
#endif
  return NULL;
}

void free(void *ptr) {
}

#endif
