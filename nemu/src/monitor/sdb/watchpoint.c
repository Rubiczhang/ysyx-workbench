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

#include "sdb.h"
#include <common.h>
#include <monitor/sdb/watchpoint.h>

#define NR_WP 32

bool wtchpntWorking = false;

// typedef struct watchpoint {
//   int NO;
//   struct watchpoint *next;
//   /* TODO: Add more members if necessary */
//   char* expr;
//   bool enable;
//   word_t val;
// } WP;


static int nr_freeWtchpnt = NR_WP;

static WP wp_pool[NR_WP] = {};
static WP wp_Lklst[2] = {};
// static WP *head = NULL, *free_ = NULL;
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].expr[0] = '\0';
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].enable = false;
  }
  free_ = &wp_Lklst[0];
  head = &wp_Lklst[1];
  free_->NO = -1;
  head->NO = -1;
  free_->next = wp_pool;
  head->next = NULL;

}

void show_all_free();
void show_all_working();
static void appendLklst(WP* list, WP* newNode){
  Assert(newNode, "Inner Error: newNode should not be NULL");
  Assert(!newNode->next, "Next of NewNode should be empty");
  WP* endNode = list;
  // show_all_working();
  while(endNode->next){
    endNode = endNode->next;
  }
  // show_all_working();
  // newNode->next = NULL;
  endNode->next = newNode;
}

static WP* deletFrst(WP* list){
  //make sure list is not empty
  Assert(list && list->next, "Inner Error: list is empty\n");
  WP* res = list->next;
  list->next = res->next;
  res->next = NULL;
  res->enable = false;
  return res;
}

extern word_t expr(char* e, bool *success);


WP* new_wp(char* e){
  Assert(nr_freeWtchpnt > 0, "Too Much watch point");
  nr_freeWtchpnt--;
  wtchpntWorking = true;
  WP* new_node;

  new_node = deletFrst(free_);
  new_node->enable = true;

// get value 
  bool success; 
    // printf("expression:%s\n", curNode->expr);
  word_t val = expr(e, &success);
  if(!success){
    printf("Wrong Expr: %s\n", e); return NULL;
  }
  new_node->val = val;

  // printf("appendLkLst(head) be called\n");
  appendLklst(head, new_node);
  // printf("appendLkLst(head) returned\n");
  strncpy(new_node->expr, e, MAX_EXPRESSION_LEN);
  // printf("new_node expr:%s\n", new_node->expr);
  // show_all_working();
  return new_node;
}

void free_wp(WP *wp){
  Assert(wp, "Inner Error: should not free a empty node");
  WP* curNode = NULL;
  WP* preNode = NULL;
  for(curNode = head; curNode; curNode = curNode->next){
    if(curNode == wp){
      break;
    }
    preNode = curNode;
  }
  if(curNode == NULL){
    Log("Wrong free");
    return;
  }

  preNode->next = curNode->next;
  curNode->enable = false;
  curNode->next = NULL;
  nr_freeWtchpnt++;
  if(nr_freeWtchpnt == NR_WP) 
    wtchpntWorking = false;
  printf("appendLkLst(free_) returned\n");
}

void free_wp_by_no(int no){
  WP* curNode = NULL;
  WP* preNode = NULL;
  for(curNode = head; curNode; curNode = curNode->next){
    if(curNode->NO == no){
      break;
    }
    preNode = curNode;
  }
  if(curNode == NULL){
    Log("Wrong free, no wathcpoint with NO:%d", no);
    return;
  }

  // curNode->enable = false;
  // printf("appendLkLst(free_) be called\n");
	// show_all_working();

  preNode->next = curNode->next;
  curNode->next = NULL;
  nr_freeWtchpnt++;
  if(nr_freeWtchpnt == NR_WP) 
    wtchpntWorking = false;
  appendLklst(free_, curNode);
	// show_all_working();
  // printf("appendLkLst(free_) returned\n");
  // printf("preNode->NO:%d  curNode->NO:%d\n", preNode->NO, curNode->NO);
}

void show_all_wtchpnt(WP* list){
  printf("%-10s %-6s %s\n", "Num" ,  "Enb", "What");
  const WP* cur = list->next;
  while(cur){
    
    printf("%-10d %-6s %s\n", cur->NO,cur->enable?"y":"n", cur->expr);
    cur = cur->next;
  }
}


void show_all_free(){
  show_all_wtchpnt(free_);
}
void show_all_working(){
  show_all_wtchpnt(head);
}

bool check_wtchpnt_chngd(){  
// output infomation of the first changed watchpnt
  int max_loop = NR_WP+1;
  WP* curNode = head->next;
  WP* chngdNode = NULL;
  bool changed = false;
  word_t val;
  while(curNode && max_loop--){
    bool succ = false;
    val = expr(curNode->expr, &succ);
    // printf("expression:%s\n", curNode->expr);
    Assert(succ, "Wrong expression: %s", curNode->expr);
    if(val != curNode->val){
      changed = true;
      chngdNode = curNode;
      break;
    }
    curNode = curNode->next;
  }
  Assert(max_loop > 0, "Inner Error, Head list without normal tail");
  
  if(changed){
    assert(chngdNode);
    printf("Hardware watchpoint %d: %s\n", chngdNode->NO, chngdNode->expr);
    printf("Old value = %x\n", chngdNode->val);
    chngdNode->val = val;
    printf("New value = %x\n", chngdNode->val);
  }
  return changed;
}
/* TODO: Implement the functionality of watchpoint */

