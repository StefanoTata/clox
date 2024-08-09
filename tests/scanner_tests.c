#pragma warning(disable:4996)

#define SCANNER_IMPL
#include "scanner.h"

#define ARENA_IMPL
#include "arena.h"

#include "containers.h"

int main(){
  
  Arena a = {0};
  unsigned char b[256];
  arena_init(&a, b, 256);

  Token t0 = {0, "t0", 0}; 
  char *str = token_to_string(&t0, &a);
  assert(strcmp(str, "0 t0") == 0);
  
  Token t1 = {1, "t1", 0};
  Token t2 = {2, "t2", 0};

  TokenNode n0 = {.token=&t0}, n1 = {.token=&t1}, n2 = {.token=&t2};
  add_node(&n0, &n1);
  add_node(&n1, &n2);
  
  int i=0;
  list_for_each(p, &n0.list) i++;
  assert(i==3);
  
  i = 0;
  list_for_each(p, &n0.list){
    TokenNode* el = get_node(p, TokenNode);
    assert(el->token->type == i++);
  }

  TokenNode* token_list_head = arena_alloc(&a, sizeof(TokenNode));;
  scanner("();", token_list_head, &a);
  
  list_for_each(p, token_list_head->list.next){
    TokenNode* el = get_node(p, TokenNode);
    printf("-> %d\n", el->token->type);
  }
  
  return 0;
}
