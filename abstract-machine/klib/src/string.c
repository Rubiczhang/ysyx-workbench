#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>
#define MAX_BUF_LEN (1 << 15)

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while(*(s+len) != '\0' && len < MAX_BUF_LEN){
    len++;
  }
  if(len == (MAX_BUF_LEN) ){
    panic("Wrong string at strlen or string is too long");
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  int i = 0;
  for(; i < MAX_BUF_LEN && src[i] != '\0'; i++){
    dst[i] = src[i];
  }
  dst[i] = '\0';
  if(i == MAX_BUF_LEN)
    panic("Wrong usage of strcpy");
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  int i = 0;
  for(; i < n ; i++){
    dst[i] = src[i];
    if(src[i] == '\0')
      break;
  }
  for(; i < n; i++)
    dst[i] = '\0';

  if(i == MAX_BUF_LEN)
    panic("Wrong usage of strncpy");
  return dst;
}

char *strcat(char *dst, const char *src) {
  int lenSrc= strlen(src);
  int lenDst= strlen(dst);
  int j = lenDst;
  for(int i = 0 ; i < lenSrc;){
    dst[j++] = src[i++];
  }
  dst[j] = '\0';
  return dst;
}

// char *strncat(char *dst, const char *src, size_t n) {
//   int lenSrc= strlen(src);
//   int loop_n = lenSrc < n ? lenSrc : n;
//   int lenDst= strlen(dst);
//   int j = lenDst;
//   for(int i = 0 ; i <loop_n;){
//     dst[j++] = src[i++];
//   }
//   dst[j] = '\0';
//   return dst;
// }

int strcmp(const char *s1, const char *s2) {
  int ans = 0;
  int len1 = strlen(s1);
  int len2 = strlen(s2);
  int loop_n = len1 < len2 ? len1 : len2;
  loop_n ++;
  for(int i = 0; i < loop_n; i++){
    if(s1[i] != s2[i]){
      ans = s1[i] - s2[i];
      break;
    }
  }
  return ans;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  int ans = 0;
  int len1 = strlen(s1);
  int len2 = strlen(s2);
  int loop_n = len1 < len2 ? len1 : len2;
  loop_n ++;
  loop_n = loop_n < n ? loop_n : n;
  for(int i = 0; i < loop_n; i++){
    if(s1[i] != s2[i]){
      ans = s1[i] - s2[i];
      break;
    }
  }
  return ans;

}

// static int is_big_little_endian(){        //return 1 if big endian
//   int a = 1;
//   int *p = &a;
//   char *cp = p;
//   if( *cp == *p){
//     return 0;
//   }else{
//     return 1;
//   }
// }

static unsigned char get_int_lower_byte(int x){
  int temp = x << (32-8-1);
  temp = temp >> (32-8-1);
  unsigned char ch = x;
  return ch;
}

void *memset(void *s, int c, size_t n) {
  char *ch_s = (char*)s;
  char ch = get_int_lower_byte(c);
  for(int i = 0; i < n; i++){
    ch_s[i] = ch;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  int ans = 0;
  const char* ch_s1 = (char*)s1;
  const char* ch_s2 = (char*)s2;
  for(int i = 0; i < n; i++){
    if(ch_s1[i] != ch_s2[i]){
      ans = ch_s1[i] - ch_s2[i];
      break;
    }
  }
  return ans;
}

#endif
