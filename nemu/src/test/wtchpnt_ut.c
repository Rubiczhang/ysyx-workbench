#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  /* TODO: Add more members if necessary */
  char* expr;

} WP;

void init_wp_pool();
void free_wp(WP*wp);
void free_wp_by_no(int no);


WP* new_wp(char* expr);
void show_all_free();
void show_all_working();

static void test1(){

    new_wp("1+1==2");
    // show_all_free();
	  // show_all_working();
    new_wp("3+3");
    // show_all_free();
	  // show_all_working();
    new_wp("4+4");
    // show_all_free();
	  // show_all_working();
    free_wp_by_no(0);
    // show_all_free();
	  // show_all_working();
    free_wp_by_no(2);
    show_all_free();
	  show_all_working();
}

// static void test2(){
//   new_wp("1+1==2");
//   free_wp_by_no(2);
//   show_all_free();
// 	show_all_working();
// }


void wtchpnt_ut(void){
  test1();
  // test2();
}