#ifndef __WATCHPOINT__H__
#define __WATCHPOINT__H__
#define MAX_EXPRESSION_LEN 2048
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  /* TODO: Add more members if necessary */
  char expr[MAX_EXPRESSION_LEN];
  bool enable;
  word_t val;
} WP;


WP* new_wp(char* e);
void free_wp_by_no(int no);
bool check_wtchpnt_chngd();
void show_all_working(void);
#endif